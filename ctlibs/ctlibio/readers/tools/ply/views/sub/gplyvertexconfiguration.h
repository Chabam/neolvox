#ifndef GPLYVERTEXCONFIGURATION_H
#define GPLYVERTEXCONFIGURATION_H

#include <QWidget>
#include "iplygenericconfiguration.h"
#include "readers/tools/ply/internal/plyvertexconfiguration.h"

namespace Ui {
class GPlyVertexConfiguration;
}

class GPlyVertexConfiguration : public QWidget, public IPlyGenericConfiguration
{
    Q_OBJECT

public:
    explicit GPlyVertexConfiguration(QWidget *parent = nullptr);
    ~GPlyVertexConfiguration();

    /**
     * @brief Returns true if all channels is set
     */
    bool isValid() const;

    /**
     * @brief Set a configuration
     */
    void setConfiguration(const PlyVertexConfiguration& config);

    /**
     * @brief Returns the configuration
     */
    PlyVertexConfiguration getConfiguration() const;

    QWidget* toWidget() const { return const_cast<GPlyVertexConfiguration*>(this); }

private:
    Ui::GPlyVertexConfiguration *ui;

protected:
    /**
     * @brief Call when the ply element change to reset the ui
     */
    void resetUI();
};

#endif // GPLYVERTEXCONFIGURATION_H
