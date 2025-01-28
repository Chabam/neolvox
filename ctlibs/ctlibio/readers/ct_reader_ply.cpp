#include "ct_reader_ply.h"

#include <QDialog>

#include "readers/headers/ct_plyheader.h"
#include "readers/tools/ply/plyheaderreader.h"
#include "readers/tools/ply/plyfilepartreader.h"
#include "readers/tools/ply/plycomputreewrappers.h"

#include "ct_view/tools/ct_configurablewidgettodialog.h"

#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_normalcloud/ct_normalcloudstdvector.h"

#include "ct_log/ct_logmanager.h"

#define DEF_Scene "sce"
#define DEF_Color "col"
#define DEF_Normal "nor"
#define DEF_Scalar "sca"

#define readConfiguration(SNG, ELNAME, VAR) snv = SNG->firstValueByTagName(ELNAME); \
                                            if(snv == nullptr) \
                                                return false; \
                                            VAR = snv->value().toInt(&ok); \
                                            if(!ok) \
                                                return false;

#define readNewConfiguration(ELNAME, VAR)   if(!reader.parameter(this, ELNAME, value)) \
                                                return false; \
                                            VAR = value.toInt(&ok); \
                                            if(!ok) \
                                                return false;

#define readNewConfigurationInfo(ELNAME, VAR)   if(!reader.parameterInfo(this, pID, ELNAME, value)) \
                                                    return false; \
                                                VAR = value.toInt(&ok); \
                                                if(!ok) \
                                                    return false;

CT_Reader_PLY::CT_Reader_PLY(int subMenuLevel) : SuperClass(subMenuLevel), CT_ReaderPointsFilteringExtension()
{
    addNewReadableFormat(FileFormat("ply", tr("Fichiers de géométrie 3D .ply")));

    setToolTip(tr("Charge un nuage de points depuis un fichier au format PLY (Objet 3D)"));
}

CT_Reader_PLY::CT_Reader_PLY(const CT_Reader_PLY &other) : SuperClass(other), CT_ReaderPointsFilteringExtension()
{

}

CT_Reader_PLY::~CT_Reader_PLY()
{
    qDeleteAll(m_outColorVector);
    qDeleteAll(m_outNormalVector);
    qDeleteAll(m_outScalarVector);
}

QString CT_Reader_PLY::displayableName() const
{
    return tr("Points, Fichier PLY");
}

bool CT_Reader_PLY::configure()
{
    PlyHeaderReader headerReader;
    headerReader.setFilePath(filepath());

    if(!headerReader.process()) {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("L'en-tête du fichier \"%1\" n'est pas formé correctement.").arg(filepath()));
        return false;
    }

    GPly_CT_ReadConfiguration_Wrapper config;
    config.plyConfigurationWidget()->setHeader(headerReader.getHeader());

    if(m_config.vertex.elementIndex != -1)
        config.plyConfigurationWidget()->setConfiguration(m_config);

    if (CT_ConfigurableWidgetToDialog::exec(&config) == QDialog::Accepted)
        return setConfiguration(config.plyConfigurationWidget()->getConfiguration());

    return false;
}

bool CT_Reader_PLY::setConfiguration(const PlyReadConfiguration &config)
{
    m_config = config;
    return true;
}

PlyReadConfiguration CT_Reader_PLY::getConfiguration() const
{
    return m_config;
}

CT_FileHeader* CT_Reader_PLY::createHeaderPrototype() const
{
    return new CT_PLYHeader();
}

void CT_Reader_PLY::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    writer.addParameter(this, "elementIndex", m_config.vertex.elementIndex);
    writer.addParameter(this, "xPropertyIndex", m_config.vertex.xPropertyIndex);
    writer.addParameter(this, "yPropertyIndex", m_config.vertex.yPropertyIndex);
    writer.addParameter(this, "zPropertyIndex", m_config.vertex.zPropertyIndex);

    foreach (const PlyColorConfiguration& cc, m_config.colors) {
        const int pID = writer.addParameter(this, "Color", true);

        writer.addParameterInfo(this, pID, "elementIndex", cc.elementIndex);
        writer.addParameterInfo(this, pID, "rPropertyIndex", cc.rPropertyIndex);
        writer.addParameterInfo(this, pID, "gPropertyIndex", cc.gPropertyIndex);
        writer.addParameterInfo(this, pID, "bPropertyIndex", cc.bPropertyIndex);
        writer.addParameterInfo(this, pID, "aPropertyIndex", cc.aPropertyIndex);
    }

    foreach (const PlyNormalConfiguration& cc, m_config.normals) {
        const int pID = writer.addParameter(this, "Normal", true);
        writer.addParameterInfo(this, pID, "elementIndex", cc.elementIndex);
        writer.addParameterInfo(this, pID, "nxPropertyIndex", cc.nxPropertyIndex);
        writer.addParameterInfo(this, pID, "nyPropertyIndex", cc.nyPropertyIndex);
        writer.addParameterInfo(this, pID, "nzPropertyIndex", cc.nzPropertyIndex);
        writer.addParameterInfo(this, pID, "curvaturePropertyIndex", cc.ncPropertyIndex);
    }

    foreach (const PlyScalarConfiguration& cc, m_config.scalars) {
        const int pID = writer.addParameter(this, "Scalar", true);
        writer.addParameterInfo(this, pID, "elementIndex", cc.elementIndex);
        writer.addParameterInfo(this, pID, "valuePropertyIndex", cc.sPropertyIndex);
    }
}

