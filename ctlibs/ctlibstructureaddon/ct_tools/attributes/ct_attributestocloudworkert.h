#ifndef CT_ATTRIBUTESTOCLOUDWORKERT_H
#define CT_ATTRIBUTESTOCLOUDWORKERT_H

#include "ct_tools/attributes/ct_abstractattributestocloudworker.h"

#include "ct_colorcloud/registered/ct_standardcolorcloudregistered.h"
#include "ct_normalcloud/registered/ct_standardnormalcloudregistered.h"

/**
 * Class that copy attributes to cloud passed in parameters.
 *
 *  TypeAttributes must be CT_AbstractPointsAttributes or CT_AbstractFaceAttributes, etc...
 */
template<typename TypeAttribute>
class CT_AttributesToCloudWorkerT : public CT_AbstractAttributesToCloudWorker
{
public:
    CT_AttributesToCloudWorkerT() : CT_AbstractAttributesToCloudWorker() {}

    void setAttributes(const QList<const TypeAttribute*> &attr)
    {
        m_attributes = attr;
    }

    const QList<const TypeAttribute*>& attributes() const
    {
        return m_attributes;
    }

    void setColorCloud(QSharedPointer<CT_StandardColorCloudRegistered> colors)
    {
        m_colors = colors;
    }

    QSharedPointer<CT_StandardColorCloudRegistered> colorCloud() const
    {
        return m_colors;
    }

    void setNormalCloud(QSharedPointer<CT_StandardNormalCloudRegistered> normals)
    {
        m_normals = normals;
    }

    QSharedPointer<CT_StandardNormalCloudRegistered> normalCloud() const
    {
        return m_normals;
    }

    void apply();

    void cancel()
    {
        m_cancel = true;
    }

private:
    int                                                 m_progress;
    bool                                                m_cancel;
    QList<const  TypeAttribute*>                        m_attributes;
    QSharedPointer<CT_StandardColorCloudRegistered>     m_colors;
    QSharedPointer<CT_StandardNormalCloudRegistered>    m_normals;

    void setProgress(int progress)
    {
        if(m_progress != progress)
        {
            m_progress = progress;
            emit progressChanged(m_progress);
        }
    }

};

#include "ct_tools/attributes/ct_attributestocloudworkert.hpp"

#endif // CT_ATTRIBUTESTOCLOUDWORKERT_H
