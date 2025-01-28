#include "ct_standardscannerdrawmanager.h"

#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/tools/scanner/ct_thetaphishootingpattern.h"
#include "painterinterface.h"

#include <QtMath>

const QString CT_StandardScannerDrawManager::INDEX_CONFIG_FIELD_OF_VIEW = CT_StandardScannerDrawManager::staticInitConfigFieldOfView();

CT_StandardScannerDrawManager::CT_StandardScannerDrawManager(const QString& drawConfigurationName) :
    SuperClass(drawConfigurationName.isEmpty() ? CT_Scanner::staticName() : drawConfigurationName)
{
}

void CT_StandardScannerDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    const CT_Scanner& item = dynamic_cast<const CT_Scanner&>(itemDrawable);

    SuperClass::draw(view, painter, itemDrawable);

    // Drawing the field of view
    CT_ShootingPattern* pattern = item.shootingPattern();

    if (CT_ThetaPhiShootingPattern* p = dynamic_cast<CT_ThetaPhiShootingPattern*>(pattern)){
        drawFieldOfView(painter, p);
    }else{//If it's a real shooting pattern, I make a placeholder 360 degrees shooting pattern just for visualization
        CT_ThetaPhiShootingPattern pReal(pattern->centerCoordinate(),150,360,0.036,0.036,0,0,Eigen::Vector3d(0,0,1),true,false);
        drawFieldOfView(painter, &pReal);
    }

    // TODO: support other types of shooting pattern
}

CT_ItemDrawableConfiguration CT_StandardScannerDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    // Creating the configuration box for the view of the item drawable
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    // Adding the configuration inherited from the CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager class
    item.addAllConfigurationOf( CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::createDrawConfiguration(drawConfigurationName));

    // Adding lines to this config dialog box
    item.addNewConfiguration(CT_StandardScannerDrawManager::staticInitConfigFieldOfView(), QObject::tr("Field of view scale"), CT_ItemDrawableConfiguration::Double, 1.0 );       // How big will be the field of view

    return item;
}

// PROTECTED //

QString CT_StandardScannerDrawManager::staticInitConfigFieldOfView()
{
    return "SC_FOV";
}

void CT_StandardScannerDrawManager::drawFieldOfView(PainterInterface& painter, CT_ThetaPhiShootingPattern* pattern) const
{
    CT_ThetaPhiShootingPattern* patternSimple = static_cast<CT_ThetaPhiShootingPattern*>(pattern->clone());

    patternSimple->setHRes(qDegreesToRadians(10.));
    patternSimple->setVRes(qDegreesToRadians(10.));

    const size_t n = patternSimple->numberOfShots();

    for (size_t i = 0; i < n; i++) {

        const CT_Shot& shot = patternSimple->shotAt(i);

        double x0 = shot.origin().x();
        double y0 = shot.origin().y();
        double z0 = shot.origin().z();

        Eigen::Vector3d dir = shot.direction();
        dir.normalize();

        Eigen::Vector3d v = dir + shot.origin();
        painter.drawLine(x0, y0, z0, v.x(), v.y(), v.z());
    }

    // Drawing the four major limits of the field of view
    /*
    double midPhi = scanner.getVFov()/2 + scanner.getInitPhi();
    drawLineToPosition(painter, scanner, endTheta, scanner.getInitPhi());
    drawLineToPosition(painter, scanner, endTheta, endPhi);
    drawLineToPosition(painter, scanner, endTheta, midPhi);
    */

    delete patternSimple;
}

void CT_StandardScannerDrawManager::drawLineToPosition(PainterInterface &painter, const CT_Scanner &scanner, double theta, double phi) const
{
    const double scaling = drawConfiguration()->variableValue(INDEX_CONFIG_FIELD_OF_VIEW).toDouble();
    const double sinPhi = sin(phi);

    painter.drawLine(scanner.positionX(),
                     scanner.positionY(),
                     scanner.positionZ(),
                     scanner.positionX() + scaling*sinPhi*cos(theta),
                     scanner.positionY() + scaling*sinPhi*sin(theta),
                     scanner.positionZ() + scaling*cos(phi));
}
