#ifndef GPLYSCALARCONFIGURATION_H
#define GPLYSCALARCONFIGURATION_H

#include <QWidget>
#include "iplygenericconfiguration.h"
#include "readers/tools/ply/internal/plyscalarconfiguration.h"

namespace Ui {
class GPlyScalarConfiguration;
}

class GPlyScalarConfiguration : public QWidget, public IPlyGenericConfiguration
{
    Q_OBJECT

public:
    explicit GPlyScalarConfiguration(QWidget *parent = nullptr);
    ~GPlyScalarConfiguration();

    /**
     * @brief Returns true if all channels is set
     */
    bool isValid() const;

    /**
     * @brief Set a configuration
     */
    void setConfiguration(const PlyScalarConfiguration& config);

    /**
     * @brief Returns the configuration
     */
    PlyScalarConfiguration getConfiguration() const;

    QWidget* toWidget() const { return const_cast<GPlyScalarConfiguration*>(this); }

private:
    Ui::GPlyScalarConfiguration *ui;

protected:
    /**
     * @brief Call when the ply element change to reset the ui
     */
    void resetUI();
};

#endif // GPLYSCALARCONFIGURATION_H
