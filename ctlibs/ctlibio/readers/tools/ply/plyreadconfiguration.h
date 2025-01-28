#ifndef PLYREADCONFIGURATION_H
#define PLYREADCONFIGURATION_H

#include "internal/plyvertexconfiguration.h"
#include "internal/plycolorconfiguration.h"
#include "internal/plynormalconfiguration.h"
#include "internal/plyscalarconfiguration.h"

#include <QList>

struct PlyReadConfiguration {

    PlyVertexConfiguration          vertex;
    QList<PlyColorConfiguration>    colors;
    QList<PlyNormalConfiguration>   normals;
    QList<PlyScalarConfiguration>   scalars;

};

#endif // PLYREADCONFIGURATION_H
