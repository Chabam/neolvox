#include "fileformat.h"

FileFormat::FileFormat(const QList<QString>& suffixes, const QString& description) :
    _suffixes(suffixes),
    _description(description)
{
}

FileFormat::FileFormat(const QString& suffix, QString description) :
    _description(description)
{
    _suffixes.append(suffix);
}

const QList<QString>& FileFormat::suffixes() const
{
    return _suffixes;
}

QString FileFormat::description() const
{
    return _description;
}
