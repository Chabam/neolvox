#ifndef CT_READER_OPF_H
#define CT_READER_OPF_H

#include "ct_reader/abstract/ct_abstractreader.h"

#include "ct_model/outModel/ct_outopfnodegroupmodel.h"

#include "ct_itemdrawable/ct_ttreegroup.h"
#include "ct_itemdrawable/ct_topfnodegroup.h"
#include "ct_itemdrawable/ct_opfmeshmodel.h"
#include "ct_itemdrawable/ct_itemattributelist.h"

#include "ct_mesh/ct_mesh.h"
#include "ct_point.h"

#include "rapidxml/rapidxml_utils.hpp"

#include "ctlibio/ctlibio_global.h"

class CT_OPF_Attribute
{
public:
    CT_OPF_Attribute() {}

    CT_OPF_Attribute(const QString &name, const QString &type, const int& indexCreation)
    {
        m_name = name;
        m_type = type;
        m_indexCreation = indexCreation;
    }

    QString                 m_name;
    QString                 m_type;
    int                     m_indexCreation;

    bool operator ==(const CT_OPF_Attribute &o)
    {
        return (o.m_name == m_name) && (o.m_type == m_type);
    }
};

class CT_OPF_Type
{
public:
    CT_OPF_Type() {}

    CT_OPF_Type(const QString &name, const quint8 &level, const size_t &id, const int& indexCreation)
    {
        m_name = name;
        m_level = level;
        m_id = id;
        m_indexCreation = indexCreation;
    }

    QString                             m_name;
    quint8                              m_level;
    size_t                              m_id;
    int                                 m_indexCreation;
    QHash<QString, CT_OPF_Attribute>    m_attributes;
};

class CT_OPF_Mesh
{
public:
    CT_Mesh                 *m_mesh;

    CT_OPF_Mesh() { m_mesh = nullptr; m_bboxOK = false; }
    ~CT_OPF_Mesh() { delete m_mesh; }

    void getBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const;

private:
    mutable Eigen::Vector3d   m_min;
    mutable Eigen::Vector3d   m_max;
    mutable bool              m_bboxOK;
};

class QXmlStreamReader;

/**
 * @brief Read OPF File (http://amapstudio.cirad.fr/)
 */
class CTLIBIO_EXPORT CT_Reader_OPF : public CT_AbstractReader
{
    Q_OBJECT
    using SuperClass = CT_AbstractReader;

public:
    CT_Reader_OPF(int subMenuLevel = 0);
    CT_Reader_OPF(const CT_Reader_OPF &other);
    ~CT_Reader_OPF() override;

    QString displayableName() const override;

    bool configure() override;

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    CT_AbstractReader* copy() const;
    READER_ALL_COPY_IMP(CT_Reader_OPF)

    static CT_AbstractItemAttribute* staticCreateAttributeForType(const QString &type, const QString &value = "");

private:
    using TopologyHandleType = CT_HandleOutGroup<CT_TTreeGroup, CT_OutOPFNodeGroupModel>;
    using NodeGroupHandleType = CT_HandleOutGroup<CT_TOPFNodeGroup, CT_OutOPFNodeGroupModel>;
    using MeshHandleType = CT_HandleOutSingularItem<CT_OPFMeshModel>;
    using AttributeListHandleType = CT_HandleOutSingularItem<CT_ItemAttributeList>;
    using AttributeIntHandleType = CT_HandleOutStdIntItemAttribute;
    using AttributeDoubleHandleType = CT_HandleOutStdDoubleItemAttribute;
    using AttributeBoolHandleType = CT_HandleOutStdBoolItemAttribute;
    using AttributeStringHandleType = CT_HandleOutStdStringItemAttribute;
    using AbstractAttributeHandle = CT_AbstractHandleOutItemAttribute;

    TopologyHandleType                                  m_topologyHandle;

    bool                                                m_loadMeshes;
    QHash<QString, CT_OPF_Type>                         m_types;
    QHash<QString, CT_OPF_Attribute>                    m_attributes;
    QHash<int, CT_OPF_Mesh*>                            m_meshes;
    QHash<int, CT_OPF_Mesh*>                            m_shapes;

    int                                                 m_totalNode;

    QHash<QString, CT_OPF_Type>                         m_typesNew;
    QHash<QString, CT_OPF_Attribute>                    m_attributesNew;
    int                                                 m_totalNodeNew;

    CT_OPF_Mesh                                         m_cylinderMesh;

    static const QVector<QString>                       TOPOLOGY_NAMES;

    void finalizeSettings();

    template<typename HandleType>
    void createAndAddItemAttributeHandle(CT_ReaderOutModelStructureManager& manager, AttributeListHandleType& parentHandle, const CT_OPF_Type &type, const CT_OPF_Attribute &attribute) {
        HandleType* attHandle = new HandleType();
        manager.addItemAttribute(parentHandle, *attHandle, CT_AbstractCategory::DATA_VALUE, attribute.m_name);
        registerHandlePtr(type.m_name + "_" + attribute.m_name, attHandle);
    }

    void clearMeshes();
    void clearShapes();

    void readMesh(rapidxml::xml_node<> *xmlNode);
    void readShape(rapidxml::xml_node<> *xmlNode);
    void readGeometry(rapidxml::xml_node<> *xmlNode, CT_TOPFNodeGroup *node, const QString &typeName);
    void transformAndCreateMesh(CT_Mesh *mesh, Eigen::Vector3d &min, Eigen::Vector3d &max, CT_TOPFNodeGroup *node, const double &dUp, const double &dDwn, const QString &typeName);

    void recursiveReadTopologyForModel(rapidxml::xml_node<> *node,
                                       int &totalNode,
                                       QHash<QString,
                                       CT_OPF_Type> &types,
                                       const QHash<QString,
                                       CT_OPF_Attribute> &attributes,
                                       int& nTypesCreated);

    void recursiveReadTopology(rapidxml::xml_node<> *xmlNode,
                               CT_TOPFNodeGroup *node,
                               int &nNodeReaded);

protected:
    /**
     * @brief Redefined to verify that file can be opened.
     */
    bool postVerifyFile(const QString &filepath) override;

    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;

    bool internalReadFile(CT_StandardItemGroup* rootGroup) override;

    bool protectedReadFile();
};

#endif // CT_READER_OPF_H
