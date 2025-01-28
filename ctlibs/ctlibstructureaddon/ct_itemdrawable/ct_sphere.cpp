#include "ct_sphere.h"

const CT_StandardSphereDrawManager CT_Sphere::SPHERE_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_Sphere)

CT_Sphere::CT_Sphere() : SuperClass()
{
    setBaseDrawManager(&SPHERE_DRAW_MANAGER);
}

CT_Sphere::CT_Sphere(CT_SphereData *data) : SuperClass(data)
{
    setBaseDrawManager(&SPHERE_DRAW_MANAGER);
}

double CT_Sphere::getRadius() const
{
    return ((const CT_SphereData&)getData()).getRadius();
}
