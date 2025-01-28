#include "ct_genericconfigurablewidget.h"

#include "ct_spinbox.h"
#include "ct_doublespinbox.h"
#include "ct_checkbox.h"
#include "ct_asciifilechoicebutton.h"
#include "ct_combobox.h"
#include "ct_radiobutton.h"
#include "ct_buttongroup.h"
#include "ct_lineedit.h"
#include "ct_multilineedit.h"
#include "ct_groupbox.h"

#include <QFrame>
#include <QScrollArea>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

CT_GenericConfigurableWidget::CT_GenericConfigurableWidget(QWidget *parent) : CT_GenericConfigurableWidget(true, parent)
{

}

CT_GenericConfigurableWidget::CT_GenericConfigurableWidget(bool useScrollArea, QWidget *parent) : CT_AbstractConfigurableWidget(parent)
{
    setWindowTitle(tr("Configuration"));

    m_gridLayout = new QGridLayout();

    m_centralWidget = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(m_centralWidget);
    lay->addLayout(m_gridLayout);

    lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    if(useScrollArea) {
        QScrollArea* sc = new QScrollArea();
        sc->setStyleSheet("QScrollArea { border: 0px solid transparent; }");
        sc->setWidgetResizable(true);
        sc->setWidget(m_centralWidget);
        lay->addWidget(sc);
    } else {
        lay->addWidget(m_centralWidget);
    }

    m_canEdit = true;
    m_nRow = 0;
}

CT_GenericConfigurableWidget::~CT_GenericConfigurableWidget()
{
    qDeleteAll(m_ctWidget.begin(), m_ctWidget.end());
}

bool CT_GenericConfigurableWidget::isEmpty() const
{
    return m_ctWidget.isEmpty() && m_centralWidget->children().size();
}

CT_GroupBox* CT_GenericConfigurableWidget::addNewGroupBox(const QString &title)
{
    if(canEdit()) {
        CT_GroupBox* gpw = new CT_GroupBox(title);
        m_ctWidget.append(gpw);
        m_groupBoxCollection.append(gpw);

        m_centralWidget->layout()->addWidget(gpw->createWidget(*m_centralWidget));

        return gpw;
    }

    return nullptr;
}

bool CT_GenericConfigurableWidget::addText(const QString &firstColumnLabelText,
                                           const QString &secondColumnLabelText,
                                           const QString &thirdColumnLabelText,
                                           const QString &description)
{
    if(canEdit())
    {
        addLabel(nRows(), 0, firstColumnLabelText, 1, (secondColumnLabelText.isEmpty() ? (thirdColumnLabelText.isEmpty() ? 3 : 2) : 1));
        addLabel(nRows(), 1, secondColumnLabelText, 1, (thirdColumnLabelText.isEmpty() ? 2 : 1));
        addLabel(nRows(), 2, thirdColumnLabelText);
        addDescription(nRows(), description);

        addRow();

        m_widgetList.append(nullptr);
        m_paramLab.append("<br>" + firstColumnLabelText + "<br>");
        m_paramDesc.append("");

        return true;
    }

    return false;
}

void CT_GenericConfigurableWidget::addSeparationLine()
{
    if(canEdit())
    {
        addLabel(nRows(), 0, "");
        addRow();

        QFrame* line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        addWidget(nRows(), 0, line, 1, 4);
        addRow();

        m_widgetList.append(nullptr);
        m_paramLab.append("<br>");
        m_paramDesc.append("");

    }
}

bool CT_GenericConfigurableWidget::addTitle(const QString &text,
                                            const QString &description)
{
    return addText(text, "", "", description);
}

