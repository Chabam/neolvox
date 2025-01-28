#ifndef PLYCOMPUTREEWRAPPERS_H
#define PLYCOMPUTREEWRAPPERS_H

///////////////////////////////////////////////
/// This file will contains some wrappers
/// that can must be use by computree to read
/// a plys file and store readed values in
/// computree arrays.
///////////////////////////////////////////////

#include <QVBoxLayout>

#include "ct_point.h"
#include "ct_accessor/ct_pointaccessor.h"
#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"
#include "ct_view/ct_abstractconfigurablewidget.h"

#include "readers/tools/ply/views/gplyreadconfiguration.h"

/**
 * @brief This is a wrapper to convert double values of a readed vertex
 *        in a ply file to a CT_PointData
 */
struct Ply_CT_Point_Wrapper {
    Ply_CT_Point_Wrapper()
    {
        globalIndex = 0;
        updateIt = true;
        accessor = nullptr;
    }

    /**
     * @brief Called by Ply_CT_PointCloud_Wrapper to set the accessor to use
     */
    void setAccessor(const CT_PointAccessor* acc)
    {
        accessor = const_cast<CT_PointAccessor*>(acc);
    }

    /**
     * @brief Called by Ply_CT_PointCloud_Wrapper so this wrapper know which point to modify
     * @param index : the global index of the point to modify in the global point cloud
     */
    void setCurrentGlobalIndex(const size_t& index)
    {
        if(globalIndex != index) {
            updateIt = true;
            globalIndex = index;
        }
    }

    /**
     * @brief Called by a ply reader when he want to modify the point
     * @param other : point that contains readed values
     */
    Ply_CT_Point_Wrapper& operator=(const Ply_CT_Point_Wrapper& other)
    {
        if(updateIt) {
            accessor->replacePointAt(globalIndex, other.p);
            updateIt = false;
        }

        return *this;
    }

    /**
     * @brief Called by a ply reader when he want to modify a specific
     *        coordinate of the point
     * @param index : index of the coordinate to modify
     */
    double& operator[](const size_t& index)
    {
        return p[qlonglong(index)];
    }

    bool                updateIt;
    size_t              globalIndex;
    CT_PointAccessor*   accessor;
    CT_Point            p;
};

/**
 * @brief This is a class that create a cloud of points and register it
 *        to the repository of computree. Also it will set the current
 *        global index of the point to modify.
 */
struct Ply_CT_PointCloud_Wrapper {
    typedef Ply_CT_Point_Wrapper value_type;

    using CreatePointCloudCallbackFunc = std::function<void(CT_NMPCIR)>;

    Ply_CT_PointCloud_Wrapper(CreatePointCloudCallbackFunc f) :
        mCallback(f)
    {
        pcir = CT_NMPCIR(nullptr);
        beginIndex = 0;
        point.setAccessor(&accessor);
    }

    /**
     * @brief Called by a ply reader when we must create the point cloud
     */
    void resize(const size_t& newSize)
    {
        if((newSize > 0)
                && pcir.isNull()) {
            pcir = PS_REPOSITORY->createNewPointCloud(newSize);
            beginIndex = pcir->abstractCloudIndex()->first();

            if(mCallback != nullptr)
                mCallback(pcir);
        }
    }

    /**
     * @brief Called by a ply reader when he want to modify a point
     * @param index : local index of the point to modify in the "pcir" array
     * @return Returns a wrapper that can modify the computree point in the global point cloud
     */
    Ply_CT_Point_Wrapper& operator[](const size_t& index)
    {
        // set the current global index to the point so it can know which point to modify
        point.setCurrentGlobalIndex(beginIndex + index);

        return point;
    }

    CT_NMPCIR               pcir;
    size_t                  beginIndex;
    CT_PointAccessor        accessor;
    Ply_CT_Point_Wrapper    point;
    CreatePointCloudCallbackFunc    mCallback;
};

template<typename SetterPtr, typename ValueInCloud>
struct Ply_CT_GenericValueInCloud_Wrapper
{
    Ply_CT_GenericValueInCloud_Wrapper() :
        mSetter(nullptr),
        mUpdateIt(true),
        mLocalIndex(0)
    {
    }

