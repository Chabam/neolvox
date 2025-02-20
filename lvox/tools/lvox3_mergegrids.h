#ifndef LVOX3_MERGEGRIDS_H
#define LVOX3_MERGEGRIDS_H

#include <memory>
#include <QVector>
#include "tools/lvox3_utils.h"
#include "tools/lvox3_factory.hpp"

#define LVOX_REDUCER_RDI "MaxRDI"
#define LVOX_REDUCER_TRUST "MaxTrust"
#define LVOX_REDUCER_TRUST_RATIO "MaxTrustRatio"
#define LVOX_REDUCER_HITS "MaxNi"
#define LVOX_REDUCER_SUM_RATIO "SumRatio"
#define LVOX_REDUCER_WEIGHTED_RDI "WeightedRDI"
#define LVOX_REDUCER_WEIGHTED_RDI_ALT "WeightedRDIAlternative"

struct VoxelData {
    void load(LVOXGridSet *g, size_t idx);
    void commit(LVOXGridSet *g, size_t idx);
    int nt;
    int nb;
    int ni;
    float rd;
};

class VoxelReducerOptions {
public:
    VoxelReducerOptions() :
        reducerLabel(),
        ignoreVoxelZeroDensity(true),
        effectiveRaysThreshold(5) {}
    QString reducerLabel;
    bool ignoreVoxelZeroDensity;
    int effectiveRaysThreshold;
    bool isRelativeThreshold;
};

class VoxelReducer {
public:
    VoxelReducer();
    VoxelReducer(VoxelReducerOptions& opts);
    virtual ~VoxelReducer();
    virtual void init(VoxelData &data);
    virtual void join(const VoxelData &rhs) = 0;
    virtual VoxelData& value();
    VoxelData m_data;
    VoxelReducerOptions m_opts;
};

class VoxelReducerMaxRDI : public VoxelReducer {
public:
    void join(const VoxelData &rhs);
};

class VoxelReducerMaxTrust : public VoxelReducer {
public:
    void join(const VoxelData &rhs);
};

class VoxelReducerMaxTrustRatio : public VoxelReducer {
public:
    void join(const VoxelData &rhs);
};

class VoxelReducerMaxNi : public VoxelReducer {
public:
    void join(const VoxelData &rhs);
};

class VoxelReducerSumRatio : public VoxelReducer {
public:
    void join(const VoxelData &rhs);
    VoxelData& value();
};



//This class is a weighted RDI value of all the scans following the rule of:
//[ (Nt-Nb)1 × RDI1] + [ (Nt-Nb)2 × RDI2] +[ (Nt-Nb)3 × RDI3] +[ (Nt-Nb)4 × RDI4]/[(Nt-Nb)1 +(Nt-Nb)2 +(Nt-Nb)3 +(Nt-Nb)4]
//The numbers 1,2,3,4 are only to represent every scanner
class VoxelReducerWeightedRDI : public VoxelReducer {
public:
    VoxelReducerWeightedRDI();
    VoxelReducerWeightedRDI(VoxelReducerOptions& opts);
    void init(VoxelData &data);
    void join(const VoxelData &rhs);
    VoxelData& value();
protected:
    bool isNotOcclusion(const VoxelData &v);
    //Intermediary rdi value, the actual value is calculated in the value() method
    double sumRDI;
     //Intermediary weights
    int sumWeight;
};

class VoxelReducerWeightedRDIAlt : public VoxelReducerWeightedRDI {
    using VoxelReducerWeightedRDI::VoxelReducerWeightedRDI;
    //using VoxelReducerWeightedRDI::init;
    using VoxelReducerWeightedRDI::value;
    void init(VoxelData &data);
    void join(const VoxelData &rhs);
};

struct VoxelReducerDefinitionStruct {
    QString label;
    QString name;
    QString desc;
};

#include <functional>
typedef std::function<bool (const size_t &)> ProgressMonitor;

// TODO: LVOX3_MergeGrids should be converted to singleton

class LVOX3_MergeGrids
{
public:
    LVOX3_MergeGrids();

    static void apply(LVOXGridSet *merged, QVector<LVOXGridSet*> *gs,
                      VoxelReducer *reducer);

    static void apply(LVOXGridSet *merged, QVector<LVOXGridSet*> *gs,
                      VoxelReducer *reducer, ProgressMonitor monitor);

    static const QVector<VoxelReducerDefinitionStruct> getReducerList();

    std::unique_ptr<VoxelReducer> makeReducer(QString &label);

private:
    Factory<VoxelReducer> f;
    static const QVector<VoxelReducerDefinitionStruct> m_voxelReducerDefinitions;
};

#endif // LVOX3_MERGEGRIDS_H
