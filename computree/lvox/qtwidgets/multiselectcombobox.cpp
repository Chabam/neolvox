#include "multiselectcombobox.h"

namespace {
    const int noneIndex = 0;
    const int allIndex = 1;
}

///////////////////////////////////////////////////////////
/// PUBLIC METHODS
///////////////////////////////////////////////////////////
MultiselectComboBox::MultiselectComboBox(QWidget* aParent) :
    QComboBox(aParent),
    _isHidden(false),
    _widgets(new QListWidget(this)),
    _lineEdits(new QLineEdit(this))
{
    QListWidgetItem* noneWidgetItem = new QListWidgetItem(_widgets);

    QPushButton* noneButton = new QPushButton(this);
    noneButton->setText(tr("None"));
    _widgets->addItem(noneWidgetItem);
    _widgets->setItemWidget(noneWidgetItem, noneButton);

    QListWidgetItem* listWidgetItem = new QListWidgetItem(_widgets);
    QPushButton* allButton = new QPushButton(this);
    allButton->setText(tr("All"));
    _widgets->addItem(listWidgetItem);
    _widgets->setItemWidget(listWidgetItem, allButton);

    _lineEdits->setReadOnly(true);
    _lineEdits->installEventFilter(this);

    setModel(_widgets->model());
    setView(_widgets);
    setLineEdit(_lineEdits);

    connect(noneButton, &QPushButton::clicked, this, &MultiselectComboBox::resetSelection);
    connect(allButton, &QPushButton::clicked, this, &MultiselectComboBox::selectAll);
    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &MultiselectComboBox::itemClicked);
}

MultiselectComboBox::~MultiselectComboBox()
{

}

void MultiselectComboBox::addItem(const QString& text, bool value)
{
    QListWidgetItem* listWidgetItem = new QListWidgetItem(_widgets);
    QCheckBox* checkBox = new QCheckBox(this);
    checkBox->setText(text);
    _widgets->addItem(listWidgetItem);
    _widgets->setItemWidget(listWidgetItem, checkBox);
    connect(checkBox, &QCheckBox::stateChanged, this, &MultiselectComboBox::stateChanged);
    checkBox->setChecked(value);
}

void MultiselectComboBox::addItems(const QStringList& aTexts)
{

    for (size_t i = 0; i < static_cast<size_t>(aTexts.size()); i++)
        addItem(aTexts[i]);
}

void MultiselectComboBox::setPlaceHolderText(const QString& aPlaceHolderText)
{
    _lineEdits->setPlaceholderText(aPlaceHolderText);
}

void MultiselectComboBox::hidePopup()
{
    int width = this->width();
    int height = _widgets->height();
    int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();
    int y = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y() + geometry().y();
    if (x >= 0 && x <= width && y >= this->height() && y <= height + this->height())
    {
        _isHidden = false;
    }
    else
    {
        _isHidden = true;
        QComboBox::hidePopup();
    }
}

QVector<size_t> MultiselectComboBox::getSelectedIds()
{
    QVector<size_t> v;

    if(_widgets->count() == 2)
        return v;

    for (int p = BUTTONS_COUNT; p < _widgets->count(); p++)
    {
        QWidget *widget = _widgets->itemWidget(_widgets->item(p));
        QCheckBox *checkBox = static_cast<QCheckBox *>(widget);
        if(checkBox->isChecked())
            v.append(p - BUTTONS_COUNT);
    }

    return v;
}

void MultiselectComboBox::setSelectedIds(QVector<size_t> ids)
{
    this->blockSignals(true);
    for (const auto i : ids)
    {
        QWidget* widget = _widgets->itemWidget(_widgets->item(i + BUTTONS_COUNT));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        checkBox->setChecked(true);
    }
    this->blockSignals(false);
    stateChanged(0);
}

void MultiselectComboBox::stateChanged(int state)
{
    Q_UNUSED(state);
    QString selectedData("");

    size_t selectedItems = 0;

    for (int i = BUTTONS_COUNT; i < _widgets->count(); i++)
    {
        QWidget* widget = _widgets->itemWidget(_widgets->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);

        if (checkBox->isChecked())
        {
            selectedItems += 1;
            selectedData += checkBox->text() + "; ";
        }
    }

    if (selectedItems > 2)
        _lineEdits->setText(tr("Multiple (%1)").arg(selectedItems));
    else if (selectedItems > 0)
        _lineEdits->setText(selectedData);
    else
        _lineEdits->setText(tr("None"));

    _lineEdits->setToolTip(selectedData);

    emit selectionChanged();
}

QStringList MultiselectComboBox::currentText() const
{
    QStringList emptyStringList;
    if (!_lineEdits->text().isEmpty())
    {
        emptyStringList = _lineEdits->text().split(';');
    }
    return emptyStringList;
}

int MultiselectComboBox::count() const
{
    return _widgets->count() - BUTTONS_COUNT;
}

bool MultiselectComboBox::isTextInWidgets(QString& str)
{
    auto ids = getSelectedIds();
    for(const auto id : ids)
    {
        QWidget* widget = _widgets->itemWidget(_widgets->item(id + BUTTONS_COUNT));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        if(checkBox->text() == str)
            return true;
    }

    return false;
}


void MultiselectComboBox::itemClicked(int aIndex)
{
    if (aIndex != noneIndex && aIndex != allIndex)
    {
        QWidget* widget = _widgets->itemWidget(_widgets->item(aIndex));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        checkBox->setChecked(!checkBox->isChecked());
    }
}

void MultiselectComboBox::clear()
{
    _widgets->clear();
    QListWidgetItem* curItem = new QListWidgetItem(_widgets);
    _widgets->addItem(curItem);
}

void MultiselectComboBox::selectAll()
{
    this->blockSignals(true);
    for(size_t i = 2; i < static_cast<size_t>(_widgets->count()); i++)
    {
        QWidget* widget = _widgets->itemWidget(_widgets->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        checkBox->setChecked(true);
    }
    this->blockSignals(false);
    stateChanged(0);
}

bool MultiselectComboBox::eventFilter(QObject* aObject, QEvent* aEvent)
{
    if (aObject == _lineEdits && aEvent->type() == QEvent::MouseButtonRelease) {
        showPopup();
        return false;
    }
    return false;
}

void MultiselectComboBox::setCurrentText(const QStringList& aText)
{
    for (int i = 1; i <  _widgets->count(); ++i)
    {
        QWidget* widget = _widgets->itemWidget(_widgets->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        QString checkBoxString = checkBox->text();
        if (aText.contains(checkBoxString))
        {
            checkBox->setChecked(true);
        }
    }
}

void MultiselectComboBox::resetSelection()
{
    this->blockSignals(true);
    for (int i = 1; i <  _widgets->count(); ++i)
    {
        QWidget* widget = _widgets->itemWidget(_widgets->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        checkBox->setChecked(false);
    }
    this->blockSignals(false);
    stateChanged(0);
}

void MultiselectComboBox::wheelEvent(QWheelEvent *aWheelEvent)
{
    Q_UNUSED(aWheelEvent);
}

void MultiselectComboBox::keyPressEvent(QKeyEvent *aEvent)
{
    // Do not handle key event
    Q_UNUSED(aEvent);
}

void MultiselectComboBox::setCurrentText(const QString &aText)
{
    Q_UNUSED(aText);
}
