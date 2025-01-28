#ifndef CT_GDALTOOLS_H
#define CT_GDALTOOLS_H

#include "gdal_priv.h"

#include <QString>
#include <QStringList>
#include <QMutableListIterator>

#include "ctlibgdal/ctlibgdal_global.h"

class CTLIBGDAL_EXPORT CT_GdalTools
{
public:
    static QString staticGdalDriverName(GDALDriver *driver)
    {
        if(driver != NULL) {
            QString name = "GDAL " + QString(driver->GetMetadataItem(GDAL_DMD_LONGNAME));
            name.remove(QRegExp("\\(\\..*\\)"));

            return name;
        }

        return "";
    }

    static QStringList staticGdalDriverExtension(GDALDriver *driver)
    {
        if(driver != NULL) {
            QStringList ext = QString(driver->GetMetadataItem(GDAL_DMD_EXTENSION)).split("/");
            QString name = staticGdalDriverName(driver);

            QMutableListIterator<QString> it(ext);

            while(it.hasNext()) {

                if(it.next().trimmed().isEmpty())
                    it.remove();
            }

            if (name == "GDAL AirSAR Polarimetric Image") {ext.append("dat");}
            if (name == "GDAL Arc/Info Binary Grid") {ext.append("adf");}
            if (name == "GDAL Azavea Raster Grid format") {ext.append("json");}
            if (name == "GDAL Maptech BSB Nautical Charts") {ext.append("kap");}
            if (name == "GDAL DTED Elevation Raster") {ext.append("dt0");ext.append("dt1");ext.append("dt2");}
            if (name == "GDAL ENVI .hdr Labelled") {ext.append("hdr");}
            if (name == "GDAL ESRI .hdr Labelled") {ext.append("hdr");ext.append("bil");}
            if (name == "GDAL EarthWatch .TIL") {ext.append("til");}
            if (name == "GDAL ERMapper .ers Labelled") {ext.append("ers");}
            if (name == "GDAL Erdas .LAN/.GIS") {ext.append("lan");ext.append("gis");}
            if (name == "GDAL Erdas Imagine Raw") {ext.append("raw");}
            if (name == "GDAL OziExplorer .MAP") {ext.append("map");}
            if (name == "GDAL PCI .aux Labelled") {ext.append("aux");}
            if (name == "GDAL UK .NTF") {ext.append("ntf");}

            return ext;
        }

        return QStringList();
    }
};


#endif // CT_GDALTOOLS_H
