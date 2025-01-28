#ifndef CT_SAMEHEADERFORMATMERGER_LAS_H
#define CT_SAMEHEADERFORMATMERGER_LAS_H

#include <QString>
#include "ctliblas/readers/headers/ct_lasheader.h"

/**
 * @brief Merge las files that have header with same format but scale / offset different.
 *
 * @example QStringList filesPath;
 *          filesPath << "0.las" << "1.las";
 *
 *          CT_SameHeaderFormatMerger_LAS merger;
 *          merger.setInputFilesPath(filesPath);
 *          merger.setOutputFilePath("merge.las");
 *          merger.process();
 *
 *          if(merger.hasError())
 *              qDebug() << merger.errorMessage();
 */
class CTLIBLAS_EXPORT CT_SameHeaderFormatMerger_LAS
{
public:
    CT_SameHeaderFormatMerger_LAS();

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
    bool readHeaderAndSetError(CT_LASHeader& header);

    /**
     * @brief Check if headers are compatible and set the error message if not
     */
    bool checkHeadersAndSetError(const CT_LASHeader& h1, const CT_LASHeader& h2);

    /**
     * @brief Merge header h with header h
     */
    void mergeHeader(CT_LASHeader& h1, const CT_LASHeader& h2);

    static void deleteTemporaryFiles(const QStringList& filePathList);
};

#endif // CT_SAMEHEADERFORMATMERGER_LAS_H
