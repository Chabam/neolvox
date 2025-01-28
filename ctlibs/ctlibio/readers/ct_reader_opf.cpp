#include "ct_reader_opf.h"

#include "ct_mesh/tools/ct_meshallocatort.h"
#include "ct_mesh/ct_face.h"
#include "ct_mesh/ct_edge.h"
#include "ct_math/ct_mathpoint.h"
#include "ct_view/ct_genericconfigurablewidget.h"
#include "ct_view/tools/ct_configurablewidgettodialog.h"

#include "ct_log/ct_logmanager.h"

#include <QFile>
#include <limits>

const QVector<QString> CT_Reader_OPF::TOPOLOGY_NAMES = QVector<QString>() << "topology" << "decomp" << "follow" << "branch";

CT_Reader_OPF::CT_Reader_OPF(int subMenuLevel) : SuperClass(subMenuLevel)
{
    m_loadMeshes = false;

    addNewReadableFormat(FileFormat("opf", tr("Fichiers AmapStudio .opf")));

    setToolTip(tr("Charge un fichier décrivant l'architecture de végétaux au format OPF, créé par le logiciel AMAPStudio.<br>http://amapstudio.cirad.fr"));
}

CT_Reader_OPF::CT_Reader_OPF(const CT_Reader_OPF &other) : SuperClass(other)
{
    m_loadMeshes = other.m_loadMeshes;
    m_typesNew = other.m_typesNew;
    m_attributesNew = other.m_attributesNew;
    m_totalNodeNew = other.m_totalNodeNew;

    m_types = other.m_types;
    m_attributes = other.m_attributes;
    m_totalNode = other.m_totalNode;
}

CT_Reader_OPF::~CT_Reader_OPF()
{
    clearMeshes();
    clearShapes();
}

QString CT_Reader_OPF::displayableName() const
{
    return tr("Fichier AmapStudio - OPF");
}

void CT_Reader_OPF::recursiveReadTopologyForModel(rapidxml::xml_node<> *node,
                                                  int &totalNode,
                                                  QHash<QString, CT_OPF_Type> &types,
                                                  const QHash<QString, CT_OPF_Attribute> &attributes,
                                                  int& nTypesCreated)
{
    ++totalNode;

    QString nameT = node->first_attribute("class", 0, false)->value();

    rapidxml::xml_attribute<>* scaleAttribute = node->first_attribute("scale", 0, false);
    rapidxml::xml_attribute<>* idAttribute = node->first_attribute("id", 0, false);

    CT_OPF_Type type = types.value(nameT, CT_OPF_Type(nameT,
                                                      scaleAttribute ? QString(scaleAttribute->value()).toInt() : 0,
                                                      idAttribute ? QString(idAttribute->value()).toInt() : 0,
                                                      nTypesCreated+1));

    if(type.m_indexCreation > nTypesCreated)
        nTypesCreated = type.m_indexCreation;

    const bool addAttributes = (type.m_attributes.size() != attributes.size());

    rapidxml::xml_node<> *nextNode = node->first_node();

    while(nextNode != nullptr)
    {
        QString name = nextNode->name();

        if(TOPOLOGY_NAMES.contains(name))
        {
            recursiveReadTopologyForModel(nextNode,
                                          totalNode,
                                          types,
                                          attributes,
                                          nTypesCreated);
        }
        else if(addAttributes
                && attributes.contains(name))
        {
            if(!type.m_attributes.contains(name))
                type.m_attributes.insert(name, attributes.value(name));
        }

        nextNode = nextNode->next_sibling();
    }

    types.insert(nameT, type);
}

