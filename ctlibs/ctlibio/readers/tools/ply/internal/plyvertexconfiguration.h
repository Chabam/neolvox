#ifndef PLYVERTEXCONFIGURATION_H
#define PLYVERTEXCONFIGURATION_H

struct PlyVertexConfiguration {
    PlyVertexConfiguration() {
        elementIndex = -1;
        xPropertyIndex = -1;
        yPropertyIndex = -1;
        zPropertyIndex = -1;
    }

    int elementIndex;
    int xPropertyIndex;
    int yPropertyIndex;
    int zPropertyIndex;
};

#endif // PLYVERTEXCONFIGURATION_H
