#include "ct_sameheadermerger_las.h"

CT_SameHeaderMerger_LAS::CT_SameHeaderMerger_LAS()
{
}

void CT_SameHeaderMerger_LAS::setInputFilesPath(const QStringList &filesPath)
{
    m_filesPath = filesPath;
}

void CT_SameHeaderMerger_LAS::setOutputFilePath(const QString &outFilePath)
{
    m_outFilePath = outFilePath;
}

void CT_SameHeaderMerger_LAS::process()
{
    m_error = "";

    if(m_outFilePath.isEmpty())
        return;

    const int n = m_filesPath.size();

    if(n > 0) {

        struct InFileInfo {
            InFileInfo(const QString& fp, qint64 off) : filepath(fp), offset(off) {}

            QString filepath;
            qint64  offset;
        };

        QList<InFileInfo> inFilesInfo;

        CT_LASHeader fHeader;
        fHeader.setFilePath(m_filesPath.first());

        if(!readHeaderAndSetError(fHeader))
            return;

        inFilesInfo.append(InFileInfo(m_filesPath.first(), fHeader.m_offsetToPointData));

        for(int i=1; i<n; ++i) {
            const QString filepath = m_filesPath.at(i);

            CT_LASHeader header;
            header.setFilePath(filepath);

            if(!readHeaderAndSetError(header)
                    || !checkHeadersAndSetError(fHeader, header))
                return;

            fHeader.mergeWith(header);
            inFilesInfo.append(InFileInfo(filepath, header.m_offsetToPointData));
        }

        QFile outF(m_outFilePath);

        if(outF.open(QFile::WriteOnly)) {

            QDataStream stream(&outF);
            stream.setByteOrder(QDataStream::LittleEndian);

            QString error;

            if(fHeader.write(stream, error)) {

                QListIterator<InFileInfo> it(inFilesInfo);

                while(it.hasNext()
                      && m_error.isEmpty()) {
                    const InFileInfo& info = it.next();

                    QFile inFile(info.filepath);

                    if(inFile.open(QFile::ReadOnly)) {
                        const qint64 size = inFile.size()-info.offset;

                        outF.write((char*)inFile.map(info.offset, size), size);
                        inFile.close();
                    } else {
                        m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(info.filepath);
                    }
                }

            } else {
                m_error = QObject::tr("Impossible d'écrire l'en-tête dans le fichier \"%1\". %2").arg(m_outFilePath).arg(error);
            }

            outF.close();

        } else {
            m_error = QObject::tr("Impossible d'ouvrir le fichier \"%1\" en écriture.").arg(m_outFilePath);
        }
    }
}

bool CT_SameHeaderMerger_LAS::hasError() const
{
    return !m_error.isEmpty();
}

QString CT_SameHeaderMerger_LAS::errorMessage() const
{
    return m_error;
}

bool CT_SameHeaderMerger_LAS::readHeaderAndSetError(CT_LASHeader &header)
{
    QFile file(header.fileInfo().absoluteFilePath());

    if(file.open(QFile::ReadOnly)) {
        QDataStream stream(&file);
        stream.setByteOrder(QDataStream::LittleEndian);

        QString error;

        if(!header.read(stream, error)) {
            m_error = QObject::tr("Impossible de lire l'en-tête du fichier \"%1\". %2").arg(header.fileInfo().absoluteFilePath()).arg(error);
            return false;
        }

        file.close();
        return true;
    }

    m_error = QObject::tr("Impossible d'ouvrir le fichier \"%1\"").arg(header.fileInfo().absoluteFilePath());
    return false;
}

bool CT_SameHeaderMerger_LAS::checkHeadersAndSetError(const CT_LASHeader &h1, const CT_LASHeader &h2)
{
    if(!h1.isHeaderAlmostSimilar(h2)) {
        m_error = QObject::tr("L'en-tête du fichier \"%1\" n'est pas compatible avec l'en-tête du fichier \"%2\"").arg(((CT_LASHeader &)h2).fileInfo().absoluteFilePath()).arg(((CT_LASHeader &)h1).fileInfo().absoluteFilePath());
        return false;
    }

    return true;
}
