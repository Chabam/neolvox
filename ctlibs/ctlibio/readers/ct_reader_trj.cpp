#include "ct_reader_trj.h"

#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"
#include "ct_log/ct_logmanager.h"

#include <QFile>

#include <limits>
#include <iostream>
#include <fstream>

CT_Reader_TRJ::CT_Reader_TRJ(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewReadableFormat(FileFormat("trj", tr("Trajectoire TerraScan .trj")));
    setToolTip(tr("Chargement de trajectoires à partir du format binaire TerraScan .trj"));
}

CT_Reader_TRJ::CT_Reader_TRJ(const CT_Reader_TRJ& other) : SuperClass(other)
{
}

QString CT_Reader_TRJ::displayableName() const
{
    return tr("Trajectoire TerraScan, Fichier TJR");
}

bool CT_Reader_TRJ::setFilePath(const QString& filepath)
{
    QString error;
    CT_TRJHeader* header = static_cast<CT_TRJHeader*>(internalReadHeader(filepath, error));

    if(!error.isEmpty()) {
        delete header;
        return false;
    }

    if(CT_AbstractReader::setFilePath(filepath)) {
        delete header;
        return true;
    }

    delete header;
    return false;
}

CT_FileHeader* CT_Reader_TRJ::createHeaderPrototype() const
{
    return new CT_TRJHeader();
}


void CT_Reader_TRJ::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);
}

bool CT_Reader_TRJ::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    return true;
}


CT_FileHeader* CT_Reader_TRJ::internalReadHeader(const QString& filepath, QString& error) const
{
    Q_UNUSED(error);

    CT_TRJHeader* header = static_cast<CT_TRJHeader*>(createHeaderPrototype());
    header->setFilePath(filepath);

    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if(f.open(QFile::ReadOnly))
        {
            if (!f.atEnd())
            {
                TrajHdr d_data;
                QDataStream stream(&f);
                stream.setByteOrder(QDataStream::LittleEndian);

                stream.readRawData(d_data.Recog, 8);
                stream >> d_data.Version;
                stream >> d_data.HdrSize;
                stream >> d_data.PosCnt;
                stream >> d_data.PosSize;
                stream.readRawData(d_data.Desc, 78);
                stream >> d_data.SysIdv;
                stream >> d_data.Quality;
                stream >> d_data.BegTime;
                stream >> d_data.EndTime;
                stream >> d_data.OrigNbr;
                stream >> d_data.Number;
                stream.readRawData(d_data.VrtVideo, 400);
                stream >> d_data.VrtBeg;
                stream >> d_data.VrtEnd;
                stream.readRawData(d_data.FwdVideo, 400);
                stream >> d_data.FwdBeg;
                stream >> d_data.FwdEnd;
                stream.readRawData(d_data.WaveFile, 400);
                stream.readRawData(d_data.Group, 16);

                header->setData(QString(d_data.Recog), d_data.Version, d_data.PosCnt, QString(d_data.Desc), d_data.BegTime, d_data.EndTime, d_data.OrigNbr, d_data.Number, QString(d_data.Group), d_data.HdrSize);
            }
            f.close();
        }
    }

    return header;
}

void CT_Reader_TRJ::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_hOutScanPath, tr("Trajectoire"));
}

bool CT_Reader_TRJ::internalReadFile(CT_StandardItemGroup* group)
{
    QString error;
    CT_TRJHeader *header = static_cast<CT_TRJHeader*>(internalReadHeader(filepath(), error));

    if(header == nullptr) {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("Impossible de lire l'en-tête du fichier %1").arg(filepath()));
        return false;
    }

    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if(f.open(QIODevice::ReadOnly))
        {
            TrajPos d_data;
            QDataStream stream(&f);
            stream.setByteOrder(QDataStream::LittleEndian);

            QFileInfo info(filepath());
            CT_ScanPath* scanPath = new CT_ScanPath(info.baseName());

            f.seek(header->getHdrSize());

            while(!stream.atEnd())
            {
                stream >> d_data.Time;
                stream >> d_data.x;
                stream >> d_data.y;
                stream >> d_data.z;
                stream >> d_data.Head;
                stream >> d_data.Roll;
                stream >> d_data.Pitch;
                stream >> d_data.QtyXy;
                stream >> d_data.QtyZ;
                stream >> d_data.QtyH;
                stream >> d_data.QtyRp;
                stream >> d_data.Mark;
                stream >> d_data.Flag;

                scanPath->addPathPoint(d_data.Time, d_data.x, d_data.y, d_data.z, d_data.Head, d_data.Roll, d_data.Pitch);
            }

            group->addSingularItem(m_hOutScanPath, scanPath);

            f.close();

            return true;
        }
    }

    delete header;

    return false;
}
