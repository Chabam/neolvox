#ifndef GPLYNORMALCONFIGURATION_H
#define GPLYNORMALCONFIGURATION_H

#include <QWidget>
#include "iplygenericconfiguration.h"
#include "readers/tools/ply/internal/plynormalconfiguration.h"

namespace Ui {
class GPlyNormalConfiguration;
}

class GPlyNormalConfiguration : public QWidget, public IPlyGenericConfiguration
{
    Q_OBJECT

public:
    explicit GPlyNormalConfiguration(QWidget *parent = nullptr);
    ~GPlyNormalConfiguration();

    /**
     * @brief Returns true if all channels is set
     */
    bool isValid() const;

    /**
     * @brief Set a configuration
     */
    void setConfiguration(const PlyNormalConfiguration& config);

    /**
     * @brief Returns the configuration
     */
    PlyNormalConfiguration getConfiguration() const;

    QWidget* toWidget() const { return const_cast<GPlyNormalConfiguration*>(this); }

private:
    Ui::GPlyNormalConfiguration *ui;

protected:
    /**
     * @brief Call when the ply element change to reset the ui
     */
    void resetUI();

    /**
     * @brief Returns a score to know if this property is the "xxx" channel or not
     */
    static int staticFindNormalProperty(const PlyProperty& property, const QString &wordToFind);
};

#endif // GPLYNORMALCONFIGURATION_H
