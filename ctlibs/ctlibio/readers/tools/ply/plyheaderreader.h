#ifndef PLYHEADERREADER_H
#define PLYHEADERREADER_H

#include "plyheader.h"

class QTextStream;

/**
 * @brief Reader for an header of a ply file
 */
class PlyHeaderReader
{
public:
    PlyHeaderReader();

    /**
     * @brief Set the filepath of the file to read
     */
    void setFilePath(const QString& filepath);

    /**
     * @brief Read the file and construct the header
     * @return false if it was an error when reading
     */
    bool process();

    /**
     * @brief Returns the header of the file
     */
    PlyHeader getHeader() const;

    /**
     * @brief Find the magic word in the file
     * @param stream : stream to use to read the file
     * @return true if it was founded
     */
    bool readMagicWord(QTextStream& stream);

    /**
     * @brief Read the format and create the empty header
     * @param stream : stream to use to read the file
     * @return The empty header (valid or not)
     */
    PlyHeader readFormatAndCreateEmptyHeader(QTextStream& stream, const QString& filepath);

    /**
     * @brief Check the type of the next object (comment, element, etc...) to read, read it and add it to the header
     * @param stream :  stream to use to read the file
     * @param header : header to modify
     * @return 0 if it was no other object to read and end of header founded
     *         -1 if it was no other object to read but end of header was not found
     *         -2 if an element was not properly readed
     *         1 if we must continue to read the file
     */
    int readNextObjectAndAddToHeader(QTextStream& stream, PlyHeader& header, QString& lastLineReaded);

    /**
     * @brief Read an element
     * @param stream : stream to use to read the file
     * @param elementLine : the line that contains element info previously readed (element XXX N)
     * @param ok : set to true if an element has been readed
     * @return The element readed or an invalid element if (ok == false)
     */
    PlyElement readElement(QTextStream& stream, QString elementLine, QString& lastLineReaded, bool& ok) const;

    /**
     * @brief Read a property
     * @param stream : stream to use to read the file
     * @param propertyLine : the line that contains property info previously readed (property TTT N)
     * @param ok : set to true if a property has been readed
     * @return The property readed or an invalid element if (ok == false)
     */
    PlyProperty readProperty(QTextStream& stream, QString propertyLine, QString& lastLineReaded, bool& ok) const;

    /**
     * @brief Returns true if the line declare the end of the header
     * @param line : line to check
     * @return true if it was the end otherwise return false
     */
    bool isEndOfHeader(const QString& line) const;

private:
    QString     m_filePath;
    PlyHeader   m_header;

    /**
     * @brief Find the file format and return the line that contains it
     * @param stream : stream to use to read the file
     * @param ok : set to true if the format was founded
     * @return The format readed or an invalid element if (ok == false)
     */
    QString findFileFormat(QTextStream &stream, bool& ok) const;
};

#endif // PLYHEADERREADER_H
