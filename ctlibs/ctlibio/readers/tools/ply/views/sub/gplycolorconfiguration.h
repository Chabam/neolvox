#ifndef GPLYCOLORCONFIGURATION_H
#define GPLYCOLORCONFIGURATION_H

#include <QWidget>
#include "iplygenericconfiguration.h"
#include "readers/tools/ply/internal/plycolorconfiguration.h"

namespace Ui {
class GPlyColorConfiguration;
}

class GPlyColorConfiguration : public QWidget, public IPlyGenericConfiguration
{
    Q_OBJECT

public:
    explicit GPlyColorConfiguration(QWidget *parent = nullptr);
    ~GPlyColorConfiguration();

    /**
     * @brief Returns true if all channels is set
     */
    bool isValid() const;

    /**
     * @brief Set a configuration
     */
    void setConfiguration(const PlyColorConfiguration& config);

    /**
     * @brief Returns the configuration
     */
    PlyColorConfiguration getConfiguration() const;

    QWidget* toWidget() const { return const_cast<GPlyColorConfiguration*>(this); }

private:
    Ui::GPlyColorConfiguration  *ui;

protected:
    /**
     * @brief Call when the ply element change to reset the ui
     */
    void resetUI();

    /**
     * @brief Returns a score to know if this property is the "xxx" channel or not
     */
    static int staticFindColorProperty(const PlyProperty& property, const QStringList &wordsToFindByBestScore);
};

#endif // GPLYCOLORCONFIGURATION_H
