#include "ct_reader_ptx.h"

#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_view/ct_genericconfigurablewidget.h"
#include "ct_view/tools/ct_configurablewidgettodialog.h"

#include <QTextStream>

#define DEF_CT_Reader_PTX_groupOut      "grp"
#define DEF_CT_Reader_PTX_sceneOut      "sce"
#define DEF_CT_Reader_PTX_intensityOut  "int"
#define DEF_CT_Reader_PTX_rgbOut        "rgb"
#define DEF_CT_Reader_PTX_scannerOut    "sca"
#define DEF_CT_Reader_PTX_matrixOut     "mt"

CT_Reader_PTX::CT_Reader_PTX(int subMenuLevel) : SuperClass(subMenuLevel), CT_ReaderPointsFilteringExtension()
{
    m_applyTransformation = true;

    addNewReadableFormat(FileFormat("ptx", tr("Fichiers de points .ptx")));

    setToolTip(tr("Chargement de points depuis un fichier format ascii PTX"));
}

CT_Reader_PTX::CT_Reader_PTX(const CT_Reader_PTX &other) : SuperClass(other), CT_ReaderPointsFilteringExtension()
{
    m_applyTransformation = other.m_applyTransformation;
}

QString CT_Reader_PTX::displayableName() const
{
    return tr("Points, Fichier PTX");
}


bool CT_Reader_PTX::setFilePath(const QString &filepath)
{
    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if(f.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&f);

            int nColumn, nRow;
            Eigen::Matrix4d matrix;
            bool hasColors;

            if(readHeaderValues(stream, nColumn, nRow, matrix, hasColors)) {
                CT_AbstractReader::setFilePath(filepath);
                return true;
            }

            f.close();;
        }
    }

    return false;
}

bool CT_Reader_PTX::configure()
{
    CT_GenericConfigurableWidget w;
    w.addBool("","",tr("Appliquer la matrice de transformation"), m_applyTransformation);

    CT_ConfigurableWidgetToDialog d;
    return (d.exec(&w) == QDialog::Accepted);
}

void CT_Reader_PTX::setApplyTransformationMatrix(bool enable)
{
    m_applyTransformation = enable;
}

bool CT_Reader_PTX::applyTransformationMatrix() const
{
    return m_applyTransformation;
}

void CT_Reader_PTX::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    writer.addParameter(this, "applyTransformationMatrix", applyTransformationMatrix());
}

bool CT_Reader_PTX::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    QVariant value;

    if(reader.parameter(this, "applyTransformationMatrix", value))
        setApplyTransformationMatrix(value.toBool());

    return true;
}

bool CT_Reader_PTX::readHeaderValues(QTextStream &stream, int &nColumn, int &nRow, Eigen::Matrix4d &matrix, bool &hasColors) const
{
    if(stream.atEnd())
        return false;

    QString firstLine = stream.readLine().simplified();

    if(firstLine.isEmpty()) {
        if(stream.atEnd())
            return false;

        firstLine = stream.readLine();
    }

    bool ok;
    nColumn = firstLine.toInt(&ok);

    if(!ok || stream.atEnd())
        return false;

    nRow = stream.readLine().toInt(&ok);

    if(!ok || stream.atEnd())
        return false;

    // skip 4 lines
    stream.readLine();
    if(stream.atEnd())
        return false;

    stream.readLine();
    if(stream.atEnd())
        return false;

    stream.readLine();
    if(stream.atEnd())
        return false;

    stream.readLine();
    if(stream.atEnd())
        return false;

    // read matrix
    // the position is the fourth line of the transformation matrix
    for(int i=0; i<4; ++i) {
        for(int j=0; j<4; ++j) {
            stream >> matrix(i,j);
        }
    }

    qint64 pos = stream.pos();
    firstLine = stream.readLine().simplified();

    if(firstLine.isEmpty()) {
        if(stream.atEnd())
            return false;

        pos = stream.pos();
        firstLine = stream.readLine();
    }

    QString pointLine = firstLine.simplified();
    int nPointsCol = pointLine.split(" ").size();
    hasColors = (nPointsCol == 7);
    stream.seek(pos);

    return true;
}

