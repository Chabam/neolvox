#include "cdm_configfile.h"

#include <QCoreApplication>
#include <QDir>

CDM_ConfigFile* CDM_ConfigFile::_uniqueInstance = nullptr;

CDM_ConfigFile* CDM_ConfigFile::createInstance()
{
    if(_uniqueInstance == nullptr)
    {
        QString path = QCoreApplication::applicationDirPath();
        int n;

        if((n = path.lastIndexOf("/debug", -1, Qt::CaseInsensitive)) >= 0)
        {
            path = path.left(n);
        }


#if defined(__linux__) // Linux

        QString currentDir = QDir::homePath() + "/.computree/config";
        QDir dir(currentDir);
        if (!dir.exists()) {dir.mkpath(".");}

        _uniqueInstance = new CDM_ConfigFile(QDir::homePath() + "/.computree/config/config.ini", QSettings::IniFormat);
#else

        QString currentDir = QCoreApplication::applicationDirPath() + "/config";
        QDir dir(currentDir);
        if (!dir.exists()) {dir.mkpath(".");}

        _uniqueInstance = new CDM_ConfigFile(QCoreApplication::applicationDirPath() + "/config/config.ini", QSettings::IniFormat);
#endif
    }

    return _uniqueInstance;
}

void CDM_ConfigFile::setColorValue(const QString &key, const QColor &value)
{
    setValue(key, QString("%1,%2,%3,%4").arg(value.red()).arg(value.green()).arg(value.blue()).arg(value.alpha()));
}

QColor CDM_ConfigFile::colorValue(const QString &key, const QColor &defaultValue) const
{
    QColor color = defaultValue;

    QString stringColor = value(key, QString("")).toString();
    QStringList list = stringColor.split(",");

    if(list.size() >= 3)
    {
        int alpha = 255;

        if(list.size() >= 4)
        {
            alpha = list.at(3).toInt();
        }

        color.setRgb(list.at(0).toInt(), list.at(1).toInt(), list.at(2).toInt(), alpha);
    }

    return color;
}

CDM_ConfigFile::CDM_ConfigFile(const QString &fileName, QSettings::Format format) : QSettings(fileName, format)
{
}


