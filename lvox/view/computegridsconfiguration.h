#ifndef COMPUTEGRIDSCONFIGURATION_H
#define COMPUTEGRIDSCONFIGURATION_H

#include <QObject>
#include <QWidget>
#include <QDialog>

#include <Eigen/Core>

#include "ct_view/ct_abstractconfigurablewidget.h"
#include "ui_computegridsconfiguration.h"

#include "tools/lvox3_gridmode.h"

class ComputeGridsConfiguration : public CT_AbstractConfigurableWidget
{
    Q_OBJECT
public:
    using GridMode = lvox::GridMode;
    enum VegetationType {
        NeedleFromDimension = 0,
        BroadLeafFromDimension,
        NeedleFromFlatArea,
        BroadLeafFromFlatArea
    };
    ComputeGridsConfiguration(QWidget * parent = nullptr);
    ~ComputeGridsConfiguration();

    bool isSettingsModified() const final;
    bool canAcceptIt(QString *err) const final;

    inline const uint32_t getCurrentGridMode() { return _currentGridMode; }

    Eigen::Vector3d getResolution() const;
    Eigen::Vector3d getCoordinates() const;
    Eigen::Vector3d getDimensiosn() const;
    double getFlatArea() const;
    VegetationType getElementProjectedAreaMethods() const;
    GridMode getGridMode() const;
    bool getComputeDistances() const;
    double getLengthOrExtension1() const;
    double getWidthOrExtension2() const;

    void setCurrentProjectedAreaMethod(VegetationType vegetationType);
    void setCurrentGridMode(GridMode gridMode);
    void setCoordinates(Eigen::Vector3d coordinates);
    void setDimensions(Eigen::Vector3d dimensions);
    void setComputeDistances(bool value);
    void setResolution(Eigen::Vector3d resolution);
    void setFlatArea(double area);
        void setLengthOrExtension1(double value);
    void setWidthOrExtension2(double value);


public slots:
    /**
     * @brief Update the element to match the configuration defined by the user
     * @param err : (optionnal) will not be empty if an error can be displayed and if different of nullptr
     * @return true if update was successfull, otherwise returns false and filled the "err" parameter if it was not nullptr
     */
    bool updateElement(QString *err = nullptr) final;

    void estimationMethodHasChanged(const QString s);
    void referenceMethodHasChanged(const QString s);


private:

    void updateInterface() const;


    Ui::ComputeGridsConfiguration * _ui;

    VegetationType                        _currentVegetationType;
    QMap<VegetationType, QString>         _vegetationTypeCollection;

    GridMode                        _currentGridMode;
    QMap<GridMode, QString>         _gridModesCollection;

};

#endif // COMPUTEGRIDSCONFIGURATION_H
