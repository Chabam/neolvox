#ifndef PLYCOLORCONFIGURATION_H
#define PLYCOLORCONFIGURATION_H

struct PlyColorConfiguration {
    PlyColorConfiguration() {
        elementIndex = -1;
        rPropertyIndex = -1;
        gPropertyIndex = -1;
        bPropertyIndex = -1;
        aPropertyIndex = -1;
    }

    int         elementIndex;
    int         rPropertyIndex;
    int         gPropertyIndex;
    int         bPropertyIndex;
    int         aPropertyIndex;
};

#endif // PLYCOLORCONFIGURATION_H
