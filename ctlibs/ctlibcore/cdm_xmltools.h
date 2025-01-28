#ifndef CDM_XMLTOOLS_H
#define CDM_XMLTOOLS_H

#include <QMap>

class CDM_XMLTools
{
public:
    static QString encodeDecodeEntities(const QMap<QString, QString> &map, const QString &strToCompute);
    static QString encodeEntities(const QString &strToCompute);
    static QString decodeEntities(const QString &strToCompute);

private:
    static QMap<QString, QString> mapEncodeEntities;
    static QMap<QString, QString> mapDecodeEntities;

    static QMap<QString, QString> staticInitMapEncodeEntities();
    static QMap<QString, QString> staticInitMapDecodeEntities();
};

#endif // CDM_XMLTOOLS_H
