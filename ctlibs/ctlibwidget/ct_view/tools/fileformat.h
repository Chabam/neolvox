#ifndef FILEFORMAT_H
#define FILEFORMAT_H

#include "ctlibwidget_global.h"

#include <QString>
#include <QList>

/**
 * @brief Represent a format of file for read or export
 */
class CTLIBWIDGET_EXPORT FileFormat
{
public:
    FileFormat(const QList<QString> &suffixes, const QString& description);
    FileFormat(const QString &suffix, QString description);

    /**
     * @brief suffixes of file
     */
    const QList<QString>& suffixes() const;

    /**
     * @brief description
     */
    QString description() const;

private:
    QStringList _suffixes;
    QString     _description;
};

#endif // FILEFORMAT_H
