#ifndef PLYSCALARCONFIGURATION_H
#define PLYSCALARCONFIGURATION_H

struct PlyScalarConfiguration {
    PlyScalarConfiguration() {
        elementIndex = -1;
        sPropertyIndex = -1;
    }

    int         elementIndex;
    int         sPropertyIndex;
};

#endif // PLYSCALARCONFIGURATION_H
