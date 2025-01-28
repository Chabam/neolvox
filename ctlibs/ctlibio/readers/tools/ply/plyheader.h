#ifndef PLYHEADER_H
#define PLYHEADER_H

#include <QList>
#include <QString>

#include "readers/tools/ply/internal/plyelement.h"

/**
 * @brief Represent the header of the ply file
 */
class PlyHeader
{
public:

    /**
     * @brief Construct an invalid header
     */
    PlyHeader();

    /**
     * @brief Construct an header
     * @param filepath : absolute filepath of the file
     * @param format : file format
     * @param formatVersion : version of the file format
     */
    PlyHeader(const QString& filepath,
              const PlyFormatType& format,
              const float& formatVersion);

    /**
     * @brief Returns true if this header is valid
     */
    bool isValid() const;

    /**
     * @brief Set the format of the file
     */
    void setFileFormat(const PlyFormatType& format);

    /**
     * @brief Set the version of the file format
     */
    void setFileFormatVersion(const float& formatVersion);

    /**
     * @brief Set the header end position (returns of "QFile::pos" method)
     * @param pos : value to pass to "QFile::seek" if you want to jump the header
     */
    void setEndPositionInFile(const qint64& pos);

    /**
     * @brief Returns the value to pass to "QFile::seek" if you want to jump the header
     */
    qint64 getEndPositionInFile() const;

    /**
     * @brief Returns the absolute filepath
     */
    QString getFilePath() const;

    /**
     * @brief Returns the format of the file
     */
    PlyFormatType getFileFormat() const;

    /**
     * @brief Returns the format of the file
     */
    QString getDisplayableFileFormat() const;

    /**
     * @brief Returns the version of the file format
     */
    float getFileFormatVersion() const;

    /**
     * @brief Add a new comment
     */
    void addComment(const QString& comment);

    /**
     * @brief Add a new element
     */
    void addElement(const PlyElement& el);

    /**
     * @brief Returns the list of elements (in the order they were added)
     */
    QList<PlyElement> getElements() const;

private:
    QString             m_filepath;
    PlyFormatType       m_formatType;
    float               m_formatVersion;
    QStringList         m_comments;
    QList<PlyElement>   m_elements;
    qint64              m_endPos;
};

#endif // PLYHEADER_H
