#include "lvox3_interpolatekrigingnormal.h"
#include "tools/lvox3_errorcode.h"
#include "tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.h"
#include "tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.h"
#include "tools/traversal/propagation/expfit.h"
#include <Eigen/Dense>
#include <QObject>
#include "tools/lvox3_gridtype.h"

LVOX3_InterpolateKrigingNormal::LVOX3_InterpolateKrigingNormal(const lvox::Grid3Df* originalDensityGrid,
                                                   const lvox::Grid3Di* beforeGrid,
                                                   const lvox::Grid3Di* theoriticalsGrid,
                                                   lvox::Grid3Df* const outDensityGrid,
                                                   double radius, qint32 effectiveRayThreshold)
{
    m_originalDensityGrid = (lvox::Grid3Df*)originalDensityGrid;
    m_outDensityGrid = (lvox::Grid3Df*)outDensityGrid;
    m_beforeGrid = (lvox::Grid3Di*)beforeGrid;
    m_theoriticalsGrid = (lvox::Grid3Di*)theoriticalsGrid;
    m_radius = radius;
    m_effectiveRayThreshold = effectiveRayThreshold;
}

void LVOX3_InterpolateKrigingNormal::doTheJob()
{
    krigepointNormal();
}

void LVOX3_InterpolateKrigingNormal::krigepointNormal()
{
    for(size_t ref = 0; ref < m_originalDensityGrid->nCells(); ref++)
    {
        if(m_originalDensityGrid->valueAtIndex(ref) == -1)
        {
            Eigen::Vector3d coordsBefore;
            m_originalDensityGrid->getCellCenterCoordinates(ref,coordsBefore);

            double xBefore = coordsBefore.x();
            double yBefore = coordsBefore.y();
            double zBefore = coordsBefore.z();

            int num_points = 0;

            Eigen::MatrixXd temppoints(m_originalDensityGrid->nCells(),5);

            for(size_t p = 0; p < m_originalDensityGrid->nCells(); p++)
            {
                Eigen::Vector3d coordsLocal;
                m_originalDensityGrid->getCellCenterCoordinates(p,coordsLocal);

                double xLocal = coordsLocal.x();
                double yLocal = coordsLocal.y();
                double zLocal = coordsLocal.z();

                double dx = coordsBefore.x() - xLocal;
                double dy = coordsBefore.y() - yLocal;
                double dz = coordsBefore.z() - zLocal;

                double distance = sqrt(dx*dx + dy*dy + dz*dz);


                if((distance < m_radius)&&(m_originalDensityGrid->valueAtIndex(p)>=0)&&m_originalDensityGrid->valueAtIndex(p)<=1)
                {
                    temppoints(num_points,0) = xLocal;
                    temppoints(num_points,1) = yLocal;
                    temppoints(num_points,2) = zLocal;
                    temppoints(num_points,3) = m_originalDensityGrid->valueAtIndex(p);
                    temppoints(num_points,4) = m_theoriticalsGrid->valueAtIndex(p) - m_beforeGrid->valueAtIndex(p);

                    num_points++;
                }
            }

            if(num_points == 0)
            {
                m_outDensityGrid->setValueAtIndex(ref,0);
            }
            else
            {
                Eigen::MatrixXd points(num_points,5);
                Eigen::MatrixXd pointsWithRef(num_points+1,5);

                for(int j = 0; j<num_points; j++)
                {
                    points(j,0) = temppoints(j,0);
                    points(j,1) = temppoints(j,1);
                    points(j,2) = temppoints(j,2);
                    points(j,3) = temppoints(j,3);
                    points(j,4) = temppoints(j,4);
                    pointsWithRef(j,0) = temppoints(j,0);
                    pointsWithRef(j,1) = temppoints(j,1);
                    pointsWithRef(j,2) = temppoints(j,2);
                    pointsWithRef(j,3) = temppoints(j,3);
                    pointsWithRef(j,4) = temppoints(j,4);
                }

                pointsWithRef(num_points,0) = xBefore;
                pointsWithRef(num_points,1) = yBefore;
                pointsWithRef(num_points,2) = zBefore;
                pointsWithRef(num_points,3) = m_originalDensityGrid->valueAtIndex(ref);
                pointsWithRef(num_points,4) = m_theoriticalsGrid->valueAtIndex(ref) - m_beforeGrid->valueAtIndex(ref);
                //variogramme expérimental

                //ndistances bruteforce
                int ndistances=0;
                bool flag;
                Eigen::VectorXd tempdistances(num_points*num_points);
                for(int i =0; i<num_points*num_points;i++)
                {
                    tempdistances[i]=-1;
                }
                for(int i = 0; i<num_points; i++)
                {
                    for(int j = 0; j<num_points; j++)
                    {
                        flag = 0;
                        double dx = points(i,0) - points(j,0);
                        double dy = points(i,1) - points(j,1);
                        double dz = points(i,2) - points(j,2);
                        double distance = sqrt(dx*dx + dy*dy + dz*dz);
                        for(int k=0;k<num_points*num_points&&flag==0;++k)
                        {
                            if(tempdistances[k] == distance)
                            {
                                flag = 1;
                            }
                        }
                        if(flag==0)
                        {
                            tempdistances[ndistances] = distance;
                            ++ndistances;
                        }
                    }
                }

                if(ndistances <= 1)
                {
                    m_outDensityGrid->setValueAtIndex(ref,0);
                }
                else
                {

                    Eigen::VectorXd distances(ndistances);
                    for(int i = 0; i<ndistances;i++)
                    {
                        distances(i)= -1;
                    }
                    Eigen::MatrixXd gamma(ndistances,2);
                    for(int i = 0; i<ndistances;i++)
                    {
                        gamma(i,0)=0;
                        gamma(i,1)=0;
                    }
                    int lastDistanceLocation = 0;
                    for(int i = 0; i<num_points-1; i++)
                    {
                        for(int j = i; j<num_points; j++)
                        {
                            flag = 0;
                            double dx = points(i,0) - points(j,0);
                            double dy = points(i,1) - points(j,1);
                            double dz = points(i,2) - points(j,2);
                            double distance = sqrt(dx*dx + dy*dy + dz*dz);

                            for(int k=0;k<ndistances&&flag==0;++k)
                            {
                                if(distances[k] == distance)
                                {
                                    gamma(k,0) += pow((points(i,3)-points(j,3)),2);
                                    ++gamma(k,1);
                                    flag = 1;
                                }
                            }
                            if(flag==0)
                            {
                                distances[lastDistanceLocation] = distance;
                                gamma(lastDistanceLocation,0) += pow((points(i,3)-points(j,3)),2);

                                ++gamma(lastDistanceLocation,1);
                                ++lastDistanceLocation;
                            }
                        }
                    }

                    Eigen::MatrixXd Vgexp(ndistances,2);
                    for(int i=0; i<ndistances; i++)
                    {
                        Vgexp(i,1) = gamma(i,0)/(gamma(i,1));
                        Vgexp(i,0) = distances[i];
                    }

                    //Sorting Vgexp by the first colums value
                    int j;
                    int min_idx;
                    for(int i = 0; i<ndistances-1;i++)
                    {
                        min_idx = i;
                        for(j=i+1;j < ndistances; j++)
                        {
                            if(Vgexp(j,0) < Vgexp(min_idx,0))
                            {
                                min_idx = j;
                            }
                        }
                        double temp1 = Vgexp(min_idx,0);
                        double temp2 = Vgexp(min_idx,1);

                        Vgexp(min_idx,0) = Vgexp(i,0);
                        Vgexp(min_idx,1) = Vgexp(i,1);

                        Vgexp(i,0) = temp1;
                        Vgexp(i,1) = temp2;
                    }

                    double sumd =0;
                    for(int i = 0; i<ndistances;i++)
                    {
                        sumd += Vgexp(i,1);
                    }

                    if(sumd == 0)
                    {
                        m_outDensityGrid->setValueAtIndex(ref,0);
                    }
                    else
                    {
                        //exp model fitting
                        double param[3];
                        fit(param, Vgexp, ndistances);

                        double range = -1/param[2];
                        double sill = -param[1];
                        double nugget = param[0]-sill;

                        //Fill V matrix
                        Eigen::MatrixXd V(num_points,num_points);
                        for(int c = 0; c < num_points; c++)
                        {
                            for(int d = 0; d < num_points; d++)
                            {
                                V(c,d) = variogram(points, c, d, nugget, range, sill);
                            }
                        }

                        //Fill v
                        Eigen::VectorXd v(num_points);
                        for(int c = 0; c < num_points; c++)
                        {
                            v(c) = variogram(pointsWithRef, num_points, c, nugget, range, sill);
                        }

                        Eigen::VectorXd out;
                        out = V.fullPivLu().solve(v);

                        double output = 0;
                        for(int c = 0; c < num_points; c++)
                        {
                            output += points(c,3) * out(c);
                        }

                        if(output <= 0)
                        {
                            m_outDensityGrid->setValueAtIndex(ref, 0);
                        }
                        else
                        {
                            m_outDensityGrid->setValueAtIndex(ref, output);
                        }
                    }
                }
            }
        }
    }
}


double LVOX3_InterpolateKrigingNormal::variogram(Eigen::MatrixXd points, int a, int b, double nugget, double range, double sill)
{
  double d = anisotropic_distance(points, a, b);

  double r = nugget + range*(1 - exp(-(d/sill)));
  return r;
}

double LVOX3_InterpolateKrigingNormal::anisotropic_distance(Eigen::MatrixXd points, int a, int b){
  double dx = points(a,0)- points(b,0);
  double dy = points(a,1)- points(b,1);
  double dz = points(a,2)- points(b,2);

    return sqrt(dx*dx + dy*dy + dz*dz);
}

