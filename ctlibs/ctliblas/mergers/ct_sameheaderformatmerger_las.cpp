#include "ct_sameheaderformatmerger_las.h"

#include "ct_sameheadermerger_las.h"
#include "readers/ct_reader_lasv2.h"

#include <QSharedPointer>

CT_SameHeaderFormatMerger_LAS::CT_SameHeaderFormatMerger_LAS()
{
}

void CT_SameHeaderFormatMerger_LAS::setInputFilesPath(const QStringList &filesPath)
{
    m_filesPath = filesPath;
}

void CT_SameHeaderFormatMerger_LAS::setOutputFilePath(const QString &outFilePath)
{
    m_outFilePath = outFilePath;
}

void CT_SameHeaderFormatMerger_LAS::process()
{
    m_error = "";

    if(m_outFilePath.isEmpty())
        return;

    const int n = m_filesPath.size();

    if(n > 0) {

        struct HeaderInfo {
        public:
            HeaderInfo() {}
            HeaderInfo(const QString& filepath) {
                headerPtr = QSharedPointer<CT_LASHeader>(new CT_LASHeader());
                headerPtr->setFilePath(filepath);
                filePathOfFiles.append(filepath);
            }

            bool mergeWith(const HeaderInfo& oInfo) {
                if(!header().isHeaderAlmostSimilar(oInfo.header()))
                    return false;

                header().mergeWith(oInfo.header());
                filePathOfFiles.append(oInfo.filePathOfFiles.first());

                return true;
            }

            CT_LASHeader& header() const { return *headerPtr.data(); }

            QList<QString>  filePathOfFiles;

        private:
            QSharedPointer<CT_LASHeader>   headerPtr;
        };

        // List of header merged (merged if same format|version / scale / offset) and not merged (same format|version but not same scale or offset)
        QList<HeaderInfo> filesInfo;

        // Merge header that can be and add other in the list
        HeaderInfo fInfo(m_filesPath.first());

        if(!readHeaderAndSetError(fInfo.header()))
            return;

        filesInfo.append(fInfo);

        for(int i=1; i<n; ++i) {
            const QString filepath = m_filesPath.at(i);

            HeaderInfo info(filepath);

            if(!readHeaderAndSetError(info.header())
                    || !checkHeadersAndSetError(fInfo.header(), info.header()))
                return;

            bool merged = false;

            QMutableListIterator<HeaderInfo> it(filesInfo);

            while(it.hasNext() && !merged) {
                merged = it.next().mergeWith(info);
            }

            if(!merged)
                filesInfo.append(info);
        }

        // The list now contains one or multiple HeaderInfo
        // if HeaderInfo contains multiple filepath so it can be merged with a simple merger
        // if HeaderInfo contains only one filepath we must check if it can be merged with other in the list
        // that contains only one filepath. If no return false !

        int nFilesInfo = filesInfo.size();

        if(nFilesInfo == 1) {
            CT_SameHeaderMerger_LAS merger;
            merger.setInputFilesPath(filesInfo.first().filePathOfFiles);
            merger.setOutputFilePath(m_outFilePath);
            merger.process();

            if(merger.hasError())
                m_error = merger.errorMessage();

            return;
        }

        QStringList temporaryFiles;

        const QFileInfo fileInfo(m_outFilePath);
        const QString suffix = fileInfo.suffix();
        QString baseFilePath = m_outFilePath;
        baseFilePath.remove(baseFilePath.size()-suffix.size()-1, suffix.size()+1);

        int complexFileIndex = 0;

        // Merge simple
        QMutableListIterator<HeaderInfo> it(filesInfo);

        while(it.hasNext()) {
            const HeaderInfo& info = it.next();

            if(info.filePathOfFiles.size() > 1) {

                const QString temporaryOutputFilePath = baseFilePath + "_tmp" + QString().setNum(complexFileIndex++);

                temporaryFiles.append(temporaryOutputFilePath);

                CT_SameHeaderMerger_LAS merger;
                merger.setInputFilesPath(info.filePathOfFiles);
                merger.setOutputFilePath(temporaryOutputFilePath);
                merger.process();

                if(merger.hasError()) {
                    m_error = merger.errorMessage();
                    deleteTemporaryFiles(temporaryFiles);
                    return;
                }

                HeaderInfo newInfo(temporaryOutputFilePath);

                if(!readHeaderAndSetError(newInfo.header())) {
                    deleteTemporaryFiles(temporaryFiles);
                    return;
                }

                it.setValue(newInfo);
            }
        }

        // Merge complex
        CT_LASHeader* finalHeaderPtr = (CT_LASHeader*)filesInfo.first().header().copy(nullptr, nullptr);
        CT_LASHeader& finalHeader = *finalHeaderPtr;

        for(int i=1; i<nFilesInfo; ++i) {
            const CT_LASHeader& oHeader = filesInfo.at(i).header();
            finalHeader.mergeWith(oHeader);
        }

        // Read all files and use the final header to write it
        QFile outF(m_outFilePath);

        if(outF.open(QFile::WriteOnly)) {
            QDataStream outStream(&outF);
            outStream.setByteOrder(QDataStream::LittleEndian);

            QString error;

            if(finalHeader.write(outStream, error)) {

                QListIterator<HeaderInfo> it(filesInfo);

                while(it.hasNext()
                      && m_error.isEmpty()) {
                    const HeaderInfo& inInfo = it.next();

                    QFile inFile(inInfo.filePathOfFiles.first());

                    if(inFile.open(QFile::ReadOnly))
                    {
                        const qint64 size = inFile.size()-inInfo.header().m_offsetToPointData;

                        // if this header has the same scale and the same offset than the final header
                        if(finalHeader.isHeaderAlmostSimilar(inInfo.header(), CT_LASHeader::CO_Offset | CT_LASHeader::CO_Scale))
                        {
                            // we just have to copy data from this file to output file
                            outF.write((char*)inFile.map(inInfo.header().m_offsetToPointData, size), size);

                        } else {
                            inFile.seek(inInfo.header().m_offsetToPointData);

                            QDataStream inStream(&inFile);
                            inStream.setByteOrder(QDataStream::LittleEndian);

                            // we must read each point, convert it to be compatible with the new header and write it
                            const size_t nPoints = inInfo.header().getPointsRecordCount();

                            std::vector<char> datas;

                            qint32 x, y, z;
                            double xc, yc, zc;

                            for(size_t i=0; i<nPoints; ++i)
                            {
                                inStream >> x >> y >> z;
                                datas.resize(inInfo.header().get_pointDataRecordLength() - 12); // 12 = (3 * qint32) [x, y, z]
                                inStream.readRawData(&datas[0], int(datas.size()));

                                inInfo.header().transformPoint(x, y, z, xc, yc, zc);

                                finalHeader.inverseTransformPoint(xc, yc, zc, x, y, z);

                                outStream << x << y << z;

                                if(!datas.empty())
                                    outStream.writeRawData(&datas[0], int(datas.size()));
                            }
                        }

                        inFile.close();
                    }
                    else
                    {
                        m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(inInfo.filePathOfFiles.first());
                    }
                }

            } else {
                m_error = QObject::tr("Impossible d'écrire l'en-tête dans le fichier \"%1\". %2").arg(m_outFilePath).arg(error);
            }

            outF.close();

        } else {
            m_error = QObject::tr("Impossible d'ouvrir le fichier \"%1\" en écriture.").arg(m_outFilePath);
        }

        deleteTemporaryFiles(temporaryFiles);
        delete finalHeaderPtr;
    }
}

