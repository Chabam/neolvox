#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanagert.h"

template<typename T>
CT_AbstractGlobalCloudManagerT<T>::CT_AbstractGlobalCloudManagerT() : CT_AbstractGlobalCloudManager()
{
}

// protected //

template<typename T>
void CT_AbstractGlobalCloudManagerT<T>::shiftAllCloudIndexFrom(const int &from, const size_t &offset, const bool &negativeOffset)
{
    int aSize = m_cirArray.size();

    for(int i=from; i<aSize; ++i)
        m_cirArray.at(i)->abstractCloudIndexT()->internalShiftAll(offset, negativeOffset);
}

template<typename T>
template<typename CI>
std::vector< ct_index_type >* CT_AbstractGlobalCloudManagerT<T>::getInternalDataOfIndex(CI *index) const
{
    return index->internalData();
}

template<typename T>
template<typename CIR, typename CI>
CIR* CT_AbstractGlobalCloudManagerT<T>::createNewIndexRegistered(CI *index) const
{
    return new CIR(index);
}

template<typename T>
void CT_AbstractGlobalCloudManagerT<T>::setEnableSyncForCIR(CT_AbstractCloudIndexRegisteredT<T> *cir, bool enable)
{
    if(enable)
    {
        m_cirArrayUnsync.removeOne(cir);
        return;
    }

    if(!enable && !m_cirArrayUnsync.contains(cir))
        m_cirArrayUnsync.append(cir);
}

template<typename T>
bool CT_AbstractGlobalCloudManagerT<T>::nextIndexToIgnoreToSync(const size_t &currentIndex, size_t &beginIndex, size_t &endIndex)
{
    if(m_cirArrayUnsync.isEmpty())
        return false;

    QListIterator< CT_AbstractCloudIndexRegisteredT<T>* > it(m_cirArrayUnsync);

    if(it.hasNext())
    {
        CT_AbstractCloudIndexRegisteredT<T> *cir = it.next();

        beginIndex = cir->first();
        endIndex = cir->last();

        bool stop = false;

        while(!stop
              && it.hasNext())
        {
            cir = it.next();

            if(currentIndex > endIndex)
            {
                beginIndex = cir->first();
                endIndex = cir->last();
            }
            else
            {
                stop = true;
            }
        }

        if(currentIndex > beginIndex)
            beginIndex = currentIndex;

        return (currentIndex <= endIndex);
    }

    return false;
}
