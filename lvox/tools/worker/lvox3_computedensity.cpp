#include "lvox3_computedensity.h"

//Tools
#include "tools/lvox3_errorcode.h"

LVOX3_ComputeDensity::LVOX3_ComputeDensity(lvox::Grid3Df* density,
                                           const lvox::Grid3Di* hits,
                                           const lvox::Grid3Di* theoritical,
                                           const lvox::Grid3Di* before,
                                           qint32 effectiveRayThreshold)
{
    m_density = density;
    m_hits = (lvox::Grid3Di*)hits;
    m_theoritical = (lvox::Grid3Di*)theoritical;
    m_before = (lvox::Grid3Di*)before;
    m_effectiveRayThreshold = effectiveRayThreshold;
}

void LVOX3_ComputeDensity::doTheJob()
{
    size_t nbVoxels = m_density->nCells();

    qint32 NtMinusNb;
    float Ni;

    setProgressRange(0, nbVoxels);

    // For each voxel
    for ( size_t i = 0 ; i < nbVoxels && !mustCancel(); ++i )
    {
        const qint32 Nb = m_before->valueAtIndex(i);
        const qint32 Nt = m_theoritical->valueAtIndex(i);

        if (Nt == Nb)
            m_density->setValueAtIndex(i, lvox::Nt_Equals_Nb);
        else if (Nt < Nb)
            m_density->setValueAtIndex(i, lvox::Nt_Inferior_Nb);
        else if ((NtMinusNb = (Nt - Nb)) < m_effectiveRayThreshold)
            m_density->setValueAtIndex(i, lvox::Nt_Minus_Nb_Inferior_Threshold);
        else if ((Ni = m_hits->valueAtIndex(i)) > NtMinusNb) // TODO : check if must be > or >=
            m_density->setValueAtIndex(i, lvox::Ni_Superior_Nt_Minus_Nb);
        else
            m_density->setValueAtIndex(i, Ni / NtMinusNb ); //NtMinusNb is initialized in the second else if

        setProgress(i);
    }

    m_density->computeMinMax();
}
