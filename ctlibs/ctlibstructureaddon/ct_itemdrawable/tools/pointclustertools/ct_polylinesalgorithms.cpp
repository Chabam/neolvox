#include "ct_polylinesalgorithms.h"

#include "ct_pointcloudindex/ct_pointcloudindexvector.h"

#include "ct_math/ct_mathpoint.h"
#include "ct_math/ct_math2dlines.h"
#include "ct_accessor/ct_pointaccessor.h"
#include "ct_iterator/ct_pointiterator.h"

#include <limits>

void CT_PolylinesAlgorithms::createPolyline2D(const CT_PointCluster *baseCluster, CT_PointCluster *polylineCluster)
{
    const CT_AbstractPointCloudIndex* baseIndexCloud = baseCluster->pointCloudIndex();
    const CT_AbstractPointCloudIndex* polylineIndexCloud = polylineCluster->pointCloudIndex();

    QList<CandidatePoint*> unMarked;
    CandidatePoint* current;

    /////////////////////////////////////////////////////////////////////////////////////
    // Initialisation de unMarked ///////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

    size_t sIC = baseIndexCloud->size();

    for ( size_t i = 0 ; i < sIC; i++ )
    {
        current = new CandidatePoint();
        current->indexInCloud = baseIndexCloud->indexAt(i);
        current->indexOfMinDistance = std::numeric_limits<size_t>::max();
        current->distanceToLine = std::numeric_limits<double>::max();
        unMarked.push_back(current);
    }

    if (unMarked.size() <= 0) {return;}

    // On retire le premier element et on l'ajoute a la polyligne
    current = unMarked.takeFirst();
    polylineCluster->addPoint(current->indexInCloud);

    CT_PointAccessor pAccess1;
    CT_PointAccessor pAccess2;

    /////////////////////////////////////////////////////////////////////////////////////
    // Prise en compte de chaque point candidat//////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    while ( !unMarked.empty() )
    {
        /////////////////////////////////////////////////////////////////////////////////
        // Mise  jour des distances ////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////

        // current est le point que je viens d'ajouter a ma polyligne
        // Pour tous les points non marques
        for ( int i = 0 ; i < unMarked.size() ; i++ )
        {
            CandidatePoint* candidate = unMarked.at(i);

            const CT_Point &p1 = pAccess1.constPointAt(candidate->indexInCloud);
            const CT_Point &p2 = pAccess2.constPointAt(current->indexInCloud);

            // Distance a l'objet qu'on vient d'ajouter pour ensuite la comparer a la distance de l'ancienne tete et de l'ancienne queue de ma polyligne
            double distance = CT_MathPoint::distance2D (p1, p2);

            // Si ils pointaient vers un point diffrent de celui de current
            if ( current->indexOfMinDistance != candidate->indexOfMinDistance )
            {

                if (distance <= candidate->distanceToLine)
                {
                    // MAJ de la distance et on pointe vers le meme point que celui pointe par current
                    candidate->distanceToLine = distance;
                    candidate->indexOfMinDistance = current->indexInCloud;
                }
            }

            else
            {
                double distanceToOtherEnd;

                // Comparaison a la tete
                if ( current->indexInCloud == polylineIndexCloud->first())
                {
                    const CT_Point &pA = pAccess1.constPointAt(candidate->indexInCloud);
                    const CT_Point &pB = pAccess2.constPointAt(polylineIndexCloud->last());

                    distanceToOtherEnd = CT_MathPoint::distance2D(Eigen::Vector3d(pA(0), pA(1), pA(2)), Eigen::Vector3d(pB(0), pB(1), pB(2)));

                    if ( distance < distanceToOtherEnd )
                    {
                        candidate->distanceToLine = distance;
                        candidate->indexOfMinDistance = current->indexInCloud;
                    }

                    else
                    {
                        candidate->distanceToLine = distanceToOtherEnd;
                        candidate->indexOfMinDistance = polylineIndexCloud->last();
                    }
                }

                // Comparaison a la queue
                else if ( current->indexInCloud == polylineIndexCloud->last())
                {
                    const CT_Point &pA = pAccess1.constPointAt(candidate->indexInCloud);
                    const CT_Point &pB = pAccess2.constPointAt(polylineIndexCloud->first());

                    distanceToOtherEnd = CT_MathPoint::distance2D(Eigen::Vector3d(pA(0), pA(1), pA(2)), Eigen::Vector3d(pB(0), pB(1), pB(2)));

                    if ( distance < distanceToOtherEnd )
                    {
                        candidate->distanceToLine = distance;
                        candidate->indexOfMinDistance = current->indexInCloud;
                    }

                    else
                    {
                        candidate->distanceToLine = distanceToOtherEnd;
                        candidate->indexOfMinDistance = polylineIndexCloud->first();
                    }
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////
        // Ajout  la polyligne /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////
        int		indexMin = 0;
        double	distMin = std::numeric_limits<double>::max();

        // On cherche le point le plus proche de l'actuelle polyligne
        for ( int i = 0 ; i < unMarked.size() ; i++ )
        {
            if ( unMarked[i]->distanceToLine < distMin )
            {
                distMin = unMarked[i]->distanceToLine;
                indexMin = i;
            }
        }

        // Si la liste est vide on l'ajoute simplement
        if ( polylineIndexCloud->size() <= 0)
            polylineCluster->addPoint(unMarked[indexMin]->indexInCloud);

        else
        {
            // Sinon on regarde de quel cote il faut l'ajouter
            if ( polylineIndexCloud->last() == unMarked[indexMin]->indexOfMinDistance )
                polylineCluster->addPoint(unMarked[indexMin]->indexInCloud);
            else
                polylineCluster->addPoint(unMarked[indexMin]->indexInCloud, false, true);
        }

        // Suppression du current dans la liste des non marques
        delete current;
        current = unMarked.takeAt(indexMin);

    }
    delete current;
}

Eigen::Vector2d CT_PolylinesAlgorithms::compute2DArcData(const CT_PointCluster *polyline, double &sagitta, double &chord, double &radius)
{
    const CT_AbstractPointCloudIndex* indexCloud = polyline->pointCloudIndex();

    CT_PointIterator itP(indexCloud);

    if(itP.size() > 0) {
        const CT_Point &first = itP.next().currentPoint();
        itP.toBack();
        const CT_Point &last = itP.previous().currentPoint();

        Eigen::Vector2d v1(first(0), first(1));
        Eigen::Vector2d v2(last(0),  last(1));
        Eigen::Vector2d v3((first(0) + last(0))/2, (first(1) + last(1))/2);
        Eigen::Vector2d v4;

        chord = CT_MathPoint::distance2D(first, last);
        // Calcul de la perpendiculaire à la corde
        CT_Math2DLines::computePerpendicularSegment(v1, v2, v3, v4, -1, false);

        // recherche des intersections entre les segments successifs de l'arc et la médiatrice de la corde pour déterminer la flèche
        sagitta = 0;
        double sagittacw  = 0;
        double sagittaccw = 0;
        int nbcw = 0;
        int nbccw = 0;

        itP.toFront();

        while(itP.hasNext())
        {
            const CT_Point &p1 = itP.next().currentPoint();

            if(itP.hasNext()) {
                const CT_Point &p2 = itP.next().currentPoint();

                Eigen::Vector2d v5(p1(0), p1(1));
                Eigen::Vector2d v6(p2(0), p2(1));
                Eigen::Vector2d intersection;
                double r, s;

                if (CT_Math2DLines::intersectSegments(v3, v4, v5, v6, r, s, intersection))
                {
                    if (s>=0 && s <= 1)
                    {
                        if (r<0)
                        {
                            sagittacw += CT_Math2DLines::distance2D(v3, intersection);
                            ++nbcw;
                        } else {
                            sagittaccw += CT_Math2DLines::distance2D(v3, intersection);
                            ++nbccw;
                        }
                    }
                }
            }
        }

        if (nbcw > 0) {sagittacw /= nbcw;}
        if (nbccw > 0) {sagittaccw /= nbccw;}

        bool cw = (sagittacw > sagittaccw);
        if (cw) {
            sagitta = sagittacw;
        } else {
            sagitta = sagittaccw;
        }

        if (sagitta > (chord/2))
        {
            sagitta = chord*chord/(4*sagitta);
            cw = !cw;
        }

        // calcul du rayon du cercle
        radius = ((4*sagitta*sagitta) + chord*chord)/(8*sagitta);


        // Calcul du centre du cercle
        Eigen::Vector2d circleCenter;
        CT_Math2DLines::computePerpendicularSegment(v1, v2, v3, circleCenter, (radius-sagitta), !cw);
        return circleCenter;
    }

    return Eigen::Vector2d();
}