    /**
     * @brief Called by Ply_CT_PointCloud_Wrapper so this wrapper know which value to modify
     * @param index : the local index of the value to modify in the cloud
     */
    void setCurrentLocalIndex(const size_t& index)
    {
        if(mLocalIndex != index) {
            mUpdateIt = true;
            mLocalIndex = index;
        }
    }

    /**
     * @brief Called by a ply reader when he want to modify a specific
     *        member of the value
     * @param index : index of the member to modify
     */
    auto& operator[](const size_t& index)
    {
        return mValue[index];
    }

    /**
     * @brief Called by a ply reader when he want to modify the value
     * @param newValue : the new value to set
     */
    Ply_CT_GenericValueInCloud_Wrapper& operator=(const double& newValue)
    {
        mValue = newValue;

        if(mUpdateIt) {
            mSetter->setValueWithLocalIndex(mLocalIndex, mValue);
            mUpdateIt = false;
        }

        return *this;
    }

    /**
     * @brief Called by a ply reader when he want to modify the value
     * @param other : wrapper that contains readed values
     */
    Ply_CT_GenericValueInCloud_Wrapper& operator=(const Ply_CT_GenericValueInCloud_Wrapper& other)
    {
        if(mUpdateIt) {
            mSetter->setValueWithLocalIndex(mLocalIndex, other.mValue);
            mUpdateIt = false;
        }

        return *this;
    }

    SetterPtr       mSetter;
    ValueInCloud    mValue;
    bool            mUpdateIt;
    size_t          mLocalIndex;
};

/**
 * @brief This is a class that use the attributes manager of ValueInCloud to set the value
 *        for the point at specified index.
 */
template<typename Manager, typename ValueInCloud>
struct Ply_CT_GenericCloud_Wrapper {
    typedef Ply_CT_GenericValueInCloud_Wrapper<typename Manager::SetterPtr, ValueInCloud> value_type;

    Ply_CT_GenericCloud_Wrapper(Manager& manager) :
        mManager(manager)
    {
    }

    /**
     * @brief Called the first time the pcir is created
     */
    void setPCIR(CT_NMPCIR pcir)
    {
        mValueWrapper.mSetter = mManager.createAttributesSetterPtr(pcir);
    }

    /**
     * @brief Called by a ply reader when we must create the attribute cloud
     */
    void resize(const size_t& /*newSize*/)
    {
    }

    /**
     * @brief Called by a ply reader when he want to modify the attribute
     * @param index : local index of the point
     * @return Returns a wrapper that can modify the computree point in the global point cloud
     */
    inline value_type& operator[](const size_t& index)
    {
        mValueWrapper.setCurrentLocalIndex(index);
        return mValueWrapper;
    }

    Manager&    mManager;
    value_type  mValueWrapper;
};

/**
 * @brief Wrapper to use the configuration widget of a ply file with computree
 */
class GPly_CT_ReadConfiguration_Wrapper : public CT_AbstractConfigurableWidget {
    Q_OBJECT
public:
    GPly_CT_ReadConfiguration_Wrapper(QWidget *parent = nullptr) : CT_AbstractConfigurableWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);

        plyConfig = new GPlyReadConfiguration();
        layout->addWidget(plyConfig);

        setWindowTitle(plyConfig->windowTitle());
    }

    GPlyReadConfiguration* plyConfigurationWidget() const {
        return plyConfig;
    }

    virtual bool canAcceptIt(QString *err) const {
        if(plyConfig->isValid())
            return true;

        if(err != nullptr)
            *err = tr("Certains éléments ne sont pas complètement renseignés.");

        return false;
    }

    virtual bool isSettingsModified() const {
        return true;
    }

public slots:
    virtual bool updateElement(QString *err)  {
        return canAcceptIt(err);
    }

private:
    GPlyReadConfiguration* plyConfig;
};

#endif // PLYCOMPUTREEWRAPPERS_H
