#include "pb_xybpiecebypieceprivateexporter.h"

#include <QFileInfo>

#include "ct_itemdrawable/tools/scanner/ct_thetaphishootingpattern.h"

#include "ct_log/ct_logmanager.h"

PB_XYBPieceByPiecePrivateExporter::PB_XYBPieceByPiecePrivateExporter(PB_XYBExporter& exporter, const QString& filePath) :
    mExporter(exporter),
    mFilePath(filePath),
    mStream(nullptr)
{
}

PB_XYBPieceByPiecePrivateExporter::~PB_XYBPieceByPiecePrivateExporter()
{
    if(mFile.isOpen())
        mFile.close();

    delete mStream;
}

bool PB_XYBPieceByPiecePrivateExporter::internalCreateFile()
{
    const QFileInfo exportPathInfo = QFileInfo(mFilePath);
    const QString path = exportPathInfo.path();
    const QString baseName = exportPathInfo.baseName();
    const QString suffix = "xyb";

    const QString currentFilePath = QString("%1/%2.%4").arg(path).arg(baseName).arg(suffix);

    const CT_Scanner* scanner = nullptr;

    if(mExporter.mustUseModels())
    {
        for(const CT_Scanner* scanner : mExporter.m_hInScanner.iterateInputs(mExporter.m_handleResultExport)) {
#if defined(__APPLE__) // Clang Compiler (Apple)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign"
#endif
            scanner = scanner;
#if defined(__APPLE__) // Clang Compiler (Apple)
#pragma GCC diagnostic pop
#endif
            break;
        }
    }
    else
    {
        scanner = mExporter.mScanner;
    }

    CT_ThetaPhiShootingPattern* pattern = nullptr;

    if (scanner != nullptr)
        pattern = dynamic_cast<CT_ThetaPhiShootingPattern*>(scanner->shootingPattern());

    QFile file(currentFilePath);

    if(!file.open(QFile::WriteOnly | QFile::Text))
        return false;

    QTextStream txtStream(&file);

    txtStream << "# SCENE XYZ binary format v1.0\n";

    if(pattern != nullptr)
    {
        double x = pattern->origin().x();
        double y = pattern->origin().y();
        double z = pattern->origin().z();

        txtStream << "ScanPosition " << x << " " << y << " " << z << "\n";
        txtStream << "Rows " << pattern->nVRays() << "\n";
        txtStream << "Cols " << pattern->nHRays() << "\n";
    }
    else
    {
        txtStream << "ScanPosition 0.00000000 0.00000000 0.00000000 \n";
        txtStream << "Rows 0\n";
        txtStream << "Cols 0\n";
    }

    file.close();

    if(!file.open(QFile::Append))
        return false;

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    char d_data[8];

    // write header
    d_data[0] = 0;
    d_data[1] = 0;
    d_data[2] = 0;
    d_data[3] = 0;

    stream.writeRawData(d_data, 4);

    file.close();

    mFile.setFileName(currentFilePath);

    return true;
}

bool PB_XYBPieceByPiecePrivateExporter::internalOpenFileInAppendMode()
{
    if(mFile.fileName().isEmpty())
        return false;

    if(!mFile.open(QFile::Append))
        return false;

    delete mStream;
    mStream = new QDataStream(&mFile);
    mStream->setByteOrder(QDataStream::LittleEndian);

    return true;
}

void PB_XYBPieceByPiecePrivateExporter::exportPoint(const size_t& globalIndex, const CT_Point& point, quint16 intensity)
{
    if(mPointFilter(globalIndex, point))
    {
        (*mStream) << point(CT_Point::X);
        (*mStream) << point(CT_Point::Y);
        (*mStream) << point(CT_Point::Z);
        (*mStream) << intensity;
    }
}

void PB_XYBPieceByPiecePrivateExporter::internalCloseFile()
{
    mFile.close();

    delete mStream;
    mStream = nullptr;
}

bool PB_XYBPieceByPiecePrivateExporter::internalFinalizeFile()
{
    PS_LOG->addInfoMessage(LogInterface::exporter, tr("Export du fichier %1 terminé").arg(mFile.fileName()));
    mFile.setFileName(QString());

    return true;
}
