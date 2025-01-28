#ifndef LOADFILECONFIGUTIL_H
#define LOADFILECONFIGUTIL_H

#include <QtCore>
#include <QString>
#include <QTextStream>
#include <QList>

#include <Eigen/Core>

#include "loadfileconfiguration.h"

#include "lvox_sharedfortest.h"

struct LvoxConfigError {
    LvoxConfigError(const QString msg) : m_msg(msg) {}
    QString m_msg;
};

typedef LoadFileConfiguration::Configuration ConfItem;

/**
 * @brief The LoadFileConfigUtil class: parses and load .in files for backward
 * compatibility. Each line has a point file and a spherical scanner
 * configuration. The fields of a line are separated by spaces.
 */
class LoadFileConfigUtil
{
    Q_DECLARE_TR_FUNCTIONS(LoadFileConfigUtil)
public:
    LoadFileConfigUtil();

    static void loadInFile(const QString& path, QList<ConfItem>& conf);
    static void loadInData(QTextStream& data, QList<ConfItem>& conf);
    static bool parseInLine(const QString& line, ConfItem& item);
    static Eigen::Vector3d loadFileScannerCoordinatesXYB(const QString& filepath);
    static Eigen::Vector3d loadFileScannerCoordinatesPTX(const QString& filepath);

};

#endif // LOADFILECONFIGUTIL_H
