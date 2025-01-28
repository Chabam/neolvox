#ifndef CT_UNIQUEINDEXGENERATOR_H
#define CT_UNIQUEINDEXGENERATOR_H

#include <QHash>

/**
 * @brief Class that generate a new unique index each time we request it
 */
class CT_UniqueIndexGenerator
{
public:
    /**
     * @brief Construct the object with a unique index set to 0 by default
     */
    CT_UniqueIndexGenerator();

    /**
     * @brief Reset completely the index
     */
    void resetIndex();

    /**
     * @brief
     */
    void resetIndexOfObject(const void* object);

    /**
     * @brief
     */
    void setCurrentObject(const void* object);

    /**
     * @brief
     */
    void objectWillBeDeleted(const void* object);

    /**
     * @brief Returns the current unique index and increment it for the next call
     */
    int generateNewUniqueIndex();

    /**
     * @brief Call the method "generateNewUniqueIndex()" and returns the result
     */
    int operator()();

private:
    int                     m_currentIndex;

    QHash<void*, int>    m_startIndexes;
};

#endif // CT_UNIQUEINDEXGENERATOR_H
