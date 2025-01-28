#ifndef CT_READER_LARCHITECT_GRID_H
#define CT_READER_LARCHITECT_GRID_H

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_itemdrawable/ct_grid3d.h"

#include "ctlibio/ctlibio_global.h"

class CTLIBIO_EXPORT CT_Reader_LArchitect_Grid : public CT_AbstractReader
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_LArchitect_Grid(int subMenuLevel = 0);
    CT_Reader_LArchitect_Grid(const CT_Reader_LArchitect_Grid& other);

    QString displayableName() const override;

    bool setFilePath(const QString &filepath) override;

    READER_ALL_COPY_IMP(CT_Reader_LArchitect_Grid)

protected:

    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
    bool internalReadFile(CT_StandardItemGroup* group) override;

    bool readHeader(QTextStream &stream, Eigen::Vector3d &min, Eigen::Vector3d &max, Eigen::Vector3d &res, Eigen::Vector3i &dim, int &nMat, QMap<QString, int>  &matNames) const;
    CT_Grid3D<float>* createGrid(Eigen::Vector3d &min, Eigen::Vector3d &res, Eigen::Vector3i &dim) const;

private:
    CT_HandleOutSingularItem<CT_Grid3D<float> >          m_wood_surface;
    CT_HandleOutSingularItem<CT_Grid3D<float> >          m_leaf_surface;
    CT_HandleOutSingularItem<CT_Grid3D<float> >          m_wood_volume;
    CT_HandleOutSingularItem<CT_Grid3D<float> >          m_leaf_volume;
    CT_HandleOutSingularItem<CT_Grid3D<float> >          m_all_surface;
    CT_HandleOutSingularItem<CT_Grid3D<float> >          m_all_volume;


};

#endif // CT_READER_LARCHITECT_GRID_H