bool CT_Reader_PLY::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    QVariant value;
    bool ok;
    PlyReadConfiguration config;

    readNewConfiguration("elementIndex", config.vertex.elementIndex);
    readNewConfiguration("xPropertyIndex", config.vertex.xPropertyIndex);
    readNewConfiguration("yPropertyIndex", config.vertex.yPropertyIndex);
    readNewConfiguration("zPropertyIndex", config.vertex.zPropertyIndex);

    const int nColor = reader.parameterCount(this, "Color");

    for(int i=0; i<nColor; ++i) {
        PlyColorConfiguration cc;
        const int pID = reader.parameter(this, "Color", value);

        if(pID) {
            readNewConfigurationInfo("elementIndex", cc.elementIndex);
            readNewConfigurationInfo("rPropertyIndex", cc.rPropertyIndex);
            readNewConfigurationInfo("gPropertyIndex", cc.gPropertyIndex);
            readNewConfigurationInfo("bPropertyIndex", cc.bPropertyIndex);
            readNewConfigurationInfo("aPropertyIndex", cc.aPropertyIndex);

            config.colors.append(cc);
        }
    }

    const int nNormal = reader.parameterCount(this, "Normal");

    for(int i=0; i<nNormal; ++i) {
        PlyNormalConfiguration nc;
        const int pID = reader.parameter(this, "Normal", value);

        if(pID) {
            readNewConfigurationInfo("elementIndex", nc.elementIndex);
            readNewConfigurationInfo("nxPropertyIndex", nc.nxPropertyIndex);
            readNewConfigurationInfo("nyPropertyIndex", nc.nyPropertyIndex);
            readNewConfigurationInfo("nzPropertyIndex", nc.nzPropertyIndex);
            readNewConfigurationInfo("curvaturePropertyIndex", nc.ncPropertyIndex);

            config.normals.append(nc);
        }
    }

    const int nScalar = reader.parameterCount(this, "Scalar");

    for(int i=0; i<nScalar; ++i) {
        PlyScalarConfiguration sc;
        const int pID = reader.parameter(this, "Scalar", value);

        if(pID) {
            readNewConfigurationInfo("elementIndex", sc.elementIndex);
            readNewConfigurationInfo("valuePropertyIndex", sc.sPropertyIndex);

            config.scalars.append(sc);
        }
    }

    return setConfiguration(config);
}

bool CT_Reader_PLY::plyReadMustStop() const
{
    return isStopped();
}

void CT_Reader_PLY::plyProgressChanged(int progress)
{
    setProgress(progress);
}


void CT_Reader_PLY::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_outScene, tr("Scene"));

    int size = m_config.colors.size();

    qDeleteAll(m_outColorVector);
    qDeleteAll(m_outNormalVector);
    qDeleteAll(m_outScalarVector);

    m_outColorVector.resize(size);

    for(int i = 0 ; i < size ; ++i)
    {
        m_outColorVector[i] = new CT_HandleOutPointColorWithDenseManager();
        manager.addItem(*m_outColorVector[i], tr("Color %1").arg(i+1));
    }

    size = m_config.normals.size();
    m_outNormalVector.resize(size);

    for(int i = 0 ; i < size ; ++i)
    {
        m_outNormalVector[i] = new CT_HandleOutPointNormalWithDenseManager();
        manager.addItem(*m_outNormalVector[i], tr("Normal %1").arg(i+1));
    }

    size = m_config.scalars.size();
    m_outScalarVector.resize(size);

    for(int i = 0 ; i < size ; ++i)
    {
        m_outScalarVector[i] = new CT_HandleOutPointScalarWithDenseManager<float>();
        manager.addItem(*m_outScalarVector[i], tr("Scalar %1").arg(i+1));
    }
}

typedef Ply_CT_GenericCloud_Wrapper<CT_DensePointColorManager, CT_Color> Ply_CT_ColorCloud_Wrapper;
typedef Ply_CT_GenericCloud_Wrapper<CT_DensePointNormalManager, CT_Normal> Ply_CT_NormalCloud_Wrapper;

// c++11
template<typename ValueInCloud>
using Ply_CT_ScalarCloud_Wrapper = Ply_CT_GenericCloud_Wrapper<CT_DensePointScalarManager<ValueInCloud>, ValueInCloud>;

