#include "ct_sameheaderformatmerger_laz.h"

#include "ct_sameheadermerger_laz.h"
#include "readers/ct_reader_lasv2.h"

#include "laszip/laszip_api.h"

#include <QSharedPointer>

CT_SameHeaderFormatMerger_LAZ::CT_SameHeaderFormatMerger_LAZ()
{
}

void CT_SameHeaderFormatMerger_LAZ::setInputFilesPath(const QStringList &filesPath)
{
    m_filesPath = filesPath;
}

void CT_SameHeaderFormatMerger_LAZ::setOutputFilePath(const QString &outFilePath)
{
    m_outFilePath = outFilePath;
}

void CT_SameHeaderFormatMerger_LAZ::process()
{
    m_error = "";

    if (m_outFilePath.isEmpty()) {
        return;
    }

    const int n = m_filesPath.size();

    if (n > 0) {

        struct HeaderInfo {
        public:
            HeaderInfo() {}
            HeaderInfo(const QString& filepath) {
                headerPtr = QSharedPointer<CT_LAZHeader>(new CT_LAZHeader());
                headerPtr->setFilePath(filepath);
                filePathOfFiles.append(filepath);
            }

            bool mergeWith(const HeaderInfo& oInfo) {
                if (!header().isHeaderAlmostSimilar(oInfo.header()))
                    return false;

                header().mergeWith(oInfo.header());
                filePathOfFiles.append(oInfo.filePathOfFiles.first());

                return true;
            }

            CT_LAZHeader& header() const { return *headerPtr.data(); }

            QList<QString>  filePathOfFiles;

        private:
            QSharedPointer<CT_LAZHeader>   headerPtr;
        };

        // List of header merged (merged if same format|version / scale / offset) and not merged (same format|version but not same scale or offset)
        QList<HeaderInfo> filesInfo;

        // Merge header that can be and add other in the list
        HeaderInfo fInfo(m_filesPath.first());

        if (!readHeaderAndSetError(fInfo.header()))
            return;

        filesInfo.append(fInfo);

        for (int i = 1; i < n; ++i) {
            const QString filepath = m_filesPath.at(i);

            HeaderInfo info(filepath);

            if (!readHeaderAndSetError(info.header()) ||
                !checkHeadersAndSetError(fInfo.header(), info.header())) {
                return;
            }

            bool merged = false;

            QMutableListIterator<HeaderInfo> it(filesInfo);

            while (it.hasNext() && !merged) {
                merged = it.next().mergeWith(info);
            }

            if (!merged) {
                filesInfo.append(info);
            }
        }

        // The list now contains one or multiple HeaderInfo
        // if HeaderInfo contains multiple filepath so it can be merged with a simple merger
        // if HeaderInfo contains only one filepath we must check if it can be merged with other in the list
        // that contains only one filepath. If no return false !

        int nFilesInfo = filesInfo.size();

        if (nFilesInfo == 1) {
            CT_SameHeaderMerger_LAZ merger;
            merger.setInputFilesPath(filesInfo.first().filePathOfFiles);
            merger.setOutputFilePath(m_outFilePath);
            merger.process();

            if (merger.hasError()) {
                m_error = merger.errorMessage();
            }

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

        while (it.hasNext()) {
            const HeaderInfo& info = it.next();

            if (info.filePathOfFiles.size() > 1) {

                const QString temporaryOutputFilePath = baseFilePath + "_tmp" + QString().setNum(complexFileIndex++);

                temporaryFiles.append(temporaryOutputFilePath);

                CT_SameHeaderMerger_LAZ merger;
                merger.setInputFilesPath(info.filePathOfFiles);
                merger.setOutputFilePath(temporaryOutputFilePath);
                merger.process();

                if (merger.hasError()) {
                    m_error = merger.errorMessage();
                    deleteTemporaryFiles(temporaryFiles);
                    return;
                }

                HeaderInfo newInfo(temporaryOutputFilePath);

                if (!readHeaderAndSetError(newInfo.header())) {
                    deleteTemporaryFiles(temporaryFiles);
                    return;
                }

                it.setValue(newInfo);
            }
        }

        // Merge complex
        CT_LAZHeader* finalHeaderPtr = (CT_LAZHeader*)filesInfo.first().header().copy(nullptr, nullptr);
        CT_LAZHeader& finalHeader = *finalHeaderPtr;

        for (int i = 1; i < nFilesInfo; ++i) {
            const CT_LAZHeader& oHeader = filesInfo.at(i).header();
            finalHeader.mergeWith(oHeader);
        }

        // create the writer
        laszip_POINTER laszip_writer = nullptr;
        if (laszip_create(&laszip_writer)) {
            m_error = QObject::tr("Impossible d'ouvrir le fichier \"%1\" en écriture.").arg(m_outFilePath);
        }

        laszip_header zipHeader;

        if (m_error.isEmpty()) {
            // initialize the header for the writer
            QString error;
            bool ok = finalHeader.writezip(zipHeader);
            if (!ok) {
                m_error = QObject::tr("Impossible d'écrire l'en-tête dans le fichier \"%1\". %2").arg(m_outFilePath).arg(error);
            }
        }

        if (m_error.isEmpty() &&
            laszip_set_header(laszip_writer, &zipHeader)) {
            QString error;
            m_error = QObject::tr("Impossible d'écrire l'en-tête dans le fichier \"%1\". %2").arg(m_outFilePath).arg(error);
        }

        // open the writer
        laszip_BOOL compress = true;
        if (m_error.isEmpty() &&
            laszip_open_writer(laszip_writer, m_outFilePath.toStdString().c_str(), compress)) {
            m_error = QObject::tr("Impossible d'ouvrir le fichier \"%1\" en écriture.").arg(m_outFilePath);
        }

        QListIterator<HeaderInfo> itInInfo(filesInfo);

        while (itInInfo.hasNext() &&
               m_error.isEmpty()) {
            const HeaderInfo& inInfo = itInInfo.next();

            // create the reader
            laszip_POINTER laszip_reader = nullptr;
            if (laszip_create(&laszip_reader)) {
                m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(inInfo.filePathOfFiles.first());
            }

            // open the reader
            laszip_BOOL is_compressed = 0;
            if (m_error.isEmpty() &&
                laszip_open_reader(laszip_reader, inInfo.filePathOfFiles.first().toStdString().c_str(),
                                   &is_compressed)) {
                m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(inInfo.filePathOfFiles.first());
            }

            // get a pointer to the points that will be read
            laszip_point* point;
            if (m_error.isEmpty() &&
                laszip_get_point_pointer(laszip_reader, &point)) {
                m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(inInfo.filePathOfFiles.first());
            }

            qint32 x, y, z;
            double xc, yc, zc;

            const size_t nbPoints = inInfo.header().getPointsRecordCount();
            size_t i = 0;
            while (m_error.isEmpty() && i < nbPoints) {

                // read a point
                if (laszip_read_point(laszip_reader)) {
                    m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(inInfo.filePathOfFiles.first());
                }

                if (m_error.isEmpty()) {
                    x = point->X;
                    y = point->Y;
                    z = point->Z;

                    inInfo.header().transformPoint(x, y, z, xc, yc, zc);
                    finalHeader.inverseTransformPoint(xc, yc, zc, x, y, z);

                    point->X = x;
                    point->Y = y;
                    point->Z = z;

                    // copy the point
                    laszip_set_point(laszip_writer, point);

                    // write the point
                    laszip_write_point(laszip_writer);
                }

                i++;
            }

            // close the reader
            laszip_close_reader(laszip_reader);

            // destroy the reader
            laszip_destroy(laszip_reader);
        }

        // close the writer
        laszip_close_writer(laszip_writer);

        // destroy the writer
        laszip_destroy(laszip_writer);

        deleteTemporaryFiles(temporaryFiles);
        delete finalHeaderPtr;
    }
}

bool CT_SameHeaderFormatMerger_LAZ::hasError() const
{
    return !m_error.isEmpty();
}

QString CT_SameHeaderFormatMerger_LAZ::errorMessage() const
{
    return m_error;
}

bool CT_SameHeaderFormatMerger_LAZ::readHeaderAndSetError(CT_LAZHeader &header)
{
    QString error;

    if (!header.readzip(header.filePath(), error)) {
        m_error = QObject::tr("Impossible de lire l'en-tête du fichier \"%1\". %2").arg(header.fileInfo().absoluteFilePath()).arg(error);
        return false;
    }

    return true;
}

bool CT_SameHeaderFormatMerger_LAZ::checkHeadersAndSetError(const CT_LAZHeader &h1, const CT_LAZHeader &h2)
{
    if (!h1.isHeaderAlmostSimilar(h2, CT_LAZHeader::CO_Format | CT_LAZHeader::CO_Version)) {
        m_error = QObject::tr("L'en-tête du fichier \"%1\" n'est pas compatible avec l'en-tête du fichier \"%2\"").arg(((CT_LAZHeader &)h2).fileInfo().absoluteFilePath()).arg(((CT_LAZHeader &)h1).fileInfo().absoluteFilePath());
        return false;
    }

    return true;
}

void CT_SameHeaderFormatMerger_LAZ::deleteTemporaryFiles(const QStringList &filePathList)
{
    foreach (const QString& fp, filePathList) {
        QFile::remove(fp);
    }
}
