#ifndef CT_COMBOBOX_H
#define CT_COMBOBOX_H

#include "ct_widgetwithvaluereferenceinterface.h"

#include <QComboBox>

class CTLIBWIDGET_EXPORT CT_ComboBox : public QObject, public CT_WidgetWithValueReferenceInterface
{
    Q_OBJECT

private:

    struct ComboBoxData
    {
        QStringList  _valuesList;
        QString     *_value;
    };

public:
    CT_ComboBox(QStringList valuesList, QString &value, QString description = "");

    QString type() const final { return metaObject()->className(); }

    void saveSettings(SettingsWriterInterface& writer) const final;
    bool restoreSettings(SettingsReaderInterface& reader) final;

    QWidget* createWidget(QWidget &parent) final;

    void updateValue() final;

    bool isValueAndWidgetValueDifferent() const final;

    QVariant getValue() const override;

    bool setWidgetValueList(QVariant valList);
    bool setWidgetValue(QVariant val);

public slots:
    void changeValues(QStringList valuesList, QString value);

protected:

    ComboBoxData   _data;
    QComboBox      *_comboBoxCreated;
    QString         _description;

    static QString getStringListAsString(const QStringList &liste);

signals:
    void currentIndexChanged(QString value);

};

#endif // CT_COMBOBOX_H
