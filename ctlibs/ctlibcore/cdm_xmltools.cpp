#include "cdm_xmltools.h"

QMap<QString, QString> CDM_XMLTools::mapEncodeEntities = CDM_XMLTools::staticInitMapEncodeEntities();
QMap<QString, QString> CDM_XMLTools::mapDecodeEntities = CDM_XMLTools::staticInitMapDecodeEntities();

QString CDM_XMLTools::encodeDecodeEntities(const QMap<QString,QString> &map, const QString &strToCompute){
    QString str = strToCompute;

    QMapIterator<QString, QString> it(map);

    while(it.hasNext()) {
        it.next();
        str.replace(it.key(), it.value());
    }

    return str;
}

QString CDM_XMLTools::encodeEntities(const QString &strToCompute)
{
    return encodeDecodeEntities(CDM_XMLTools::mapEncodeEntities, strToCompute);
}

QString CDM_XMLTools::decodeEntities(const QString &strToCompute)
{
    return encodeDecodeEntities(CDM_XMLTools::mapDecodeEntities, strToCompute);
}

QMap<QString, QString> CDM_XMLTools::staticInitMapEncodeEntities()
{
    QMap<QString, QString> mapEncode;

    mapEncode["À"] = "&#192;";
    mapEncode["à"] = "&#224;";
    mapEncode["Á"] = "&#193;";
    mapEncode["á"] = "&#225;";
    mapEncode["Â"] = "&#194;";
    mapEncode["â"] = "&#226;";
    mapEncode["Ã"] = "&#195;";
    mapEncode["ã"] = "&#227;";
    mapEncode["Ä"] = "&#196;";
    mapEncode["ä"] = "&#228;";
    mapEncode["Å"] = "&#197;";
    mapEncode["å"] = "&#229;";
    mapEncode["Æ"] = "&#198;";
    mapEncode["æ"] = "&#230;";
    mapEncode["Ç"] = "&#199;";
    mapEncode["ç"] = "&#231;";
    mapEncode["Ð"] = "&#208;";
    mapEncode["ð"] = "&#240;";
    mapEncode["È"] = "&#200;";
    mapEncode["è"] = "&#232;";
    mapEncode["É"] = "&#201;";
    mapEncode["é"] = "&#233;";
    mapEncode["Ê"] = "&#202;";
    mapEncode["ê"] = "&#234;";
    mapEncode["Ë"] = "&#203;";
    mapEncode["ë"] = "&#235;";
    mapEncode["Ì"] = "&#204;";
    mapEncode["ì"] = "&#236;";
    mapEncode["Í"] = "&#205;";
    mapEncode["í"] = "&#237;";
    mapEncode["Î"] = "&#206;";
    mapEncode["î"] = "&#238;";
    mapEncode["Ï"] = "&#207;";
    mapEncode["ï"] = "&#239;";
    mapEncode["Ñ"] = "&#209;";
    mapEncode["ñ"] = "&#241;";
    mapEncode["Ò"] = "&#210;";
    mapEncode["ò"] = "&#242;";
    mapEncode["Ó"] = "&#211;";
    mapEncode["ó"] = "&#243;";
    mapEncode["Ô"] = "&#212;";
    mapEncode["ô"] = "&#244;";
    mapEncode["Õ"] = "&#213;";
    mapEncode["õ"] = "&#245;";
    mapEncode["Ö"] = "&#214;";
    mapEncode["ö"] = "&#246;";
    mapEncode["Ø"] = "&#216;";
    mapEncode["ø"] = "&#248;";
    mapEncode["Œ"] = "&#152;";
    mapEncode["œ"] = "&#156;";
    mapEncode["ß"] = "&#223;";
    mapEncode["Þ"] = "&#222;";
    mapEncode["þ"] = "&#254;";
    mapEncode["Ù"] = "&#217;";
    mapEncode["ù"] = "&#249;";
    mapEncode["Ú"] = "&#218;";
    mapEncode["ú"] = "&#250;";
    mapEncode["Û"] = "&#219;";
    mapEncode["û"] = "&#251;";
    mapEncode["Ü"] = "&#220;";
    mapEncode["ü"] = "&#252;";
    mapEncode["Ý"] = "&#221;";
    mapEncode["ý"] = "&#253;";
    mapEncode["Ÿ"] = "&#178;";
    mapEncode["ÿ"] = "&#255;";
    mapEncode["<"] = "&#60;";
    mapEncode[">"] = "&#62;";
    mapEncode["!"] = "&#33;";
    mapEncode["\""] = "&#34;";
    mapEncode["#"] = "&#35;";
    mapEncode["$"] = "&#36;";
    mapEncode["%"] = "&#37;";
    mapEncode["&"] = "&#38;";
    mapEncode["'"] = "&#39;";
    mapEncode["("] = "&#40;";
    mapEncode[")"] = "&#41;";
    mapEncode["<"] = "&#60;";
    mapEncode[">"] = "&#62;";
    mapEncode["/"] = "&#47;";

    return mapEncode;
}

QMap<QString, QString> CDM_XMLTools::staticInitMapDecodeEntities()
{
    QMap<QString, QString> mapTmp = staticInitMapEncodeEntities();
    QMap<QString, QString> mapDecode;


    QMapIterator<QString, QString> it(mapTmp);

    while(it.hasNext()) {
        it.next();
        mapDecode.insert(it.value(), it.key());
    }

    return mapDecode;
}
