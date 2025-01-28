#include "lvox3_interpolatezaverage.h"

//Tools
#include "tools/lvox3_errorcode.h"

//Traversal algorithms
#include "tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.h"
#include "tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.h"

LVOX3_InterpolateZAverage::LVOX3_InterpolateZAverage(const lvox::Grid3Df *originalDensityGrid,
                                                     lvox::Grid3Df* const outDensityGrid,
                                                     const lvox::Grid3Di *hitGrid,
                                                     const lvox::Grid3Di *theoreticalGrid,
                                                     const lvox::Grid3Di *beforeGrid,
                                                     qint32 effectiveRayThreshold,
                                                     qint32 numZCalculatedOn)
{
    m_originalDensityGrid = (lvox::Grid3Df*)originalDensityGrid;
    m_outDensityGrid = (lvox::Grid3Df*)outDensityGrid;
    m_beforeGrid = (lvox::Grid3Di*)beforeGrid;
    m_theoriticalsGrid = (lvox::Grid3Di*)theoreticalGrid;
    m_hitGrid = (lvox::Grid3Di*)hitGrid;
    m_effectiveRayThreshold = effectiveRayThreshold;
    //m_isRelativeThreshold = false;
    m_numZCalculatedOn = numZCalculatedOn;
}
/*
LVOX3_InterpolateZAverage::LVOX3_InterpolateZAverage(const lvox::Grid3Df *originalDensityGrid,
                                                     lvox::Grid3Df* const outDensityGrid,
                                                     const lvox::Grid3Di *hitGrid,
                                                     const lvox::Grid3Di *theoreticalGrid,
                                                     const lvox::Grid3Di *beforeGrid,
                                                     qint32 effectiveRayThreshold,
                                                     qint32 isRelativeThreshold,
                                                     qint32 numZCalculatedOn)
{
    m_originalDensityGrid = (lvox::Grid3Df*)originalDensityGrid;
    m_outDensityGrid = (lvox::Grid3Df*)outDensityGrid;
    m_beforeGrid = (lvox::Grid3Di*)beforeGrid;
    m_theoriticalsGrid = (lvox::Grid3Di*)theoreticalGrid;
    m_hitGrid = (lvox::Grid3Di*)hitGrid;
    m_effectiveRayThreshold = effectiveRayThreshold;
    m__isRelativeThreshold = isRelativeThreshold;
    m_numZCalculatedOn = numZCalculatedOn;
}*/


void LVOX3_InterpolateZAverage::doTheJob()
{
    for (size_t zz = 0 ;  zz < m_originalDensityGrid->zdim() ; zz += m_numZCalculatedOn)//change increment for zHeight
    {

        float meanDensity = 0;
        size_t ncells = 0;
        // Compute mean value for the level z// Loop num of zHeight
        for (int zIncrement = 0; zIncrement < m_numZCalculatedOn; zIncrement ++)
        {
            for (size_t yy = 0 ;  yy < m_originalDensityGrid->ydim() ; yy++)
            {
                for (size_t xx = 0 ;  xx < m_originalDensityGrid->xdim() ; xx++)
                {
                    float value = m_originalDensityGrid->value(xx, yy, zz + zIncrement);
                    if (value >= 0) // empty cells don't count for the mean density
                    {
                        meanDensity += value;
                        ncells++;
                    }                    
                }
            }
        }
        //check ncells > 0
        meanDensity /= ncells;

        // set mean value for all NA cells
        for (int zIncrement = 0; zIncrement < m_numZCalculatedOn; zIncrement ++)
        {
            for (size_t yy = 0 ;  yy < m_originalDensityGrid->ydim() ; yy++)
            {
                for (size_t xx = 0 ;  xx < m_originalDensityGrid->xdim() ; xx++)
                {
                    float originalValue = m_originalDensityGrid->value(xx, yy, zz + zIncrement);
                    float interpolatedValue = originalValue;
                    if (originalValue < 0 && originalValue > -7.9 ) // replace NA values or incoherent density results
                    {
                        /*
                        //If even one ray was intercepted in the voxel
                        if(m_hitGrid->value(xx, yy, zz + zIncrement) >= 1){
                            int rays = m_theoriticalsGrid->value(xx,yy,zz + zIncrement) - m_beforeGrid->value(xx,yy,zz + zIncrement);
                            //If the number of rays is under the number of effective rays, should more or less always be true
                            if(rays <= m_effectiveRayThreshold){
                                m_outDensityGrid->setValue(xx, yy, zz + zIncrement, meanDensity);
                            }else { //Weird fringe case where Ni > Nt-Nb, error code -4 (Because the density value cant be negative and have more effectiveRayThresh than 10(default rayThresh)
                                m_outDensityGrid->setValue(xx, yy, zz + zIncrement, 0.0);
                            }
                        } else {
                            m_outDensityGrid->setValue(xx, yy, zz + zIncrement, 0.0);
                        }*/
                        //m_outDensityGrid->setValue(xx, yy, zz + zIncrement, meanDensity);
                        interpolatedValue = meanDensity;
                    }
                    m_outDensityGrid->setValue(xx, yy, zz + zIncrement, interpolatedValue);
                }
            }
        }
    }
}