void CT_Reader_PTX::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_outScene, tr("Scène"));
    manager.addItem(m_outIntensity, tr("Intensité"));
    manager.addItem(m_outRGB, tr("RGB"));
    manager.addItem(m_outMatrix, tr("Matrice de transformation"));
    manager.addItem(m_outScanner, tr("Scanner"));
}

bool CT_Reader_PTX::internalReadFile(CT_StandardItemGroup* group)
{
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if(f.open(QIODevice::ReadOnly))
        {
            int nColumn, nRow;
            Eigen::Matrix4d matrix;
            bool hasColors;

            // If filtering function set, do a first scan of the file to count filtered point number
            size_t numberOfFilteredPoints = 0;
            if (filterSet())
            {
                QTextStream stream(&f);
                while(readHeaderValues(stream, nColumn, nRow, matrix, hasColors))
                {
                    CT_Point pReaded;

                    Eigen::Vector4d pXYZ;
                    double reflectance;
                    quint16 r,g,b;

                    size_t a = 0;

                    size_t nPoints = size_t(nRow)*size_t(nColumn);

                    while((a < nPoints)
                          && !stream.atEnd()
                          && !isStopped())
                    {
                        stream >> pXYZ(0);
                        stream >> pXYZ(1);
                        stream >> pXYZ(2);
                        stream >> reflectance;

                        if(hasColors) {
                            stream >> r;
                            stream >> g;
                            stream >> b;
                        }

                        if(m_applyTransformation) {
                            //pXYZ = matrix * pXYZ;
                            // FP corrected transformation: rotation+translation that apply just to position
                            // Tested 19/10/2017
                            pReaded(CT_Point::X) = pXYZ(0)*matrix(0,0)+pXYZ(1)*matrix(1,0)+pXYZ(2)*matrix(2,0)+matrix(3,0);//scan origin shift
                            pReaded(CT_Point::Y) = pXYZ(0)*matrix(0,1)+pXYZ(1)*matrix(1,1)+pXYZ(2)*matrix(2,1)+matrix(3,1);//scan origin shift
                            pReaded(CT_Point::Z) = pXYZ(0)*matrix(0,2)+pXYZ(1)*matrix(1,2)+pXYZ(2)*matrix(2,2)+matrix(3,2);//scan origin shift
                        } else {
                            pReaded(CT_Point::X) = pXYZ(0);
                            pReaded(CT_Point::Y) = pXYZ(1);
                            pReaded(CT_Point::Z) = pXYZ(2);
                        }

                        if (isPointFiltered(pReaded))
                        {
                            ++numberOfFilteredPoints;
                        }

                        ++a;
                    }
                }
            }


            QTextStream stream(&f);

            while(readHeaderValues(stream, nColumn, nRow, matrix, hasColors)) {

                size_t nPoints = size_t(nRow)*size_t(nColumn) - numberOfFilteredPoints;

                CT_Point pReaded;

                CT_NMPCIR pcir = PS_REPOSITORY->createNewPointCloud(nPoints);
                CT_MutablePointIterator it(pcir);

                CT_DensePointScalarManager<double>::Setter intensitySetter = m_outIntensity.createAttributesSetter(pcir);
                CT_DensePointColorManager::SetterPtr colorSetter;

                if(hasColors)
                    colorSetter = m_outRGB.createAttributesSetterPtr(pcir);

                double xmin = std::numeric_limits<double>::max();
                double ymin = std::numeric_limits<double>::max();
                double zmin = std::numeric_limits<double>::max();
                double imin = std::numeric_limits<double>::max();

                double xmax = -std::numeric_limits<double>::max();
                double ymax = -std::numeric_limits<double>::max();
                double zmax = -std::numeric_limits<double>::max();
                double imax = -std::numeric_limits<double>::max();

                Eigen::Vector4d pXYZ;
                double reflectance;
                quint16 r,g,b;
                CT_Color color;
                size_t a = 0;

                while((a<nPoints)
                      && !stream.atEnd()
                      && !isStopped())
                {
                    stream >> pXYZ(0);
                    stream >> pXYZ(1);
                    stream >> pXYZ(2);
                    stream >> reflectance;

                    if(hasColors) {
                        stream >> r;
                        stream >> g;
                        stream >> b;

                        color.r() = uchar(r);
                        color.g() = uchar(g);
                        color.b() = uchar(b);

                        colorSetter->setValueWithLocalIndex(a, color);
                    }

                    if(m_applyTransformation) {
                        //pXYZ = matrix * pXYZ;
                        // FP corrected transformation: rotation+translation that apply just to position
                        // Tested 19/10/2017
                        pReaded(CT_Point::X) = pXYZ(0)*matrix(0,0)+pXYZ(1)*matrix(1,0)+pXYZ(2)*matrix(2,0)+matrix(3,0);//scan origin shift
                        pReaded(CT_Point::Y) = pXYZ(0)*matrix(0,1)+pXYZ(1)*matrix(1,1)+pXYZ(2)*matrix(2,1)+matrix(3,1);//scan origin shift
                        pReaded(CT_Point::Z) = pXYZ(0)*matrix(0,2)+pXYZ(1)*matrix(1,2)+pXYZ(2)*matrix(2,2)+matrix(3,2);//scan origin shift
                    } else {
                        pReaded(CT_Point::X) = pXYZ(0);
                        pReaded(CT_Point::Y) = pXYZ(1);
                        pReaded(CT_Point::Z) = pXYZ(2);
                    }

                    if (!isPointFiltered(pReaded))
                    {
                        xmin = qMin(pReaded(CT_Point::X), xmin);
                        ymin = qMin(pReaded(CT_Point::Y), ymin);
                        zmin = qMin(pReaded(CT_Point::Z), zmin);
                        xmax = qMax(pReaded(CT_Point::X), xmax);
                        ymax = qMax(pReaded(CT_Point::Y), ymax);
                        zmax = qMax(pReaded(CT_Point::Z), zmax);
                        imin = qMin(reflectance, imin);
                        imax = qMax(reflectance, imax);

                        it.next();
                        it.replaceCurrentPoint(pReaded);

                        intensitySetter.setValueWithLocalIndex(a, reflectance);
                    }

                    ++a;
                    setProgress(int(a*100/nPoints));
                }

                CT_Scene *scene = new CT_Scene(pcir);
                scene->setBoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);

                m_readScene = scene;

                // add the scene
                group->addSingularItem(m_outScene, scene);

                // add attributes
                group->addSingularItem(m_outIntensity, m_outIntensity.createAttributeInstance(pcir, imin, imax));

                if (hasColors)
                    group->addSingularItem(m_outRGB, m_outRGB.createAttributeInstance(pcir));

                double hres = 150.0 / (double(nRow));
                double vres = 360.0 / (double(nColumn));

                Eigen::Vector3d scanPos;
                //scanPos(0) = matrix(0,4);
                //scanPos(1) = matrix(1,4);
                //scanPos(2) = matrix(2,4);
                // FORCE SCANNER DEFINITION TO THE RIGHT POSITION
                if(m_applyTransformation) { // the position is the fourth line of the transformation matrix
                  scanPos(0) = matrix(3,0);
                  scanPos(1) = matrix(3,1);
                  scanPos(2) = matrix(3,2);
                } else { // centered on 0
                    scanPos(0) = 0;
                    scanPos(1) = 0;
                    scanPos(2) = 0;
                }

                // add the scanner
                group->addSingularItem(m_outScanner, new CT_Scanner(0, scanPos, Eigen::Vector3d(0,1,0), 360, 150, hres, vres, 0, 0, true, false));

                // add the matrix
                group->addSingularItem(m_outMatrix, new CT_TransformationMatrix(matrix));
            }

            f.close();

            return true;
        }
    }

    return false;
}