void CT_Reader_OPF::recursiveReadTopology(rapidxml::xml_node<> *xmlNode,
                                          CT_TOPFNodeGroup *node,
                                          int &nNodeReaded)
{
    ++nNodeReaded;

    rapidxml::xml_attribute<>* idAttribute = xmlNode->first_attribute("id", 0, false);

    node->setOPFID(idAttribute ? QString(idAttribute->value()).toInt() : 0);
    const QString typeName = xmlNode->first_attribute("class", 0, false)->value();

    CT_ItemAttributeList *attList = new CT_ItemAttributeList();
    node->addSingularItem(*registeredHandlePtr<AttributeListHandleType>(typeName + "_a"), attList);

    rapidxml::xml_node<> *xmlChild = xmlNode->first_node();

    while(xmlChild != nullptr)
    {
        const QString name = xmlChild->name();
        CT_TOPFNodeGroup *newNode = nullptr;

        if(name == "geometry")
        {
            readGeometry(xmlChild, node, typeName);
        }
        else if(((name == "decomp") || (name == "follow")))
        {
            const QString typeName2 = xmlChild->first_attribute("class", 0, false)->value();

            newNode = new CT_TOPFNodeGroup();
            node->addComponent(*registeredHandlePtr<NodeGroupHandleType>(typeName2), newNode);
        }
        else if(name == "branch")
        {
            const QString typeName2 = xmlChild->first_attribute("class", 0, false)->value();

            newNode = new CT_TOPFNodeGroup();
            node->addBranch(*registeredHandlePtr<NodeGroupHandleType>(typeName2), newNode);
        }
        else if(m_attributes.contains(name))
        {
            CT_AbstractItemAttribute *att = staticCreateAttributeForType(m_attributes.value(name).m_type, xmlChild->value());

            if(att != nullptr)
                attList->addItemAttribute(*registeredHandlePtr<AbstractAttributeHandle>(typeName + "_" + name), att);
        }

        if(newNode != nullptr)
        {
            recursiveReadTopology(xmlChild,
                                  newNode,
                                  nNodeReaded);
        }

        xmlChild = xmlChild->next_sibling();

        setProgress((nNodeReaded*100)/m_totalNode);
    }
}

bool CT_Reader_OPF::postVerifyFile(const QString& filepath)
{
    QHash<QString, CT_OPF_Attribute>    attributes;
    QHash<QString, CT_OPF_Type>         types;
    int                                 totalNode = 0;

    rapidxml::file<> xmlFile(filepath.toLatin1().data());
    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlFile.data());

    rapidxml::xml_node<> *root = doc.first_node("opf", 0, false);

    int nTypesCreated = 0;

    if(root != nullptr)
    {
        rapidxml::xml_node<> *node = nullptr;

        node = root->first_node();

        while(node != nullptr)
        {
            QString nn = node->name();

            if(nn == "attributeBDD")
            {
                rapidxml::xml_node<> *child = node->first_node();

                while(child  != nullptr)
                {
                    const QString name = child->first_attribute("name", 0, false)->value();

                    attributes.insert(name, CT_OPF_Attribute(name, child->first_attribute("class", 0, false)->value(), attributes.size()));

                    child = child->next_sibling();
                }
            }
            else if(TOPOLOGY_NAMES.contains(nn))
            {
                recursiveReadTopologyForModel(node,
                                              totalNode,
                                              types,
                                              attributes,
                                              nTypesCreated);
                node = nullptr;
            }

            if(node != nullptr)
                node = node->next_sibling();
        }
    }

    if(!types.isEmpty())
    {
        m_typesNew = types;
        m_attributesNew = attributes;
        m_totalNodeNew = totalNode;

        return true;
    }

    PS_LOG->addErrorMessage(LogInterface::reader, tr("No types found in %1").arg(filepath));
    return false;
}

