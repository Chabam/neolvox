#include "ct_standardreaderseparator.h"

CT_StandardReaderSeparator::CT_StandardReaderSeparator(const QString& title)
{
    m_title = title;
}

CT_StandardReaderSeparator::~CT_StandardReaderSeparator()
{
    qDeleteAll(m_readers.begin(), m_readers.end());
}

bool CT_StandardReaderSeparator::addReader(CT_AbstractReader *reader)
{
    m_readers.append(reader);
    return true;
}

QString CT_StandardReaderSeparator::title() const
{
    return m_title;
}

QList<CT_AbstractReader*> CT_StandardReaderSeparator::readers() const
{
    return m_readers;
}