bool CT_Reader_PLY::internalReadFile(CT_StandardItemGroup* group)
{
    PlyHeaderReader headerReader;
    headerReader.setFilePath(filepath());

    if(!headerReader.process())
        return false;

    QList<Ply_CT_ColorCloud_Wrapper*> colorsCloud;
    QList<Ply_CT_NormalCloud_Wrapper*> normalsCloud;
    QList<Ply_CT_ScalarCloud_Wrapper<float>*> scalarsCloud;

    PlyHeader header = headerReader.getHeader();
    PlyFilePartReader<Ply_CT_PointCloud_Wrapper,
            Ply_CT_ScalarCloud_Wrapper<float>,
            Ply_CT_ColorCloud_Wrapper,
            Ply_CT_NormalCloud_Wrapper> partReader(header);

    partReader.setListener(this);

    QList<Ply_CT_ColorCloud_Wrapper*> colorWrappers;
    QList<Ply_CT_NormalCloud_Wrapper*> normalWrappers;
    QList<Ply_CT_ScalarCloud_Wrapper<float>*> scalarWrappers;

    auto f = [&colorWrappers, &normalWrappers, &scalarWrappers](CT_NMPCIR pcir)
    {
        for(Ply_CT_ColorCloud_Wrapper* wrapper : colorWrappers)
        {
            wrapper->setPCIR(pcir);
        }

        for(Ply_CT_NormalCloud_Wrapper* wrapper : normalWrappers)
        {
            wrapper->setPCIR(pcir);
        }

        for(Ply_CT_ScalarCloud_Wrapper<float>* wrapper : scalarWrappers)
        {
            wrapper->setPCIR(pcir);
        }
    };

    Ply_CT_PointCloud_Wrapper wrapper(f);
    partReader.setVertexPart(&wrapper,
                             m_config.vertex.elementIndex,
                             m_config.vertex.xPropertyIndex,
                             m_config.vertex.yPropertyIndex,
                             m_config.vertex.zPropertyIndex);

    int i = 0;
    foreach(const PlyColorConfiguration& cc, m_config.colors) {
        Ply_CT_ColorCloud_Wrapper* colorCloud = new Ply_CT_ColorCloud_Wrapper(*m_outColorVector[i++]);
        colorWrappers.append(colorCloud);

        partReader.addVertexColorPart(colorCloud,
                                      cc.elementIndex,
                                      cc.rPropertyIndex,
                                      cc.gPropertyIndex,
                                      cc.bPropertyIndex,
                                      cc.aPropertyIndex);

        colorsCloud.append(colorCloud);
    }

    i = 0;
    foreach(const PlyNormalConfiguration& cc, m_config.normals) {
        Ply_CT_NormalCloud_Wrapper* normalCloud = new Ply_CT_NormalCloud_Wrapper(*m_outNormalVector[i++]);
        normalWrappers.append(normalCloud);

        partReader.addVertexNormalPart(normalCloud,
                                       cc.elementIndex,
                                       cc.nxPropertyIndex,
                                       cc.nyPropertyIndex,
                                       cc.nzPropertyIndex,
                                       cc.ncPropertyIndex);

        normalsCloud.append(normalCloud);
    }

    i = 0;
    foreach(const PlyScalarConfiguration& cc, m_config.scalars) {
        Ply_CT_ScalarCloud_Wrapper<float>* scalarCloud = new Ply_CT_ScalarCloud_Wrapper<float>(*m_outScalarVector[i++]);
        scalarWrappers.append(scalarCloud);

        partReader.addVertexScalarPart(scalarCloud,
                                       cc.elementIndex,
                                       cc.sPropertyIndex);

        scalarsCloud.append(scalarCloud);
    }

    partReader.process();

    if (filterSet())
    {
        PS_LOG->addWarningMessage(LogInterface::step, tr("ATTENTION : les filtres ne sont pas pris en charges pour le format PLY pour le moment. Tous les points seront chargés."));
    }

    CT_Scene* scene = new CT_Scene(wrapper.pcir);
    scene->updateBoundingBox();

    m_readScene = scene;

    // add the scene
    group->addSingularItem(m_outScene, scene);

    for(CT_HandleOutPointColorWithDenseManager* outColorHandle : m_outColorVector)
    {
        group->addSingularItem(*outColorHandle, outColorHandle->createAttributeInstance(wrapper.pcir));
    }

    for(CT_HandleOutPointNormalWithDenseManager* outNormalHandle : m_outNormalVector)
    {
        group->addSingularItem(*outNormalHandle, outNormalHandle->createAttributeInstance(wrapper.pcir));
    }

    for(CT_HandleOutPointScalarWithDenseManager<float>* outScalarHandle : m_outScalarVector)
    {
        group->addSingularItem(*outScalarHandle, outScalarHandle->createAttributeInstance(wrapper.pcir));
    }

    qDeleteAll(colorsCloud.begin(), colorsCloud.end());
    qDeleteAll(normalsCloud.begin(), normalsCloud.end());
    qDeleteAll(scalarsCloud.begin(), scalarsCloud.end());

    return true;
}

CT_FileHeader* CT_Reader_PLY::internalReadHeader(const QString &filepath, QString &error) const
{
    Q_UNUSED(error);

    PlyHeaderReader headerReader;
    headerReader.setFilePath(filepath);
    headerReader.process();

    CT_PLYHeader* ctH = new CT_PLYHeader();
    ctH->setHeader(headerReader.getHeader());

    return ctH;
}
