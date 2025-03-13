#ifndef LVOX3_UTILS_H
#define LVOX3_UTILS_H

#include "tools/lvox3_gridtype.h"
#include <QString>

#define DEF_SearchInResult      "r"
#define DEF_SearchInGrid        "grid"
#define DEF_SearchInGroup       "gr"
#define DEF_groupOut_grids "grids"
#define DEF_groupOut_gridsMerged "grids merged"
#define DEF_Nt "theoretical"
#define DEF_Nb "before"
#define DEF_Ni "hits"
#define DEF_Density "density"
#define DEF_LVOX_GRD_NT "LVOX_GRD_NT"
#define DEF_LVOX_GRD_NTA "LVOX_GRD_NTA"
#define DEF_LVOX_GRD_NI "LVOX_GRD_NI"
#define DEF_LVOX_GRD_NB "LVOX_GRD_NB"
#define DEF_LVOX_GRD_DENSITY "LVOX_GRD_DENSITY"

class CT_InResultModelGroupToCopy;

enum LVOXGridId {
    Theoretic = 0,
    Hits,
    Before,
    Density
};

struct LVOXGridDefinitionStruct {
    LVOXGridId id;
    QString name;
    QString type;
    QString disp;
    QString desc;
    QString attr;
    QString attr_label;
};

struct LVOXGridSet {
    lvox::Grid3Di* ni;  // hits
    lvox::Grid3Di* nt;  // theoretic
    lvox::Grid3Di* nb;  // before
    lvox::Grid3Df* rd;  // relative density
};

class LVOX3_Utils
{
public:
    LVOX3_Utils();

    //static void requireGrid(CT_InResultModelGroupToCopy *model, LVOXGridId id);

    static const LVOXGridDefinitionStruct LVOXGridDefinitions[];

};

#endif // LVOX3_UTILS_H
