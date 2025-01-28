#ifndef IGLOBALCLOUDLISTENER_H
#define IGLOBALCLOUDLISTENER_H

#include <QtGlobal>

class IGlobalCloudListener
{
public:
    virtual ~IGlobalCloudListener() {}

protected:

    friend class CT_AbstractGlobalCloudManager;

    /**
     * @brief Called before the global cloud will be imputed of elements between beginIndex and endIndex (endIndex = beginIndex+size).
     */
    virtual void cloudWillBeDeleted(const size_t &beginIndex, const size_t &size) { Q_UNUSED(beginIndex) Q_UNUSED(size) }

    /**
     * @brief Called after the global cloud it's indexes was shifted.
     */
    virtual void cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size) { Q_UNUSED(beginIndex) Q_UNUSED(size) }

    /**
     * @brief Called after the global cloud was imputed of elements between beginIndex and endIndex (endIndex = beginIndex+size).
     */
    virtual void cloudDeleted(const size_t &beginIndex, const size_t &size) { Q_UNUSED(beginIndex) Q_UNUSED(size) }

    /**
     * @brief Called when the size of the global cloud has increased
     */
    virtual void cloudAdded(const size_t &size) { Q_UNUSED(size) }
};

#endif // IGLOBALCLOUDLISTENER_H
