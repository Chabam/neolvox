#ifndef CT_ABSTRACTCONFIGURABLEWIDGET_H
#define CT_ABSTRACTCONFIGURABLEWIDGET_H

#include "ctlibwidget_global.h"

#include <QWidget>

/**
 * @brief A configurable widget is a widget that configure an element
 */
class CTLIBWIDGET_EXPORT CT_AbstractConfigurableWidget : public QWidget
{
    Q_OBJECT

public:
    CT_AbstractConfigurableWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    virtual ~CT_AbstractConfigurableWidget() {}

    /**
     * @brief Returns true if it was no error and "updateElement" method can be called
     * @param err : (optionnal) will not be empty if an error can be displayed and if different of nullptr
     */
    virtual bool canAcceptIt(QString *err = nullptr) const { Q_UNUSED(err) return true; }

    /**
     * @brief Returns true if user has modified something
     */
    virtual bool isSettingsModified() const = 0;

public slots:
    /**
     * @brief Update the element to match the configuration defined by the user
     * @param err : (optionnal) will not be empty if an error can be displayed and if different of nullptr
     * @return true if update was successfull, otherwise returns false and filled the "err" parameter if it was not nullptr
     */
    virtual bool updateElement(QString *err = nullptr) = 0;

    /**
     * @brief Update the ui to match the configuration of the element. Do nothing by default.
     */
    virtual void updateUIFromElement() {}

signals:
    /**
     * @brief Emitted when this widget is shown
     */
    void openned();

protected:

    void showEvent(QShowEvent * event) {
        emit openned();
        QWidget::showEvent(event);
    }
};

#endif // CT_ABSTRACTCONFIGURABLEWIDGET_H