void CT_Reader_OPF::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    QHash<QString, CT_OutAbstractItemAttributeModel*>   attributesOriginalModels;

    // Tree Group
    manager.addGroup(m_topologyHandle, tr("Topologie"));

    QHashIterator<QString, CT_OPF_Type> it(m_types);

    QMap<int, CT_OPF_Type> typesSorted;

    while(it.hasNext())
    {
        it.next();

        const CT_OPF_Type &type = it.value();
        typesSorted.insert(type.m_indexCreation, type);
    }

    QMapIterator<int, CT_OPF_Type> itS(typesSorted);

    while(itS.hasNext())
    {
        itS.next();

        const CT_OPF_Type &type = itS.value();

        // Node Group
        NodeGroupHandleType* nodeHandle = new NodeGroupHandleType();
        manager.addGroup(m_topologyHandle, *nodeHandle, type.m_name);
        nodeHandle->visitModels([&type](const CT_OutOPFNodeGroupModel<CT_TOPFNodeGroup>* model) -> bool
        {
            const_cast<CT_OutOPFNodeGroupModel<CT_TOPFNodeGroup>*>(model)->setOPFLevel(type.m_level);
            return true;
        });
        registerHandlePtr(type.m_name, nodeHandle);

        // Mesh model
        MeshHandleType* meshHandle = new MeshHandleType();
        manager.addItem(*nodeHandle, *meshHandle, tr("Mesh"));
        registerHandlePtr(type.m_name + "_m", meshHandle);

        // Attribute list
        AttributeListHandleType* attListHandle = new AttributeListHandleType();
        manager.addItem(*nodeHandle, *attListHandle, type.m_name);
        registerHandlePtr(type.m_name + "_a", attListHandle);

        QHashIterator<QString, CT_OPF_Attribute> itA(type.m_attributes);

        QMap<int, CT_OPF_Attribute> attributesSorted;

        while(itA.hasNext())
        {
            itA.next();

            const CT_OPF_Attribute &attribute = itA.value();
            attributesSorted.insert(attribute.m_indexCreation, attribute);
        }

        QMapIterator<int, CT_OPF_Attribute> itAS(attributesSorted);

        while(itAS.hasNext())
        {
            itAS.next();

            const CT_OPF_Attribute &attribute = itAS.value();

            if((attribute.m_type == "String") || (attribute.m_type == "Color"))
                createAndAddItemAttributeHandle<AttributeStringHandleType>(manager, *attListHandle, type, attribute);
            else if(attribute.m_type == "Integer")
                createAndAddItemAttributeHandle<AttributeIntHandleType>(manager, *attListHandle, type, attribute);
            else if((attribute.m_type == "Double")
                    || (attribute.m_type == "Metre")
                    || (attribute.m_type == "Centimetre")
                    || (attribute.m_type == "Millimetre")
                    || (attribute.m_type == "10E-5 Metre"))
                createAndAddItemAttributeHandle<AttributeDoubleHandleType>(manager, *attListHandle, type, attribute);
            else if(attribute.m_type == "Boolean")
                createAndAddItemAttributeHandle<AttributeBoolHandleType>(manager, *attListHandle, type, attribute);
            else
                PS_LOG->addWarningMessage(LogInterface::reader, "CT_Reader_OPF::staticCreateAttributeForType ( type = " + attribute.m_type + " ) => Unknown Type");
        }
    }
}

bool CT_Reader_OPF::internalReadFile(CT_StandardItemGroup* rootGroup)
{
    clearMeshes();
    clearShapes();

    // Test File validity
    if(QFile::exists(filepath()))
    {
        rapidxml::file<> xmlFile(filepath().toLatin1().data());
        rapidxml::xml_document<> doc;
        doc.parse<0>(xmlFile.data());

        rapidxml::xml_node<> *xmlRoot = doc.first_node("opf", 0, false);

        int nNodeReaded = 0;

        if(xmlRoot != nullptr)
        {
            CT_TTreeGroup *tree = new CT_TTreeGroup();
            rootGroup->addGroup(m_topologyHandle, tree);

            rapidxml::xml_node<> *xmlNode = nullptr;

            xmlNode = xmlRoot->first_node();

            while(xmlNode != nullptr)
            {
                CT_TOPFNodeGroup *node = nullptr;

                QString nn = xmlNode->name();

                if(nn == "meshBDD")
                {
                    if(m_loadMeshes)
                    {
                        rapidxml::xml_node<> *xmlMesh = xmlNode->first_node("mesh", 0, false);

                        while(xmlMesh != nullptr)
                        {
                            readMesh(xmlMesh);
                            xmlMesh = xmlMesh->next_sibling();
                        }
                    }
                }
                else if(nn == "shapeBDD")
                {
                    if(m_loadMeshes)
                    {
                        rapidxml::xml_node<> *xmlShape = xmlNode->first_node("shape", 0, false);

                        while(xmlShape != nullptr)
                        {
                            readShape(xmlShape);
                            xmlShape = xmlShape->next_sibling();
                        }
                    }
                }
                else if(nn == "topology")
                {
                    const QString typeName = xmlNode->first_attribute("class", 0, false)->value();

                    node = new CT_TOPFNodeGroup();
                    tree->setRootNode(*registeredHandlePtr<NodeGroupHandleType>(typeName), node);
                }

                if(node != nullptr)
                {
                    recursiveReadTopology(xmlNode,
                                          node,
                                          nNodeReaded);
                }

                xmlNode = xmlNode->next_sibling();
            }
        }

        return true;
    }

    return false;
}

