#ifndef LVOX3_STEPCOMPUTEPAD_H
#define LVOX3_STEPCOMPUTEPAD_H

#include "ct_step/abstract/ct_abstractstep.h"
#include "tools/lvox3_gridtype.h"

#include <QStack>


/*!
 * \class LVOX_StepComputePAD
 * \ingroup Steps_LVOX
 * \brief <b>Compute LAD grid from ni, nt, nb and deltaT grids.</b>
 *
 */

class LVOX3_StepComputePAD: public CT_AbstractStep
{
    Q_OBJECT

public:

    LVOX3_StepComputePAD();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    void savePostSettings(SettingsWriterInterface& writer) const final;

    bool restorePostSettings(SettingsReaderInterface &reader) final;

    static float computePAD(float density, float xres,float yres, float zres, float D_Nt_mean, float gFunction);

    QString parametersDescription(bool defaultValue) final;
    QString outputDescription() const final;


    QStringList getStepRISCitations() const final;

protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

    bool postInputConfigure() final;

    /*!
     * \brief computeVegetation
     *
     * Estimate the mean element cross section assuming simple element shape and size
     */
    void computeVegetation();

    /*!
     * \brief computeAttenuation
     *
     * Compute the attenuation coefficient of a single vegetation element;
     */
    void computeAttenuation(double volume);

    /*!
     * \brief computeStatistique
     * \param i index for the coputation of the statistics
     *
     * Not yep used
     */
    void computeStatistique(int i);

    /*!
     * \brief computeVariance
     *
     * Not yet used
     */
    void computeVariance();

    /*!
     * \brief computeIntervals
     *
     * Not yet used
     */
    void computeIntervals();

    /*!
     * \brief computeBias
     *
     * Not yep used
     */
    void computeBias();
    bool isOccluded(int enteredShots, int thereticalShots);

private :
    struct Method {
        QString shortName;
        QString longName;
        QString description;
        bool isSelected;
    };
    QMap<QString, Method> _methodInfos;
    QString _confidenceLevel;
    double _lambda1;// attenuation coefficient of a single vegetation element
    bool _BCMLE;
    bool _MLE;
    bool _BCBL; // equal path
    bool _BL;
    bool _CF;
    bool _BC2BL;// unequal path
    bool    _EraseInfinity;
    double   _PADlimit;
    int _NThreshold;
    bool _isRelativeThreshold;
    bool _grilleSup;

    CT_HandleInResultGroupCopy<>                    _inResult;
    CT_HandleInStdZeroOrMoreGroup                   _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                           _inGroup;
    CT_HandleInSingularItem<lvox::Grid3Di >     _inDeltaHits;
    CT_HandleInSingularItem<lvox::Grid3Di >     _inDeltaTheo;
    CT_HandleInSingularItem<lvox::Grid3Di >     _inDeltaBefore;

    CT_HandleOutSingularItem<lvox::Grid3Df> _out_PADCF;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_PADMLE;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_PADBCMLE;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_PADBL;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_PADBCBL;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_PADBC2BL;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_FreeSum;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_FreeEffSum;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_DeltaSum;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_DeltaEffSum;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_DeltaEffSquareSum;
    CT_HandleOutSingularItem<lvox::Grid3Df> _out_1ZleDeltaEffSum;

    QStack<size_t> _InfinityIndexStack;
    double _MeanElementCrossSection;        
    double _estimatedAttenuationCoefficient;
    double _estimatedVariance;

};

#endif // LVOX3_STEPCOMPUTEPAD_H