CT_SpinBox* CT_GenericConfigurableWidget::addInt(const QString &beforeLabelText,
                                          const QString &afterLabelText,
                                          const int &minValue,
                                          const int &maxValue,
                                          int &valueToUpdate,
                                          const QString &description)
{
    if(canEdit())
    {
        CT_SpinBox *spinBox = new CT_SpinBox(minValue, maxValue, valueToUpdate, beforeLabelText);
        m_ctWidget.append(spinBox);

        addLabel(nRows(), 0, beforeLabelText);
        addWidget(nRows(), 1, spinBox->createWidget(*this));
        addLabel(nRows(), 2, afterLabelText);
        addDescription(nRows(), description);

        addRow();

        QString sep = "";
        if (!beforeLabelText.isEmpty())
        {
            if (beforeLabelText.back() == ":" || beforeLabelText.back() == "."  || beforeLabelText.back() == ",") {sep = " ";}
            else if (beforeLabelText.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(spinBox);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + beforeLabelText + "</span>" + sep + tr(" <em>__VALUE__%1</em>. __DESCRIPTION__</li><br>").arg((afterLabelText.isEmpty()?"":" ") + afterLabelText));
        m_paramDesc.append("<br>" + description);

        return spinBox;
    }

    return nullptr;
}

CT_DoubleSpinBox* CT_GenericConfigurableWidget::addDouble(const QString &beforeLabelText,
                                             const QString &afterLabelText,
                                             const double &minValue,
                                             const double &maxValue,
                                             const int &nDecimals,
                                             double &valueToUpdate,
                                             const double &multValue,
                                             const QString &description)
{

    if(canEdit())
    {
        CT_DoubleSpinBox *spinBox = new CT_DoubleSpinBox(minValue, maxValue, nDecimals, valueToUpdate, multValue, beforeLabelText);
        m_ctWidget.append(spinBox);

        addLabel(nRows(), 0, beforeLabelText);
        addWidget(nRows(), 1, spinBox->createWidget(*this));
        addLabel(nRows(), 2, afterLabelText);
        addDescription(nRows(), description);

        addRow();

        QString sep = "";
        if (!beforeLabelText.isEmpty())
        {
            if (beforeLabelText.back() == ":" || beforeLabelText.back() == "."  || beforeLabelText.back() == ",") {sep = " ";}
            else if (beforeLabelText.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(spinBox);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + beforeLabelText + "</span>" + sep + tr(" <em>__VALUE__%1</em>. __DESCRIPTION__</li><br>").arg((afterLabelText.isEmpty()?"":" ") + afterLabelText));
        m_paramDesc.append("<br>" + description);

        return spinBox;
    }

    return nullptr;
}

CT_CheckBox* CT_GenericConfigurableWidget::addBool(const QString &beforeLabelText,
                                           const QString &afterLabelText,
                                           const QString &checkBoxText,
                                           bool &valueToUpdate,
                                           const QString &description)
{
    if(canEdit())
    {
        CT_CheckBox *checkBox = new CT_CheckBox(checkBoxText, valueToUpdate, beforeLabelText);
        m_ctWidget.append(checkBox);

        addLabel(nRows(), 0, beforeLabelText);
        addWidget(nRows(), 1, checkBox->createWidget(*this));
        addLabel(nRows(), 2, afterLabelText);
        addDescription(nRows(), description);

        addRow();

        QString str = beforeLabelText;
        if (str.isEmpty()) {str = checkBoxText;}

        QString sep = "";
        if (!str.isEmpty())
        {
            if (str.back() == ":" || str.back() == "."  || str.back() == ",") {sep = " ";}
            else if (str.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(checkBox);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + str + "</span>" + sep + tr(" <em>__VALUE__%1</em>. __DESCRIPTION__</li><br>").arg((afterLabelText.isEmpty()?"":" ") + afterLabelText));
        m_paramDesc.append("<br>" + description);

        return checkBox;
    }

    return nullptr;
}

CT_LineEdit* CT_GenericConfigurableWidget::addString(const QString &beforeLabelText,
                                             const QString &afterLabelText,
                                             QString &valueToUpdate,
                                             const QString &description)
{
    if(canEdit())
    {
        CT_LineEdit *lineEdit = new CT_LineEdit(valueToUpdate, beforeLabelText);
        m_ctWidget.append(lineEdit);

        addLabel(nRows(), 0, beforeLabelText);
        addWidget(nRows(), 1, lineEdit->createWidget(*this));
        addLabel(nRows(), 2, afterLabelText);
        addDescription(nRows(), description);

        addRow();

        QString sep = "";
        if (!beforeLabelText.isEmpty())
        {
            if (beforeLabelText.back() == ":" || beforeLabelText.back() == "."  || beforeLabelText.back() == ",") {sep = " ";}
            else if (beforeLabelText.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(lineEdit);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + beforeLabelText + "</span>" + sep + tr(" <em>__VALUE__%1</em>. __DESCRIPTION__</li><br>").arg((afterLabelText.isEmpty()?"":" ") + afterLabelText));
        m_paramDesc.append("<br>" + description);

        return lineEdit;
    }

    return nullptr;
}

CT_MultiLineEdit* CT_GenericConfigurableWidget::addMultiString(const QString &beforeLabelText,
                                                  const QString &afterLabelText,
                                                  QString &valueToUpdate,
                                                  const QString &description)
{
    if(canEdit())
    {
        CT_MultiLineEdit *lineEdit = new CT_MultiLineEdit(valueToUpdate, beforeLabelText);
        m_ctWidget.append(lineEdit);

        addLabel(nRows(), 0, beforeLabelText);
        addWidget(nRows(), 1, lineEdit->createWidget(*this));
        addLabel(nRows(), 2, afterLabelText);
        addDescription(nRows(), description);

        addRow();

        QString sep = "";
        if (!beforeLabelText.isEmpty())
        {
            if (beforeLabelText.back() == ":" || beforeLabelText.back() == "."  || beforeLabelText.back() == ",") {sep = " ";}
            else if (beforeLabelText.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(lineEdit);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + beforeLabelText + "</span>" + sep + tr(" <em>__VALUE__%1</em>. __DESCRIPTION__</li><br>").arg((afterLabelText.isEmpty()?"":" ") + afterLabelText));
        m_paramDesc.append("<br>" + description);


        return lineEdit;
    }

    return nullptr;
}

CT_ComboBox* CT_GenericConfigurableWidget::addStringChoice(const QString &beforeLabelText,
                                                           const QString &afterLabelText,
                                                           const QStringList &valuesList,
                                                           QString &valueToUpdate,
                                                           const QString &description)
{
    CT_ComboBox *comboBox = nullptr;

    if(canEdit())
    {
        comboBox = new CT_ComboBox(valuesList, valueToUpdate, beforeLabelText);
        m_ctWidget.append(comboBox);

        addLabel(nRows(), 0, beforeLabelText);
        addWidget(nRows(), 1, comboBox->createWidget(*this));
        addLabel(nRows(), 2, afterLabelText);
        addDescription(nRows(), description);

        addRow();

        QString sep = "";
        if (!beforeLabelText.isEmpty())
        {
            if (beforeLabelText.back() == ":" || beforeLabelText.back() == "."  || beforeLabelText.back() == ",") {sep = " ";}
            else if (beforeLabelText.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(comboBox);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + beforeLabelText + "</span>" + sep + tr(" <em>__VALUE__%1</em>. __DESCRIPTION__</li><br>").arg((afterLabelText.isEmpty()?"":" ") + afterLabelText));
        m_paramDesc.append("<br>" + description);


        if (!valueToUpdate.isEmpty())
            comboBox->setWidgetValue(valueToUpdate);
    }

    return comboBox;
}

CT_ButtonGroup& CT_GenericConfigurableWidget::addButtonGroup(int &valueToUpdate)
{
    CT_ButtonGroup *bg = new CT_ButtonGroup(*this, valueToUpdate);
    m_ctWidget.append(bg);

    return *bg;
}

bool CT_GenericConfigurableWidget::addExcludeValue(const QString &beforeLabelText,
                                                   const QString &afterLabelText,
                                                   const QString &radioButtonText,
                                                   CT_ButtonGroup &group,
                                                   const int &id,
                                                   const QString &description,
                                                   bool leftRadio)
{
    if(canEdit())
    {
        CT_RadioButton *radioButton = new CT_RadioButton(radioButtonText, group.getValueReference(), id);

        if(!group.addRadioButton(radioButton))
        {
            delete radioButton;
            return false;
        }

        if(!m_ctWidget.contains(&group))
            m_ctWidget.append(&group);

        if(leftRadio) {
            addWidget(nRows(), 0, radioButton->createWidget(*this), 1, 2);
        } else {
            addLabel(nRows(), 0, beforeLabelText);
            addWidget(nRows(), 1, radioButton->createWidget(*this));
            addLabel(nRows(), 2, afterLabelText);
        }
        addDescription(nRows(), description);

        addRow();

        QString str = radioButtonText;
        if (str.isEmpty()) {str = beforeLabelText;}
        if (!afterLabelText.isEmpty()) {str.append(QString(" (%1)").arg(afterLabelText));}

        QString sep = "";
        if (!str.isEmpty())
        {
            if (str.back() == ":" || str.back() == "."  || str.back() == ",") {sep = " ";}
            else if (str.back() != " ") {sep = ". ";}
        }


        m_widgetList.append(radioButton);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + str + "</span>" + sep + tr(" <em>__VALUE__%1</em>. __DESCRIPTION__</li><br>").arg((afterLabelText.isEmpty()?"":" ") + afterLabelText));
        m_paramDesc.append("<br>" + description);

        return true;
    }

    return false;
}

void CT_GenericConfigurableWidget::addEmpty()
{
    if(canEdit())
    {
        addLabel(nRows(), 0, "");
        addRow();
    }
}

CT_FileChoiceButton* CT_GenericConfigurableWidget::addFileChoice(const QString &btLabel,
                                                                 const CT_FileChoiceButton::NeededFileType &filetype,
                                                                 const QString &fileFilter, QStringList &valueToUpdate,
                                                                 const QString &description,
                                                                 const QString &beforeLabelText,
                                                                 const QString &afterLabelText)
{
    CT_FileChoiceButton *fileChoiceButton = nullptr;

    if(canEdit())
    {
        fileChoiceButton = new CT_FileChoiceButton(btLabel, filetype, fileFilter, valueToUpdate, btLabel);
        m_ctWidget.append(fileChoiceButton);

        addLabel(nRows(), 0, beforeLabelText);
        addWidget(nRows(), 1, fileChoiceButton->createWidget(*this));
        addLabel(nRows(), 2, afterLabelText);
        addDescription(nRows(), description);

        addRow();

        QString str = beforeLabelText;
        if (str.isEmpty()) {str = btLabel;}

        QString sep = "";
        if (!str.isEmpty())
        {
            if (str.back() == ":" || str.back() == "."  || str.back() == ",") {sep = " ";}
            else if (str.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(fileChoiceButton);

        if (filetype == CT_FileChoiceButton::OneExistingFolder)
        {
            m_paramLab.append("<li><span class=\"parameterDescr\">" + str + "</span>" + sep + tr(" __DESCRIPTION__</li><br>"));
        } else {
            m_paramLab.append("<li><span class=\"parameterDescr\">" + str + "</span>" + sep + tr(" extension : %1. __DESCRIPTION__</li><br>").arg(fileFilter));

        }

        m_paramDesc.append("<br>" + description);



        fileChoiceButton->setWidgetValue(valueToUpdate);
    }

    return fileChoiceButton;
}

CT_AsciiFileChoiceButton *CT_GenericConfigurableWidget::addAsciiFileChoice(QString btlab,
                                                                           QString fileFilter,
                                                                           bool autoDetect,
                                                                           const QStringList &neededFields,
                                                                           QString &fileName,
                                                                           bool &header,
                                                                           QString &separator,
                                                                           QString &decimal,
                                                                           QLocale &locale,
                                                                           int &skip,
                                                                           QMap<QString, int> &columns,
                                                                           QString description)
{
    CT_AsciiFileChoiceButton *fileChoiceButton = nullptr;

    if(canEdit())
    {
        fileChoiceButton = new CT_AsciiFileChoiceButton(btlab,
                                                      fileFilter,
                                                      autoDetect,
                                                      neededFields,
                                                      fileName,
                                                      header,
                                                      separator,
                                                      decimal,
                                                      locale,
                                                      skip,
                                                      columns,
                                                      description);

        m_ctWidget.append(fileChoiceButton);

        addWidget(nRows(), 0, fileChoiceButton->createWidget(*this), 1, -1);

        addRow();

        QString sep = "";
        if (btlab.back() == ":" || btlab.back() == "."  || btlab.back() == ",") {sep = " ";}
        else if (btlab.back() != " ") {sep = ". ";}


        m_widgetList.append(fileChoiceButton);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + btlab + "</span>" + sep + tr(" extension : %1. __DESCRIPTION__</li><br>").arg(fileFilter));
        m_paramDesc.append("<br>" + description);


        fileChoiceButton->setWidgetValue(fileName);
    }

    return fileChoiceButton;
}

CT_AsciiFileChoiceButton *CT_GenericConfigurableWidget::addAsciiFileChoice(QString btlab,
                                                                           QString fileFilter,
                                                                           bool autoDetect,
                                                                           QList<CT_TextFileConfigurationFields> &neededFields,
                                                                           QString &fileName,
                                                                           bool &header,
                                                                           QString &separator,
                                                                           QString &decimal,
                                                                           QLocale &locale,
                                                                           int &skip,
                                                                           QMap<QString, int> &columns,
                                                                           QString description)
{
    CT_AsciiFileChoiceButton *fileChoiceButton = nullptr;

    if(canEdit())
    {
        fileChoiceButton = new CT_AsciiFileChoiceButton(btlab,
                                                      fileFilter,
                                                      autoDetect,
                                                      neededFields,
                                                      fileName,
                                                      header,
                                                      separator,
                                                      decimal,
                                                      locale,
                                                      skip,
                                                      columns,
                                                      description);
        m_ctWidget.append(fileChoiceButton);

        addWidget(nRows(), 0, fileChoiceButton->createWidget(*this), 1, -1);

        addRow();

        QString sep = "";
        if (!btlab.isEmpty())
        {
            if (btlab.back() == ":" || btlab.back() == "."  || btlab.back() == ",") {sep = " ";}
            else if (btlab.back() != " ") {sep = ": ";}
        }


        m_widgetList.append(fileChoiceButton);
        m_paramLab.append("<li><span class=\"parameterDescr\">" + btlab + "</span>" + sep + tr(" extension : %1. __DESCRIPTION__</li><br>").arg(fileFilter));
        m_paramDesc.append("<br>" + description);

        fileChoiceButton->setWidgetValue(fileName);
    }

    return fileChoiceButton;
}

void CT_GenericConfigurableWidget::saveSettings(SettingsWriterInterface &writer) const
{
    QListIterator<CT_WidgetWithValueReferenceInterface*> it(m_ctWidget);

    while(it.hasNext())
        it.next()->saveSettings(writer);
}

bool CT_GenericConfigurableWidget::restoreSettings(SettingsReaderInterface &reader)
{
    QListIterator<CT_WidgetWithValueReferenceInterface*> it(m_ctWidget);

    while(it.hasNext()) {
        if(!it.next()->restoreSettings(reader))
            return false;
    }

    return true;
}

bool CT_GenericConfigurableWidget::isSettingsModified() const
{
    // TODO : is method "isValueAndWidgetValueDifferent" ok for all element ? to verify and after you can uncomment it.
    /*QListIterator<CT_WidgetWithValueReferenceInterface*> it(m_ctWidget);
    while(it.hasNext()) {
        if(it.next()->isValueAndWidgetValueDifferent())
            return true;
    }

    return false;*/

    return true;
}

QString CT_GenericConfigurableWidget::helpText(bool addDescriptions)
{
    QString helpText;

    bool first = true;

    for (int i = 0 ; i < m_widgetList.size() ; i++)
    {
        CT_WidgetWithValueReferenceInterface* widget = m_widgetList.at(i);
        QString paramLab = m_paramLab.at(i);
        QString paramDesc = m_paramDesc.at(i);

        QString str;
        QVariant value;

        if (addDescriptions)
        {
            str = paramLab.replace("__DESCRIPTION__", paramDesc);
        } else {
            str = paramLab.replace("__DESCRIPTION__", "");

        }


        if (widget != nullptr)
        {
            CT_RadioButton* widgEx = dynamic_cast<CT_RadioButton*>(widget);
            if (widgEx != nullptr)
            {
                value = widgEx->getValue() == widgEx->getId();
            } else {
                value = widget->getValue();
            }
        }


        if (widget == nullptr)
        {
            helpText.append("</ul>");
            first = true;
        } else
        {
            if (first)
            {
                helpText.append("<ul>");
                first = false;
            }

            str.replace("__VALUE__", formatToUpdateValue(widget, value));
        }

        helpText.append(str);
    }

    if (!first)
    {
        helpText.append("</ul>");
    }

    return helpText;
}


QString CT_GenericConfigurableWidget::formatToUpdateValue(CT_WidgetWithValueReferenceInterface *widget, QVariant value)
{
    CT_SpinBox* widg0 = dynamic_cast<CT_SpinBox*>(widget);
    CT_DoubleSpinBox* widg1 = dynamic_cast<CT_DoubleSpinBox*>(widget);
    CT_CheckBox* widg2 = dynamic_cast<CT_CheckBox*>(widget);
    CT_LineEdit* widg3 = dynamic_cast<CT_LineEdit*>(widget);
    CT_MultiLineEdit* widg4 = dynamic_cast<CT_MultiLineEdit*>(widget);
    CT_ComboBox* widg5 = dynamic_cast<CT_ComboBox*>(widget);
    CT_RadioButton* widg6 = dynamic_cast<CT_RadioButton*>(widget);

    if (widg0 != nullptr)
    {
        return value.toString();

    } else if (widg1 != nullptr)
    {
        double val = value.toDouble() * widg1->getMultValue();
        return QString::number(val, 'f', widg1->getnDecimals());

    } else if (widg2 != nullptr)
    {
        return value.toBool()?tr("Activé"):tr("Désactivé");

    } else if (widg3 != nullptr)
    {
        QString val = value.toString();
        return val.isEmpty()?tr("(vide)"):val;

    } else if (widg4 != nullptr)
    {
        QString val = value.toString();
        return val.isEmpty()?tr("(vide)"):val;

    } else if (widg5 != nullptr)
    {
        QString val = value.toString();
        return val.isEmpty()?tr("(vide)"):val;

    } else if (widg6 != nullptr)
    {
        return value.toBool()?tr("Activé"):tr("Désactivé");
    }

    return value.toString();
}

QList<CT_WidgetWithValueReferenceInterface *> CT_GenericConfigurableWidget::allWidgets() const
{
    return m_ctWidget;
}

QGridLayout* CT_GenericConfigurableWidget::getLayout() const
{
    return m_gridLayout;
}

void CT_GenericConfigurableWidget::addWidget(int row,
                                             int column,
                                             QWidget *wid,
                                             int rowSpan,
                                             int colSpan)
{
    getLayout()->addWidget(wid, row, column, rowSpan, colSpan);
}

void CT_GenericConfigurableWidget::addLabel(int row,
                                            int column,
                                            const QString &text,
                                            int rowSpan,
                                            int colSpan)
{
    addWidget(row, column, new QLabel(text, this), rowSpan, colSpan);
}

void CT_GenericConfigurableWidget::addDescription(int row, const QString &description, int column)
{
    if(!description.isEmpty()) {
        HelpButton *wid = new HelpButton(this);
        wid->setPixmap(QPixmap(":/Icones/Icones/help.png").scaledToHeight(20,Qt::SmoothTransformation));
        wid->setToolTip(description);

        getLayout()->addWidget(wid, row, column);
    }
}

bool CT_GenericConfigurableWidget::canEdit() const
{
    return m_canEdit;
}

int CT_GenericConfigurableWidget::nRows() const
{
    return m_nRow;
}

void CT_GenericConfigurableWidget::addRow()
{
    ++m_nRow;
}

void CT_GenericConfigurableWidget::endEdit()
{
    if(m_canEdit) {
        foreach (CT_GroupBox* gp, m_groupBoxCollection) {
            gp->endEdit();
        }

        m_canEdit = false;
        ((QVBoxLayout*)m_centralWidget->layout())->addStretch();
        m_centralWidget->adjustSize();

        setMinimumSize(m_centralWidget->width()+20, qMin(m_centralWidget->height()+40, QGuiApplication::primaryScreen()->size().height()/2));
    }
}

bool CT_GenericConfigurableWidget::updateElement(QString *err)
{
    Q_UNUSED(err)

    QListIterator<CT_WidgetWithValueReferenceInterface*> it(m_ctWidget);

    while(it.hasNext())
        it.next()->updateValue();

    return true;
}