bool CT_Reader_OPF::configure()
{
    CT_GenericConfigurableWidget dialog;
    dialog.addBool("", "", tr("Charger le modèle 3D ? (lent si activé)"), m_loadMeshes, tr("Ralentit les performances lors du chargement des mesh"));

    if(CT_ConfigurableWidgetToDialog::exec(&dialog) == QDialog::Accepted) {
        finalizeSettings();
        return true;
    }

    return false;
}

void CT_Reader_OPF::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    // TODO : use the CT_GenericConfigurableWidget to save settings !

    writer.addParameter(this, "LoadMeshes", m_loadMeshes);
}

bool CT_Reader_OPF::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    // TODO : use the CT_GenericConfigurableWidget to restore settings !

    QVariant value;

    if(reader.parameter(this, "LoadMeshes", value))
        m_loadMeshes = value.toBool();

    finalizeSettings();

    return true;
}

CT_AbstractReader* CT_Reader_OPF::copy() const
{
    return new CT_Reader_OPF();
}

CT_AbstractItemAttribute* CT_Reader_OPF::staticCreateAttributeForType(const QString &type, const QString &value)
{
    if((type == "String") || (type == "Color"))
        return new CT_StdItemAttributeT<QString>(CT_AbstractCategory::DATA_VALUE, value);

    if(type == "Integer")
        return new CT_StdItemAttributeT<int>(CT_AbstractCategory::DATA_VALUE, value.toInt());

    if((type == "Double")
            || (type == "Metre")
            || (type == "Centimetre")
            || (type == "Millimetre")
            || (type == "10E-5 Metre"))
        return new CT_StdItemAttributeT<double>(CT_AbstractCategory::DATA_VALUE, value.toDouble());

    if(type == "Boolean")
        return new CT_StdItemAttributeT<bool>(CT_AbstractCategory::DATA_VALUE, value.toLower() == "true");

    return nullptr;
}

void CT_Reader_OPF::finalizeSettings()
{
    m_types = m_typesNew;
    m_attributes = m_attributesNew;
    m_totalNode = m_totalNodeNew;
}

void CT_Reader_OPF::clearMeshes()
{
    qDeleteAll(m_meshes.begin(), m_meshes.end());
    m_meshes.clear();
}

void CT_Reader_OPF::clearShapes()
{
    m_shapes.clear();
}

void CT_Reader_OPF::readMesh(rapidxml::xml_node<> *xmlNode)
{
    int id = QString(xmlNode->first_attribute("Id", 0, false)->value()).toInt();

    CT_OPF_Mesh *mesh = new CT_OPF_Mesh();
    mesh->m_mesh = new CT_Mesh();

    size_t debPointIndex = 0;

    rapidxml::xml_node<> *xmlChild = xmlNode->first_node();

    while(xmlChild != nullptr)
    {
        QString nn = xmlChild->name();

        if(nn == "points")
        {
            QString tmp = QString(xmlChild->value()).trimmed();
            QStringList points = tmp.split(QRegExp("\\s+"));

            QListIterator<QString> it(points);

            const int size = points.size()/3;

            CT_MutablePointIterator itP = CT_MeshAllocator::AddVertices(mesh->m_mesh, size);
            debPointIndex = itP.next().cIndex();

            CT_Point pAdded;

            int i = 0;

            while(i<size)
            {
                pAdded(0) = it.next().toDouble();
                pAdded(1) = it.next().toDouble();
                pAdded(2) = it.next().toDouble();

                itP.replaceCurrentPoint(pAdded);

                ++i;
                itP.next();
            }
        }
        else if(nn == "faces")
        {
            rapidxml::xml_node<> *xmlFace = xmlChild->first_node("face", 0, false);

            while(xmlFace != nullptr)
            {
                QString tmp = QString(xmlFace->value()).trimmed();
                QStringList points = tmp.split(QRegExp("\\s+"));

                CT_MutableFaceIterator itF = CT_MeshAllocator::AddFaces(mesh->m_mesh, 1);

                CT_Face &face = itF.next().cT();

                CT_MutableEdgeIterator beginHe = CT_MeshAllocator::AddHEdges(mesh->m_mesh, 3);

                size_t faceIndex = itF.cIndex();

                size_t p0 = points.at(0).toInt() + debPointIndex;
                size_t p1 = points.at(1).toInt() + debPointIndex;
                size_t p2 = points.at(2).toInt() + debPointIndex;

                size_t e1Index = beginHe.next().cIndex();
                size_t e2Index;
                size_t e3Index;

                face.setEdge(e1Index);

                CT_Edge &e1 = beginHe.cT();
                e1.setPoint0(p0);
                e1.setPoint1(p1);
                e1.setFace(faceIndex);

                CT_Edge &e2 = beginHe.next().cT();
                e2.setPoint0(p1);
                e2.setPoint1(p2);
                e1.setFace(faceIndex);
                e2Index = beginHe.cIndex();

                CT_Edge &e3 = beginHe.next().cT();
                e3.setPoint0(p2);
                e3.setPoint1(p0);
                e3.setFace(faceIndex);
                e3Index = beginHe.cIndex();

                e1.setNext(e2Index);
                e1.setPrevious(e3Index);
                e2.setNext(e3Index);
                e2.setPrevious(e1Index);
                e3.setNext(e1Index);
                e3.setPrevious(e2Index);

                xmlFace = xmlFace->next_sibling();
            }
        }

        xmlChild = xmlChild->next_sibling();
    }

    m_meshes.insert(id, mesh);
}

