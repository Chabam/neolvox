#ifndef CT_SAMEHEADERMERGER_LAZ_H
#define CT_SAMEHEADERMERGER_LAZ_H

#include <QString>
#include "ctliblaz/readers/headers/ct_lazheader.h"

/**
 * @brief Merge laz files that have header compatible (same point format / scale / offset). This class use the
 *        method "isHeaderAlmostSimilar" of CT_LAZHeader to know if headers are compatible.
 * @example QStringList filesPath;
 *          filesPath << "0.laz" << "1.laz";
 *
 *          CT_SameHeaderMerger_LAZ merger;
 *          merger.setInputFilesPath(filesPath);
 *          merger.setOutputFilePath("merge.laz");
 *          merger.process();
 *
 *          if(merger.hasError())
 *              qDebug() << merger.errorMessage();
 */
class CTLIBLAZ_EXPORT CT_SameHeaderMerger_LAZ
{
public:
    CT_SameHeaderMerger_LAZ();

    /**
     * @brief Set filepath of all files to merge
     */
    void setInputFilesPath(const QStringList& filesPath);

    /**
     * @brief Set the path of the result file
     */
    void setOutputFilePath(const QString& outFilePath);

    /**
     * @brief Process
     */
    void process();

    /**
     * @brief Returns true if it was an error in process
     */
    bool hasError() const;

    /**
     * @brief Returns the error message
     */
    QString errorMessage() const;

private:
    QStringList m_filesPath;
    QString     m_outFilePath;
    QString     m_error;

    /**
     * @brief Read the header and set the error message if it was an error
     */
    bool readHeaderAndSetError(CT_LAZHeader& header);

    /**
     * @brief Check if headers are compatible and set the error message if not
     */
    bool checkHeadersAndSetError(const CT_LAZHeader& h1, const CT_LAZHeader& h2);
};

#endif // CT_SAMEHEADERMERGER_LAZ_H
