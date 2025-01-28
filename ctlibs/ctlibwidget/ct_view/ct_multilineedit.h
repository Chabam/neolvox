#ifndef CT_MULTILINEEDIT_H
#define CT_MULTILINEEDIT_H

#include "ct_lineedit.h"
#include <QTextEdit>

class CTLIBWIDGET_EXPORT CT_MultiLineEdit : public CT_LineEdit
{
    Q_OBJECT

public:
    CT_MultiLineEdit(QString &value, QString description = "");

    QWidget* createWidget(QWidget &parent);

    void updateValue();

    bool isValueAndWidgetValueDifferent() const;

    bool setWidgetValue(QVariant val);

protected:

    QTextEdit      *_multiLineEditCreated;
};

#endif // CT_MULTILINEEDIT_H
