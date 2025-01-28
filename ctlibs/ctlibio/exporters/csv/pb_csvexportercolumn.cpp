#include "pb_csvexportercolumn.h"

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"

PB_CSVExporterColumn::PB_CSVExporterColumn(const QList<const CT_AbstractSingularItemDrawable *> &items, const CT_OutAbstractItemAttributeModel *iaModel)
{
    _items = items;
    _ref = iaModel;

    m_currentItemAttribut = nullptr;
    _currentItem = nullptr;
    _hasNext = false;
    _currentItemIndex = -1;

    _iterator = new QListIterator<const CT_AbstractSingularItemDrawable*>(_items);

    nextItem();
}

PB_CSVExporterColumn::~PB_CSVExporterColumn()
{
    delete _iterator;
}

int PB_CSVExporterColumn::currentItemIndex() const
{
    return _currentItemIndex;
}

bool PB_CSVExporterColumn::hasNextValue() const
{
    return _hasNext;
}

QString PB_CSVExporterColumn::nextValueToString()
{
    QString next;

    next = m_currentItemAttribut->toString(_currentItem, nullptr);

    // on passe à l'item suivant
    nextItem();

    return next;
}

size_t PB_CSVExporterColumn::size() const
{
    size_t s = 0;

    _iterator->toFront();

    while(_iterator->hasNext())
    {
        if(_iterator->next()->itemAttributeWithOutModel(_ref) != nullptr)
            ++s;
    }

    _iterator->toFront();

    if(_currentItem != nullptr)
        _iterator->findNext(_currentItem);

    return s;
}

void PB_CSVExporterColumn::nextItem()
{
    if(_iterator->hasNext())
    {
        do
        {
            _currentItem = _iterator->next();
            m_currentItemAttribut = _currentItem->itemAttributeWithOutModel(_ref);

            ++_currentItemIndex;

            _hasNext = (m_currentItemAttribut != nullptr);

        }while(!_hasNext
               && _iterator->hasNext());
    }
    else
    {
        m_currentItemAttribut = nullptr;
        _currentItem = nullptr;
        _hasNext = false;
        ++_currentItemIndex;
    }
}
