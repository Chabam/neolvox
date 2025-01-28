#include "ct_configurablewidgettodialog.h"

#include "ct_view/ct_abstractconfigurablewidget.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

CT_ConfigurableWidgetToDialog::CT_ConfigurableWidgetToDialog()
{
}

int CT_ConfigurableWidgetToDialog::exec(CT_AbstractConfigurableWidget *widget)
{
    QDialog dialog;
    dialog.setWindowTitle(widget->windowTitle());

    QHBoxLayout *layout = new QHBoxLayout(&dialog);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Vertical, &dialog);
    buttonBox->addButton(QObject::tr("Appliquer"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(QObject::tr("Annuler"), QDialogButtonBox::RejectRole);

    layout->addWidget(widget);
    layout->addWidget(buttonBox);

    QObject::connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    int ret;
    bool ok = false;
    widget->setVisible(true);

    while(!ok && ((ret = dialog.exec()) == QDialog::Accepted)) {
        QString err;
        ok = true;

        if(!widget->canAcceptIt(&err) || !widget->updateElement(&err)) {
            ok = false;
            QString pre = QObject::tr("La configuration est invalide");

            if(!err.isEmpty())
                pre += QObject::tr(" :\r\n\r\n%1\r\n\r\n").arg(err);
            else
                pre += QObject::tr(".");

            pre += QObject::tr("Veuillez corriger les erreurs.");

            QMessageBox::critical(nullptr, QObject::tr("Erreur"), pre);
        }
    }

    layout->removeWidget(widget);
    widget->setParent(nullptr);
    widget->setVisible(false);

    return ret;
}
