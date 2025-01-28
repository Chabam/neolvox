#ifndef PLYNORMALCONFIGURATION_H
#define PLYNORMALCONFIGURATION_H

struct PlyNormalConfiguration {
    PlyNormalConfiguration() {
        elementIndex = -1;
        nxPropertyIndex = -1;
        nyPropertyIndex = -1;
        nzPropertyIndex = -1;
        ncPropertyIndex = -1;
    }

    int         elementIndex;
    int         nxPropertyIndex;
    int         nyPropertyIndex;
    int         nzPropertyIndex;
    int         ncPropertyIndex;
};
#endif // PLYNORMALCONFIGURATION_H
