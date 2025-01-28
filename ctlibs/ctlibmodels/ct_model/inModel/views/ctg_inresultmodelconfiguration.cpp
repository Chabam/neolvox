#include "ctg_inresultmodelconfiguration.h"
#include "ui_ctg_inresultmodelconfiguration.h"

#include <QMessageBox>
#include <QTimer>

#include <QPixmap>
#include <QPainter>
#include <QFileDialog>

CTG_InResultModelConfiguration::CTG_InResultModelConfiguration(IInModelPossibilitiesChoice* widgetInModelPossibilities, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTG_InResultModelConfiguration)
{
    ui->setupUi(this);

    m_inputResultModelManager = nullptr;

    mInModelPossibilities = widgetInModelPossibilities;
    ui->horizontalLayout->addWidget(mInModelPossibilities);

    connect(ui->inResultModelPossibilities, SIGNAL(showInResultModelPossibility(const CT_InStdResultModelPossibility*)), mInModelPossibilities, SLOT(setInResultModelPossibility(const CT_InStdResultModelPossibility*)), Qt::DirectConnection);
    connect(ui->pb_capture, SIGNAL(clicked()), this, SLOT(captureView()));
}

CTG_InResultModelConfiguration::~CTG_InResultModelConfiguration()
{
    delete mInModelPossibilities;

    delete ui;
}

void CTG_InResultModelConfiguration::setInResultModelManager(const CT_InModelStructureManager* manager)
{
    m_inputResultModelManager = const_cast<CT_InModelStructureManager*>(manager);

    ui->inResultModelPossibilities->setInResultModelManager(m_inputResultModelManager);
}

void CTG_InResultModelConfiguration::setReadOnly(bool enabled)
{
    mInModelPossibilities->setReadOnly(enabled);
    ui->inResultModelPossibilities->setReadOnly(enabled);
}

void CTG_InResultModelConfiguration::accept()
{
    if(mInModelPossibilities->isReadOnly()) {
        QTimer::singleShot(100, this, SLOT(reject()));
        return;
    }

    if(m_inputResultModelManager != nullptr) {
        QStringList errors;

        const bool ok = m_inputResultModelManager->visitResults([&errors](const CT_InAbstractResultModel* resultModel) -> bool {
            return resultModel->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired(&errors);
        });

        if(!ok) {
            QMessageBox::warning(this, tr("Warning"), tr("You have not correctly configured the input data. List of detected errors :\r\n\r\n- %1").arg(errors.join("\r\n- ")), QMessageBox::Ok);

            return;
        }
    }

    QDialog::accept();
}

void CTG_InResultModelConfiguration::captureView()
{
    QString outFile = QFileDialog::getSaveFileName(nullptr, tr("Choix du fichier d'export de la capture d'écran"), "", "Fichier PNG (*.png)");

    if (!outFile.isEmpty())
    {
        exportViewCapture(outFile);
    }
}

bool CTG_InResultModelConfiguration::exportViewCapture(QString exportPath)
{
    if (!exportPath.isEmpty())
    {
        int contentHeight = mInModelPossibilities->getContentHeight();

        if (contentHeight >= 0)
        {
            QRect rect = this->rect();
            QRect rect2 = rect;

            rect2.setHeight(std::max(this->size().height(), this->size().height() - mInModelPossibilities->size().height() + contentHeight + 100));

            this->resize(rect2.size());

            QPixmap pixmap(rect2.size());
            QPainter painter(&pixmap);
            this->render(&painter);

            this->resize(rect.size());

            QFile file(exportPath);
            if (file.exists()) {file.remove();}

            return pixmap.save(exportPath);
        }
    }
    return false;
}

int CTG_InResultModelConfiguration::getSelectableResultsNumber()
{
    return ui->inResultModelPossibilities->resultCount();
}

void CTG_InResultModelConfiguration::selectResultByRank(int rank)
{
    ui->inResultModelPossibilities->selectResultByRank(rank);
}
