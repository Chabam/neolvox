// #include "laszip/laszip_api.h"

#include "ct_sameheadermerger_laz.h"

#include "laszip/laszip_api.h"

CT_SameHeaderMerger_LAZ::CT_SameHeaderMerger_LAZ()
{
}

void CT_SameHeaderMerger_LAZ::setInputFilesPath(const QStringList &filesPath)
{
    m_filesPath = filesPath;
}

void CT_SameHeaderMerger_LAZ::setOutputFilePath(const QString &outFilePath)
{
    m_outFilePath = outFilePath;
}

void CT_SameHeaderMerger_LAZ::process()
{
    m_error = "";

    if (m_outFilePath.isEmpty()) {
        return;
    }

    const int n = m_filesPath.size();

    if (n > 0) {

        struct InFileInfo {
            InFileInfo(const QString& fp, qint64 nbpts) : filepath(fp), nbPoints(nbpts) {}

            QString filepath;
            qint64  nbPoints;
        };

        QList<InFileInfo> inFilesInfo;

        CT_LAZHeader fHeader;
        fHeader.setFilePath(m_filesPath.first());

        if (!readHeaderAndSetError(fHeader)) {
            return;
        }

        inFilesInfo.append(InFileInfo(m_filesPath.first(), fHeader.m_legacyNumberOfPointRecord));

        for (int i = 1; i < n; ++i) {
            const QString filepath = m_filesPath.at(i);

            CT_LAZHeader header;
            header.setFilePath(filepath);

            if (!readHeaderAndSetError(header) ||
                !checkHeadersAndSetError(fHeader, header)) {
                return;
            }

            fHeader.mergeWith(header);
            inFilesInfo.append(InFileInfo(filepath, header.m_legacyNumberOfPointRecord));
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
            bool ok = fHeader.writezip(zipHeader);
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

        QListIterator<InFileInfo> it(inFilesInfo);

        while (it.hasNext() &&
               m_error.isEmpty()) {
            const InFileInfo& info = it.next();

            // create the reader
            laszip_POINTER laszip_reader = nullptr;
            if (laszip_create(&laszip_reader)) {
                m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(info.filepath);
            }

            // open the reader
            laszip_BOOL is_compressed = 0;
            if (m_error.isEmpty() &&
                laszip_open_reader(laszip_reader, info.filepath.toStdString().c_str(),
                                   &is_compressed)) {
                m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(info.filepath);
            }

            // get a pointer to the points that will be read
            laszip_point* point;
            if (m_error.isEmpty() &&
                laszip_get_point_pointer(laszip_reader, &point)) {
                m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(info.filepath);
            }

            qint64 nbPoints = info.nbPoints;
            qint64 i = 0;
            while (m_error.isEmpty() && i < nbPoints) {

                // read a point
                if (laszip_read_point(laszip_reader)) {
                    m_error = QObject::tr("Impossible de lire le fichier \"%1\".").arg(info.filepath);
                }

                if (m_error.isEmpty()) {
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
    }
}

bool CT_SameHeaderMerger_LAZ::hasError() const
{
    return !m_error.isEmpty();
}

QString CT_SameHeaderMerger_LAZ::errorMessage() const
{
    return m_error;
}

bool CT_SameHeaderMerger_LAZ::readHeaderAndSetError(CT_LAZHeader &header)
{
    QString error;

    if (!header.readzip(header.filePath(), error)) {
        m_error = QObject::tr("Impossible de lire l'en-tête du fichier \"%1\". %2").arg(header.fileInfo().absoluteFilePath()).arg(error);
        return false;
    }

    return true;
}

bool CT_SameHeaderMerger_LAZ::checkHeadersAndSetError(const CT_LAZHeader &h1, const CT_LAZHeader &h2)
{
    if (!h1.isHeaderAlmostSimilar(h2)) {
        m_error = QObject::tr("L'en-tête du fichier \"%1\" n'est pas compatible avec l'en-tête du fichier \"%2\"").arg(((CT_LAZHeader &)h2).fileInfo().absoluteFilePath()).arg(((CT_LAZHeader &)h1).fileInfo().absoluteFilePath());
        return false;
    }

    return true;
}
