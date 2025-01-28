#include "ct_groupbox.h"

#include "settingsinterfaces.h"

#include <QGroupBox>

CT_GroupBox::CT_GroupBox(const QString &title,
                         QWidget *parent) : CT_GenericConfigurableWidget(false, parent)
{
    setWindowTitle(title);

    m_gp = nullptr;
}

QWidget* CT_GroupBox::createWidget(QWidget &parent)
{
    QGroupBox* gp = new QGroupBox(windowTitle(), &parent);
    gp->setCheckable(true);

    QString style = QString("QGroupBox::indicator {"
                                "width: 12px;"
                                "height: 12px;"
                            "}"
                            "QGroupBox::indicator:unchecked {"
                                "image: url(:/Icones/Icones/stylesheet-branch-closed.png);"
                            "}"
                            "QGroupBox::indicator:checked {"
                                "image: url(:/Icones/Icones/stylesheet-branch-open.png);"
                            "}");

    gp->setStyleSheet(style);

    QVBoxLayout *lay = new QVBoxLayout(gp);
    lay->addWidget(this);

    connect(gp, SIGNAL(toggled(bool)), this, SLOT(collapseOrExpandGroupBox(bool)));

    m_gp = gp;
    collapseOrExpandGroupBox(false);

    return gp;
}

void CT_GroupBox::updateValue()
{
    updateElement(nullptr);
}

bool CT_GroupBox::isValueAndWidgetValueDifferent() const
{
    QListIterator<CT_WidgetWithValueReferenceInterface*> it(allWidgets());

    while(it.hasNext()) {
        if(it.next()->isValueAndWidgetValueDifferent())
            return true;
    }

    return false;
}

void CT_GroupBox::saveSettings(SettingsWriterInterface &writer) const
{
    writer.beginWriteChildrens(this);
    SuperClass::saveSettings(writer);
    writer.endWriteChildrens(this);
}

bool CT_GroupBox::restoreSettings(SettingsReaderInterface &reader)
{
    reader.beginReadChildrens(this);
    bool ok = SuperClass::restoreSettings(reader);
    reader.endReadChildrens(this);

    return ok;
}

void CT_GroupBox::collapseOrExpandGroupBox(bool expand)
{
    if(m_gp != nullptr) {

        m_gp->setMinimumHeight(0);
        m_gp->setMaximumHeight(16777215);

        if(expand) {
            m_gp->adjustSize();
            m_gp->setMinimumHeight(m_gp->height());
        } else {
            m_gp->setMaximumHeight(25);
        }

        adjustSize();

        if(expand != m_gp->isChecked())
            m_gp->setChecked(expand);
    }
}