bool CT_SameHeaderFormatMerger_LAS::hasError() const
{
    return !m_error.isEmpty();
}

QString CT_SameHeaderFormatMerger_LAS::errorMessage() const
{
    return m_error;
}

bool CT_SameHeaderFormatMerger_LAS::readHeaderAndSetError(CT_LASHeader &header)
{
    QFile file(header.fileInfo().absoluteFilePath());

    if(file.open(QFile::ReadOnly)) {
        QDataStream stream(&file);
        stream.setByteOrder(QDataStream::LittleEndian);

        QString error;

        if(!header.read(stream, error)) {
            m_error = QObject::tr("(Impossible de lire l'en-tête du fichier \"%1\". %2").arg(header.fileInfo().absoluteFilePath()).arg(error);
            return false;
        }

        file.close();
        return true;
    }

    m_error = QObject::tr("Impossible d'ouvrir le fichier \"%1\"").arg(header.fileInfo().absoluteFilePath());
    return false;
}

bool CT_SameHeaderFormatMerger_LAS::checkHeadersAndSetError(const CT_LASHeader &h1, const CT_LASHeader &h2)
{
    if(!h1.isHeaderAlmostSimilar(h2, CT_LASHeader::CO_Format | CT_LASHeader::CO_Version)) {
        m_error = QObject::tr("L'en-tête du fichier \"%1\" n'est pas compatible avec l'en-tête du fichier \"%2\"").arg(((CT_LASHeader &)h2).fileInfo().absoluteFilePath()).arg(((CT_LASHeader &)h1).fileInfo().absoluteFilePath());
        return false;
    }

    return true;
}

void CT_SameHeaderFormatMerger_LAS::deleteTemporaryFiles(const QStringList &filePathList)
{
    foreach (const QString& fp, filePathList) {
        QFile::remove(fp);
    }
}
