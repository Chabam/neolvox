#include "lvox3_interpolatekrigingbinomial.h"
#include "tools/lvox3_errorcode.h"
#include "tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.h"
#include "tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.h"
#include "tools/traversal/propagation/expfit.h"
#include <Eigen/Dense>
#include <QObject>
#include "tools/lvox3_gridtype.h"

LVOX3_InterpolateKrigingBinomial::LVOX3_InterpolateKrigingBinomial(const lvox::Grid3Df* originalDensityGrid,
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

void LVOX3_InterpolateKrigingBinomial::doTheJob()
{
    krigepointBinomial();
}

void LVOX3_InterpolateKrigingBinomial::krigepointBinomial()
{
    setProgressRange(0, m_originalDensityGrid->nCells());
    for(size_t ref = 0; ref < m_originalDensityGrid->nCells(); ref++)
    {
        //If voxel value does not have an intensity
        if(m_originalDensityGrid->valueAtIndex(ref) == -1)
        {
            Eigen::Vector3d coordsBefore;
            m_originalDensityGrid->getCellCenterCoordinates(ref,coordsBefore);

            double xBefore = coordsBefore.x();
            double yBefore = coordsBefore.y();
            double zBefore = coordsBefore.z();

            int num_points = 0;
            Eigen::MatrixXd temppoints(int(m_originalDensityGrid->nCells()),5);

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
            //new matrice with the right size
            Eigen::MatrixXd points(int(num_points+1),5);

            for(int j = 0; j<num_points; j++)
            {
                points(j,0) = temppoints(j,0);
                points(j,1) = temppoints(j,1);
                points(j,2) = temppoints(j,2);
                points(j,3) = temppoints(j,3);
                points(j,4) = temppoints(j,4);
            }

            points(num_points,0) = xBefore;
            points(num_points,1) = yBefore;
            points(num_points,2) = zBefore;
            points(num_points,3) = m_originalDensityGrid->valueAtIndex(ref);
            points(num_points,4) = m_theoriticalsGrid->valueAtIndex(ref) - m_beforeGrid->valueAtIndex(ref);

            if(num_points == 0)
            {
                m_outDensityGrid->setValueAtIndex(ref,0);
            }
            else
            {
                double sum = 0;
                for(int i = 0; i<num_points;i++)
                {
                    sum += points(i,3);
                }
                if(sum ==0)
                {
                    m_outDensityGrid->setValueAtIndex(ref,0);
                }
                else
                {
                    double pi = 0;
                    double sigma = 0;
                    sum = 0;
                    for(int i=0; i<num_points; i++)
                    {
                        if(points(i,4) > m_effectiveRayThreshold && points(i,3)>=0)
                        {
                            pi += points(i,3);
                            sum++;
                        }
                    }
                    pi /= sum;

                    sum =0;
                    for(int i=0; i<num_points; i++)
                    {
                        if(points(i,4) > m_effectiveRayThreshold && points(i,3)>=0)
                        {
                            sigma += pow(points(i,3),2);
                            sum++;
                        }
                    }
                    sigma = sigma/sum - pow(pi,2);

                    int ndistances = 0;
                    bool flag;

                    std::vector <double> tempdistances(num_points*2);
                    for(int i =0; i<num_points*2;i++)
                    {
                        tempdistances[i]= -1;
                    }

                    for(int i = 0; i<num_points; i++)
                    {
                        if(points(i,3) >=0 && points(i,4) > m_effectiveRayThreshold)
                        {
                            for(int j = 0; j<num_points; j++)
                            {
                                if(points(j,3) >=0 && points(j,4) > m_effectiveRayThreshold)
                                {
                                    flag = 0;
                                    double dx = points(i,0) - points(j,0);
                                    double dy = points(i,1) - points(j,1);
                                    double dz = points(i,2) - points(j,2);

                                    double distance = sqrt(dx*dx + dy*dy + dz*dz);

                                    for(int k=0;k<num_points*2 && flag == 0 && distance<m_radius;++k)
                                    {
                                        if(tempdistances[k] == distance)
                                        {
                                            flag = 1;
                                        }
                                    }
                                    if(flag == 0 && distance<m_radius)
                                    {
                                        tempdistances[ndistances] = distance;
                                        ++ndistances;
                                    }
                                }
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
                        for(int i =0; i<ndistances;i++)
                        {
                            distances(i)= -1;
                        }
                        Eigen::MatrixXd gamma(ndistances,2);
                        for(int i =0; i<ndistances;i++)
                        {
                            gamma(i,0)=0;
                            gamma(i,1)=0;
                        }
                        int lastDistanceLocation = 0;

                        for(int i = 0; i < num_points; i++)
                        {
                            if(points(i,3) >= 0 && points(i,4) > m_effectiveRayThreshold)
                            {
                                for(int j = 0; j < num_points; j++)
                                {
                                    if(points(j,3) >= 0 && points(j,4) > m_effectiveRayThreshold)
                                    {
                                        flag = 0;
                                        double dx = points(i,0) - points(j,0);
                                        double dy = points(i,1) - points(j,1);
                                        double dz = points(i,2) - points(j,2);
                                        double distance = sqrt(dx*dx + dy*dy + dz*dz);
                                        for(int k=0; k < ndistances && flag == 0 && distance < m_radius;++k)
                                        {
                                            if(distances[k] == distance)
                                            {
                                                gamma(k,0) += (points(i,4)*points(j,4))/(points(i,4)+points(j,4))*pow(points(j,3)-points(i,3),2) - pi*(1-pi) + sigma;
                                                gamma(k,1) += (points(i,4)*points(j,4))/(points(i,4)+points(j,4));
                                                flag = 1;
                                            }
                                        }
                                        if(flag == 0 && distance < m_radius)
                                        {
                                            distances[lastDistanceLocation] = distance;
                                            gamma(lastDistanceLocation,0) += (points(i,4)*points(j,4))/(points(i,4)+points(j,4))*pow(points(j,3)-points(i,3),2) - pi*(1-pi) + sigma;
                                            gamma(lastDistanceLocation,1) += (points(i,4)*points(j,4))/(points(i,4)+points(j,4));
                                            ++lastDistanceLocation;
                                        }
                                    }
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
                            double param[3] = {0};

                            fit(param, Vgexp, ndistances);

                            double range = -1/param[2];
                            double sill = -param[1];
                            double nugget = param[0]-sill;

                            //Fill V matrix
                            Eigen::MatrixXd V(num_points+1,num_points+1);

                            for(int c = 0; c < num_points; c++)
                            {
                                for(int d = 0; d < num_points; d++)
                                {
                                    if(c!=d)
                                    {
                                        V(c,d) = variogram(points, c, d, nugget, sill, range);
                                    }
                                    else if(c==d)
                                    {
                                        V(c,d) = variogram(points, c, d, nugget, sill, range) + (pi*(1-pi)-sigma)/points(c,4);
                                    }
                                }
                            }

                            for(int c = 0;c<num_points+1;++c)
                            {
                                V(num_points,c) = 1;
                                V(c,num_points) = 1;
                            }
                            V(num_points,num_points) = 0;

                            //Fill v
                            Eigen::VectorXd v(num_points+1);
                            for(int c = 0; c < num_points; c++)
                            {
                                v[c] = variogram(points, num_points, c, nugget, sill, range);
                            }
                            v[num_points] = 1;

                            Eigen::VectorXd out;

                            out = V.fullPivLu().solve(v);

                            double output = 0;
                            for(int c = 0; c < num_points; c++)
                            {
                                output += points(c,3) * out(c);
                            }
                            m_outDensityGrid->setValueAtIndex(ref,output);
                        }
                    }
                }
            }
        }
        setProgress(ref);
    }
}

double LVOX3_InterpolateKrigingBinomial::variogram(Eigen::MatrixXd points, int a, int b, double nugget, double sill, double range){
  double d = anisotropic_distance(points, a, b);
  double r = nugget + sill*(1 - exp(-(d/range)));

  return r;
}

double LVOX3_InterpolateKrigingBinomial::anisotropic_distance(Eigen::MatrixXd points, int a, int b){
  double dx = points(a,0)- points(b,0);
  double dy = points(a,1)- points(b,1);
  double dz = points(a,2)- points(b,2);

  return sqrt(dx*dx + dy*dy + dz*dz);
}
