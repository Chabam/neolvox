#include "ct_uniqueindexgenerator.h"

CT_UniqueIndexGenerator::CT_UniqueIndexGenerator() :
    m_currentIndex(0)
{
}

void CT_UniqueIndexGenerator::resetIndex()
{
    m_currentIndex = 0;
    m_startIndexes.clear();
}

void CT_UniqueIndexGenerator::resetIndexOfObject(const void* object)
{
    m_currentIndex = m_startIndexes.value(const_cast<void*>(object), m_currentIndex);
}

void CT_UniqueIndexGenerator::setCurrentObject(const void* object)
{
    m_startIndexes.insert(const_cast<void*>(object), m_startIndexes.value(const_cast<void*>(object), m_currentIndex));
}

void CT_UniqueIndexGenerator::objectWillBeDeleted(const void* object)
{
    m_currentIndex = m_startIndexes.value(const_cast<void*>(object));

    m_startIndexes.remove(const_cast<void*>(object));
}

int CT_UniqueIndexGenerator::generateNewUniqueIndex()
{
    return m_currentIndex++;
}

int CT_UniqueIndexGenerator::operator()()
{
    return generateNewUniqueIndex();
}
