#ifndef CT_OPFMESHMODEL_H
#define CT_OPFMESHMODEL_H

#include "ct_itemdrawable/ct_meshmodel.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_OPFMeshModel : public CT_MeshModel
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_OPFMeshModel, CT_MeshModel, OPF mesh model)
    using SuperClass = CT_MeshModel;

public:
    CT_OPFMeshModel();
    CT_OPFMeshModel(CT_Mesh* mesh);
    CT_OPFMeshModel(const CT_OPFMeshModel& other) = default;

    void setDUp(double dUp);
    void setDDown(double dDown);

    double dUp() const;
    double dDown() const;

    CT_ITEM_COPY_IMP(CT_OPFMeshModel)

private:
    double m_dUp;
    double m_dDown;
};

#endif // CT_OPFMESHMODEL_H