void CT_Reader_OPF::readShape(rapidxml::xml_node<> *xmlNode)
{
    int id = QString(xmlNode->first_attribute("Id", 0, false)->value()).toInt();

    rapidxml::xml_node<> *xmlChild = xmlNode->first_node("meshIndex", 0, false);

    if(xmlChild != nullptr)
        m_shapes.insert(id, m_meshes.value(QString(xmlChild->value()).toInt(), nullptr));
}

void CT_Reader_OPF::readGeometry(rapidxml::xml_node<> *xmlNode, CT_TOPFNodeGroup *node, const QString &typeName)
{
    CT_OPF_Mesh *mesh = nullptr;
    double dUp = 1, dDwn = 1;

    rapidxml::xml_node<> *xmlChild = xmlNode->first_node();

    while(xmlChild != nullptr)
    {
        QString nn = xmlChild->name();

        if(nn == "shapeIndex")
        {
            mesh = m_shapes.value(QString(xmlChild->value()).toInt(), nullptr);
        }
        else if(nn == "mat")
        {
            QStringList mat = QString(xmlChild->value()).simplified().split(QRegExp("\\s+"));
            QMatrix4x4 matrix(mat.at(0).toDouble()/100.0, mat.at(1).toDouble()/100.0, mat.at(2).toDouble()/100.0, mat.at(3).toDouble()/100.0,
                              mat.at(4).toDouble()/100.0, mat.at(5).toDouble()/100.0, mat.at(6).toDouble()/100.0, mat.at(7).toDouble()/100.0,
                              mat.at(8).toDouble()/100.0, mat.at(9).toDouble()/100.0, mat.at(10).toDouble()/100.0, mat.at(11).toDouble()/100.0,
                              0,0,0,1);

            node->setOPFMatrix(matrix);
        }
        else if(nn == "dUp")
        {
            dUp = QString(xmlChild->value()).toDouble();
        }
        else if(nn == "dDwn")
        {
            dDwn = QString(xmlChild->value()).toDouble();
        }

        xmlChild = xmlChild->next_sibling();
    }

    if(m_loadMeshes)
    {
        if(mesh != nullptr)
        {
            Eigen::Vector3d min, max;
            mesh->getBoundingBox(min, max);

            transformAndCreateMesh(mesh->m_mesh, min, max, node, dUp, dDwn, typeName);
        }
        else
        {
            if(m_cylinderMesh.m_mesh == nullptr)
            {
                m_cylinderMesh.m_mesh = new CT_Mesh();
                m_cylinderMesh.m_mesh->createCylinder(0.5,0.5,10);
            }

            Eigen::Vector3d min, max;
            m_cylinderMesh.getBoundingBox(min, max);

            transformAndCreateMesh(m_cylinderMesh.m_mesh, min, max, node, dUp, dDwn, typeName);
        }
    }
}

