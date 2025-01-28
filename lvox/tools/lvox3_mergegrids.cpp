#include "lvox3_mergegrids.h"

//Tools
#include <QVector>

const QVector<VoxelReducerDefinitionStruct>
//All the definitions are in the .h file and should be defined there for consistency.
LVOX3_MergeGrids::m_voxelReducerDefinitions = {
    { LVOX_REDUCER_RDI,         "Max density",      "max(rdi)"              },
    { LVOX_REDUCER_TRUST,       "Max trust",        "max(nt - nb)"          },
    { LVOX_REDUCER_TRUST_RATIO, "Max trust ratio",  "max((nt - nb)/nt)"     },
    { LVOX_REDUCER_HITS,        "Max hits",         "max(ni)"               },
    { LVOX_REDUCER_SUM_RATIO,   "Sum ratio",        "sum(ni)/sum(nt - nb)"  },
    { LVOX_REDUCER_WEIGHTED_RDI,"Weighted rdi",     "sum((nt-nb)*rdi)/sum(nt-nb)"  },
    { LVOX_REDUCER_WEIGHTED_RDI_ALT,"Weighted rdi alternative",     "sum((nt-nb)/nt*rdi)/sum((nt-nb)/nt)"  }
};

//Registers all the voxel reducers that you can use (if it's not registered the merging process will fail)
LVOX3_MergeGrids::LVOX3_MergeGrids()
{
    f.registerType<VoxelReducerMaxRDI>(LVOX_REDUCER_RDI);
    f.registerType<VoxelReducerMaxTrust>(LVOX_REDUCER_TRUST);
    f.registerType<VoxelReducerMaxTrustRatio>(LVOX_REDUCER_TRUST_RATIO);
    f.registerType<VoxelReducerMaxNi>(LVOX_REDUCER_HITS);
    f.registerType<VoxelReducerSumRatio>(LVOX_REDUCER_SUM_RATIO);
    //f.registerType<VoxelReducerSumRatio>(LVOX_REDUCER_WEIGHTED_RDI);
    f.registerType<VoxelReducerWeightedRDI>(LVOX_REDUCER_WEIGHTED_RDI);
    f.registerType<VoxelReducerWeightedRDIAlt>(LVOX_REDUCER_WEIGHTED_RDI_ALT);
}

void LVOX3_MergeGrids::apply(LVOXGridSet *merged,
                             QVector<LVOXGridSet*> *gs,
                             VoxelReducer *reducer)
{
    apply(merged, gs, reducer,
        [](const size_t& i) -> bool {
            Q_UNUSED(i);
            return false;
        });
}

void LVOX3_MergeGrids::apply(LVOXGridSet *merged,
                             QVector<LVOXGridSet*> *gs,
                             VoxelReducer *reducer,
                             ProgressMonitor monitor)
{
    if (gs->empty())
        return;

    LVOXGridSet *a = gs->first();
    size_t n = a->rd->nCells();
    VoxelData item, rhs;
    for (size_t idx = 0; idx < n; idx++) {
        // default values
        item.load(a, idx);
        reducer->init(item);
        //Loops through all the grids for each voxel
        for (int gi = 1; gi < gs->size(); gi++) {            
            LVOXGridSet *set = gs->at(gi);
            rhs.load(set, idx);
            reducer->join(rhs);
        }
        //Assigns the value of the voxel for the merged density grid
        reducer->value().commit(merged, idx);
        if ((idx % 100) == 0) { // rate limit the update
            if (monitor(idx)) {
                break;
            }
        }
    }
}

const QVector<VoxelReducerDefinitionStruct> LVOX3_MergeGrids::getReducerList()
{
    return m_voxelReducerDefinitions;
}
//Calls the reducer factory to determine which voxel reducer will be used
std::unique_ptr<VoxelReducer> LVOX3_MergeGrids::makeReducer(QString &label)
{    
    return std::unique_ptr<VoxelReducer>(f.create(label));
}

void VoxelData::load(LVOXGridSet *g, size_t idx) {
    nt = g->nt->valueAtIndex(idx);
    nb = g->nb->valueAtIndex(idx);
    ni = g->ni->valueAtIndex(idx);
    rd = g->rd->valueAtIndex(idx);
}

void VoxelData::commit(LVOXGridSet *g, size_t idx) {
    g->nt->setValueAtIndex(idx, nt);
    g->nb->setValueAtIndex(idx, nb);
    g->ni->setValueAtIndex(idx, ni);
    g->rd->setValueAtIndex(idx, rd);    
}

VoxelReducer::VoxelReducer() {}

VoxelReducer::VoxelReducer(VoxelReducerOptions &opts) :
    m_opts(opts) {}

VoxelReducer::~VoxelReducer() {}

void VoxelReducer::init(VoxelData &data) {
    m_data = data;
}


VoxelData &VoxelReducer::value() { return m_data; }
//Voxelreducer that keeps grid set with max RDI
void VoxelReducerMaxRDI::join(const VoxelData &rhs) {
    if (rhs.rd > m_data.rd) {
        m_data = rhs;
    }
}

void VoxelReducerMaxTrust::join(const VoxelData &rhs) {
    if ((rhs.nt - rhs.nb) > (m_data.nt - m_data.nb)) {
        m_data = rhs;
    }
}

//Voxelreducer that keeps grid set with max ratio RDI
void VoxelReducerMaxTrustRatio::join(const VoxelData &rhs) {
    if (m_opts.ignoreVoxelZeroDensity && rhs.rd <= 0) {
        return;
    }
    float ratioSelf = 0;
    float ratioOther = 0;
    if (rhs.nt > 0) {
        ratioOther = (float)(rhs.nt - rhs.nb) / rhs.nt;
    }
    if (m_data.nt > 0) {
        ratioSelf = (float)(m_data.nt - m_data.nb) / m_data.nt;
    }
    if (ratioOther > ratioSelf) {
        m_data = rhs;
    }
}

