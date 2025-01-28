#include "ct_multilineedit.h"

CT_MultiLineEdit::CT_MultiLineEdit(QString &value, QString description) : CT_LineEdit(value, description)
{
    _multiLineEditCreated = nullptr;
}

QWidget* CT_MultiLineEdit::createWidget(QWidget &parent)
{
    if(_multiLineEditCreated == nullptr)
        _multiLineEditCreated = new QTextEdit(*(_data._value), &parent);

    return _multiLineEditCreated;
}

void CT_MultiLineEdit::updateValue()
{
    *(_data._value) = _multiLineEditCreated->toPlainText();
}

bool CT_MultiLineEdit::isValueAndWidgetValueDifferent() const
{
    return ((*_data._value) != _multiLineEditCreated->toPlainText());
}

bool CT_MultiLineEdit::setWidgetValue(QVariant val)
{
    _multiLineEditCreated->setText(val.toString());

    return true;
}