void CT_Reader_OPF::transformAndCreateMesh(CT_Mesh *mesh, Eigen::Vector3d &min, Eigen::Vector3d &max, CT_TOPFNodeGroup *node, const double &dUp, const double &dDwn, const QString &typeName)
{
    CT_Mesh *newMesh = new CT_Mesh();

    double deltaX = max.x() - min.x();

    if(deltaX == 0)
        return;

    const QMatrix4x4 &matrix = node->opfMatrix();

    double deltaD = dDwn - dUp;

    size_t nP = mesh->vert().size();
    size_t nF = mesh->face().size();

    CT_MutablePointIterator itP = CT_MeshAllocator::AddVertices(newMesh, nP);
    CT_MutableFaceIterator itF = CT_MeshAllocator::AddFaces(newMesh, nF);
    CT_MutableEdgeIterator itE = CT_MeshAllocator::AddHEdges(newMesh, nF*3);

    CT_MutablePointIterator itOP(mesh->pVert());
    CT_MutableFaceIterator itOF(mesh->pFace());

    if(itP.hasNext()){

        itP.next();

        const size_t debPointIndex = itP.cIndex() - mesh->abstractVert()->first();

        itP.toFront();

        while(itP.hasNext())
        {
            itOP.next();
            itP.next();

            const CT_Point &oPoint = itOP.currentPoint();
            CT_Point point;

            const float dx = oPoint[CT_Point::X] - min.x();
            const float factorW = dDwn - (deltaD * (dx / deltaX));

            point.setX(oPoint[CT_Point::X]);
            point.setY(oPoint[CT_Point::Y] * factorW);
            point.setZ(oPoint[CT_Point::Z] * factorW);

            CT_MathPoint::transform(matrix, point);

            point.setX(point[CT_Point::X]/100.0);
            point.setY(point[CT_Point::Y]/100.0);
            point.setZ(point[CT_Point::Z]/100.0);

            itP.replaceCurrentPoint(point);
        }

        while(itF.hasNext())
        {
            itOF.next();
            itF.next();

            const CT_Face &oFace = itOF.cT();
            CT_Face &face = itF.cT();

            itE.next();

            const size_t faceIndex = itF.cIndex();

            const size_t p0 = oFace.iPointAt(0) + debPointIndex;
            const size_t p1 = oFace.iPointAt(1) + debPointIndex;
            const size_t p2 = oFace.iPointAt(2) + debPointIndex;

            const size_t e1Index = itE.cIndex();
            size_t e2Index;
            size_t e3Index;

            face.setEdge(e1Index);

            CT_Edge &e1 = itE.cT();
            e1.setPoint0(p0);
            e1.setPoint1(p1);
            e1.setFace(faceIndex);
            itE.next();

            CT_Edge &e2 = itE.cT();
            e2.setPoint0(p1);
            e2.setPoint1(p2);
            e1.setFace(faceIndex);
            e2Index = itE.cIndex();
            itE.next();

            CT_Edge &e3 = itE.cT();
            e3.setPoint0(p2);
            e3.setPoint1(p0);
            e3.setFace(faceIndex);
            e3Index = itE.cIndex();

            e1.setNext(e2Index);
            e1.setPrevious(e3Index);
            e2.setNext(e3Index);
            e2.setPrevious(e1Index);
            e3.setNext(e1Index);
            e3.setPrevious(e2Index);
        }
    }

    CT_OPFMeshModel *meshModel = new CT_OPFMeshModel(newMesh);
    meshModel->setDUp(dUp);
    meshModel->setDDown(dDwn);

    node->addSingularItem(*registeredHandlePtr<MeshHandleType>(typeName +  + "_m"), meshModel);
}

// CT_OPF_Mesh //

void CT_OPF_Mesh::getBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    if(!m_bboxOK)
    {
        m_min(0) = std::numeric_limits<double>::max();
        m_min(1) = m_min.x();
        m_min(2) = m_min.x();

        m_max(0) = -m_min.x();
        m_max(1) = m_max.x();
        m_max(2) = m_max.x();

        CT_PointIterator it(m_mesh->abstractVert());

        while(it.hasNext())
        {
            const CT_Point &p = it.next().currentPoint();

            if(p(0) > m_max.x())
                m_max(0) = p(0);

            if(p(1) > m_max.y())
                m_max(1) = p(1);

            if(p(2) > m_max.z())
                m_max(2) = p(2);

            if(p(0) < m_min.x())
                m_min(0) = p(0);

            if(p(1) < m_min.y())
                m_min(1) = p(1);

            if(p(2) < m_min.z())
                m_min(2) = p(2);
        }

        m_bboxOK = true;
    }

    min = m_min;
    max = m_max;
}