void VoxelReducerMaxNi::join(const VoxelData &rhs) {
    if (rhs.ni > m_data.ni) {
        m_data = rhs;
    }
}

//Voxelreducer that sum of voxels with higher than raythreshhold, normally 10
void VoxelReducerSumRatio::join(const VoxelData &rhs) {
    if ((m_data.nt - m_data.nb) > m_opts.effectiveRaysThreshold) {
        m_data.nt += rhs.nt;
        m_data.ni += rhs.ni;
        m_data.nb += rhs.nb;
    }else{
        m_data.rd = rhs.rd;
    }
}

VoxelData &VoxelReducerSumRatio::value() {
    if ((m_data.nt - m_data.nb) > m_opts.effectiveRaysThreshold) {
        m_data.rd = (float)m_data.ni / (m_data.nt - m_data.nb);
    }
    return m_data;
}


VoxelReducerWeightedRDI::VoxelReducerWeightedRDI() : VoxelReducer(), sumRDI(0), sumWeight(0.0){}

VoxelReducerWeightedRDI::VoxelReducerWeightedRDI(VoxelReducerOptions &opts) : VoxelReducer(opts), sumRDI(0), sumWeight(0.0){}

void VoxelReducerWeightedRDI::init(VoxelData &data) {
    VoxelReducer::init(data);
    sumRDI = 0.0;
    sumWeight = 0;

    if(isNotOcclusion(m_data)) {
        sumRDI += (m_data.nt - m_data.nb)*m_data.rd;
        sumWeight += (m_data.nt - m_data.nb);
    }
}

//Voxelreducer that weighted sum of voxels with higher than raythreshhold, normally 10
void VoxelReducerWeightedRDI::join(const VoxelData &rhs) {
    m_data.nt += rhs.nt;
    m_data.ni += rhs.ni;
    m_data.nb += rhs.nb;

    if (isNotOcclusion(rhs) && isNotOcclusion(m_data) ){
        sumRDI += ((rhs.nt-rhs.nb)*rhs.rd);
        sumWeight += (rhs.nt-rhs.nb);
    }else if (isNotOcclusion(rhs) ){ //if
        m_data.rd = rhs.rd;
        sumRDI += ((rhs.nt-rhs.nb)*rhs.rd);
        sumWeight += (rhs.nt-rhs.nb);
    }
}

VoxelData &VoxelReducerWeightedRDI::value() {
    //Only modify of voxel value if we did sommations    
    if (isNotOcclusion(m_data)  && sumWeight > 0) {
        m_data.rd = sumRDI / sumWeight;
    }
    return m_data;
}

bool VoxelReducerWeightedRDI::isNotOcclusion(const VoxelData &v) {
    //Only modify of voxel value if we did sommations
    /*
    double passRate = static_cast<double>(nt - nb)/nt;

    if (passRate > m_opts.effectiveRaysThreshold/100.0) {
        return true;
    }*/

    // if the computation of PAD is correct, there is no need to check rd >= 0!!!
    if(m_opts.isRelativeThreshold){
        return ((v.nt - v.nb)*100.0/v.nt > m_opts.effectiveRaysThreshold) && (v.rd >= 0);
    }
    else {
        return (v.nt - v.nb > m_opts.effectiveRaysThreshold) && (v.rd >= 0);
    }
}



void VoxelReducerWeightedRDIAlt::init(VoxelData &data) {
    VoxelReducer::init(data);
    sumRDI = 0.0;
    sumWeight = 0;    
    if(isNotOcclusion(m_data)) {
        double weight = (m_data.nt - m_data.nb)/m_data.nt;
        sumRDI += weight*m_data.rd;
        sumWeight += weight;
    }
}

//Voxelreducer that weighted sum of voxels with higher than raythreshhold, normally 10
void VoxelReducerWeightedRDIAlt::join(const VoxelData &rhs) {
    m_data.nt += rhs.nt;
    m_data.ni += rhs.ni;
    m_data.nb += rhs.nb;

    if (isNotOcclusion(rhs) && isNotOcclusion(m_data) ){
        double weight = (rhs.nt - rhs.nb)/rhs.nt;
        sumRDI += weight*rhs.rd; //Intermediary rdi value, the actual value is calculated in the value() method
        sumWeight += weight; //Private variable to VoxelReducerWeightedRDI for storage purposes to the value() method
    }else if (isNotOcclusion(rhs) ){ //if
        m_data.rd = rhs.rd;
    }

    //do nothing if the value of rhs is not good
    if(rhs.nt - rhs.nb > m_opts.effectiveRaysThreshold/100.0 && rhs.rd > 0) {
        //do a summation if m_data does not contain error value
        /**only do the sum if both voxels has a significal rays and the density value (or PAD) is greater than 0
         * some voxels may have a PAD = 0 or -1 which can diminue the value of merged voxel
         **/
        if(m_data.nt - m_data.nb > m_opts.effectiveRaysThreshold && m_data.rd > 0) {
            //Merging Ni,Nt,Nb
            //Calculation of RDI
            double weight = (rhs.nt - rhs.nb)/rhs.nt;
            sumRDI += weight*rhs.rd; //Intermediary rdi value, the actual value is calculated in the value() method
            sumWeight += weight; //Private variable to VoxelReducerWeightedRDI for storage purposes to the value() method
        } else{
            // replace value of curent voxel by that of rhs
            m_data.rd = rhs.rd;
            double weight = (rhs.nt - rhs.nb)/rhs.nt;
            sumRDI += weight*m_data.rd;
            sumWeight += weight;
        }
    }
}
