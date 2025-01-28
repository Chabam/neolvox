/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)
              Alexandre Piboule (ONF)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#ifndef CT_MATHFITTEDLINE2D_H
#define CT_MATHFITTEDLINE2D_H

#include "ctlibmath_global.h"
#include "qmap.h"
#include "ct_mathpoint.h"

#include <Eigen/Core>

class CTLIBMATH_EXPORT CT_MathFittedLine2D
{
public:
    CT_MathFittedLine2D(const QList<Eigen::Vector2d*>& l_p)
    {
        _a = 0;
        _b = 0;
        _ymean = 0;

        const int nb_points = l_p.size();

        if (nb_points > 1)
        {
            _ymean = 0;
            double x_moyenne = 0;

            double xi, yi, yi_est;

            double somme_produit_xi_yi = 0;

            int n = 0;

            // somme des x et y
            while(n < nb_points)
            {
                Eigen::Vector2d* point = l_p.at(n);
                xi = (*point)(0);
                yi = (*point)(1);

                somme_produit_xi_yi += (xi * yi);

                _ymean += yi;
                x_moyenne += xi;

                ++n;
            }

            // moyennes
            _ymean /= ((double)nb_points);
            x_moyenne /= ((double)nb_points);

            // regression lineaire :

            // calcul de a :
            double a_denum = 0;

            n = 0;

            while(n < nb_points)
            {
                Eigen::Vector2d* point = l_p.at(n);

                double xi_moins_moyenne = (*point)(0) - x_moyenne;
                a_denum += (xi_moins_moyenne*xi_moins_moyenne);

                ++n;
            }

            _a = ((somme_produit_xi_yi)-(nb_points * x_moyenne * _ymean))/a_denum;

            // calcul de b0
            _b = _ymean - ((_a) * x_moyenne);

            n = 0;

            while(n < nb_points)
            {
                Eigen::Vector2d* point = l_p.at(n);
                yi_est = (_a*(*point)(0) + _b);

                _predictedValues.insert(point, yi_est);
                ++n;
            }
        }
    }

    ~CT_MathFittedLine2D()
    {
        _predictedValues.clear();
    }

    /*!
     * \brief renvoie le nombre de points de la regression linéaire
     *
     * \return (int) nombre de points
     */
    inline int _n() {return _predictedValues.size();}

    /*!
     * \brief Calcule le R2 de la régression linéaire
     *
     * \return  (double) R2
     */
    double getR2()
    {
        if (_n() < 2) {return 0;}

        // calcul des sommes de carres
        double sce = 0;
        double sct = 0;

        QMapIterator<Eigen::Vector2d*, double> it(_predictedValues);
        while(it.hasNext())
        {
            it.next();
            Eigen::Vector2d* point = it.key();
            double yi = (*point)(1);
            double yi_est = it.value();

            sct += (yi - _ymean)*(yi - _ymean);
            sce += (yi_est - _ymean)*(yi_est - _ymean);
        }

        return sce/sct;
    }

    /*!
     * \brief Calcule le R2 ajusté de la régression linéaire
     *
     * \return  (double) R2 ajusté
     */
    double getAdjustedR2()
    {
        return (1- (((_n() - 1)/(_n() - 2))*(1 - getR2())));
    }

    /*!
     * \brief Calcule la MSE de la régression linéaire
     *
     * \return (double) MSE
     */
    double getMSE()
    {
        if (_n() < 2) {return 0;}

        // calcul des sommes de carres
        double scr = 0;

        QMapIterator<Eigen::Vector2d*, double> it(_predictedValues);
        while(it.hasNext())
        {
            it.next();
            Eigen::Vector2d* point = it.key();
            double yi = (*point)(1);
            double yi_est = it.value();

            scr += (yi - yi_est)*(yi - yi_est);
        }

        return scr/(_n() - 2);
    }

    /*!
     * \brief Calcule la RMSE de la régression linéaire
     *
     * \return (double) RMSE
     */
    inline double getRMSE()
    {
        if (_n() < 2) {return 0;}
        return sqrt(getMSE());
    }


    /*!
     * \brief Calcule l'erreur maximale pour un point
     *
     * \param  (Eigen::Vector2d*) Point générant l'erreur maximale
     * \return (double) Valeur absolue de l'erreur maximale
     */
    double getMaxError(Eigen::Vector2d* errorPoint = nullptr)
    {
        // 'errorPoint' is used for returning pointer, not as an input, so we disable warning.
        Q_UNUSED(errorPoint);

        if (_n() < 2) {return 0;}

        double maxError = 0;

        QMapIterator<Eigen::Vector2d*, double> it(_predictedValues);
        while(it.hasNext())
        {
            it.next();
            Eigen::Vector2d* point = it.key();
            double yi = (*point)(1);
            double yi_est = it.value();

            double error = fabs(yi - yi_est);

            if (error > maxError)
            {
                maxError = error;
                errorPoint = point;
            }
        }

        return maxError;
    }

    /*!
     * \brief Calcule les résidus de la régression linéaire
     *
     * \param  (QMap<Eigen::Vector2d*,double>) Points de la régression avec leurs résidus
     */

    void getResiduals(QMap<Eigen::Vector2d *, double> &residuals)
    {
        residuals.clear();
        if (_n() < 2) {return;}

        // Evaluation successive de chaque point
        QMapIterator<Eigen::Vector2d*, double> it(_predictedValues);
        while (it.hasNext())
        {
            it.next();
            Eigen::Vector2d* point = it.key();

            double residual = (*point)(1) - it.value();

            residuals.insert(point, residual);
        }
    }

    /*!
     * \brief Calcule les distances de cook pour les points de la régression linéaire
     *
     * \param  (QMap<Eigen::Vector2d*,double>) Points de la régression avec leurs distances de cook
     */
    void getCookDistances(QMap<Eigen::Vector2d *, double> &cook)
    {
        cook.clear();
        if (_n() < 2) {return;}

        // Evaluation successive de chaque point
        QMapIterator<Eigen::Vector2d*, double> it(_predictedValues);
        while (it.hasNext())
        {
            it.next();
            Eigen::Vector2d* point = it.key();

            // copie de la liste des points
            QList<Eigen::Vector2d*> l_p_out = _predictedValues.keys();
            // suppression du point en cours d'examen
            l_p_out.removeOne(point);

            // Ajustement de la droite sans le point en cours d'examen
            CT_MathFittedLine2D fitted_out(l_p_out);

            // Calcul de la distance de cook pour le point en cours d'examen
            double cook_dist = 0;

            QMapIterator<Eigen::Vector2d*, double> it_out(fitted_out._predictedValues);
            while (it_out.hasNext())
            {
                it_out.next();
                cook_dist += pow(it.value() - it_out.value(), 2);
            }

            cook_dist = cook_dist/(2*getMSE());
            cook.insert(point, cook_dist);
        }
    }


    double                            _a;                 /*!< Pente de la régression linéaire */
    double                            _b;                 /*!< Ordonnée à l'origine de la régression linéaire */
    QMap<Eigen::Vector2d*, double>    _predictedValues;	 /*!< Map<Points, Valeurs prédites> */
    double                            _ymean;             /*!< Valeur moyenne des y> */

};

#endif // CT_MATHFITTEDLINE2D_H
