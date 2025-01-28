#ifndef CT_READERRESULTADDINGTOOLS_H
#define CT_READERRESULTADDINGTOOLS_H

#include "pluginShared_global.h"
#include <QHash>

class CT_OutResultModelGroupToCopyPossibilities;
class CT_OutResultModelGroup;
class CT_AutoRenameModels;
class CT_StandardItemGroup;
class CT_AbstractReader;
class CT_ResultGroup;
class CT_AbstractModel;
class CT_VirtualAbstractStep;

class PLUGINSHAREDSHARED_EXPORT CT_ReaderResultAddingTools
{
public:
    CT_ReaderResultAddingTools();
    ~CT_ReaderResultAddingTools();

    void clear();

    //// MODEL ////

    /**
     * @brief Add results (model) of a reader (that was searched in Input) in a group of an OUTPUT results
     * @param inParentGroupModelName : the unique name of the group of resCpy to add results
     * @param inReaderModelName : the unique name of the reader searched in Input
     * @param resCpy : the result that contains the group to add results
     * @param autoIndexForFindReader : a index that you must use later (in your "compute" method) to add real results of this reader
     * @return true if it was no error
     */
    bool addReaderResults(const QString &inParentGroupModelName,
                          const QString &inReaderModelName,
                          CT_OutResultModelGroupToCopyPossibilities *resCpy,
                          int &autoIndexForFindReader);

    /**
     * @brief Add results (model) of a reader in a new group of an OUTPUT results
     * @param inParentGroupModelAutoRename : the autorename object that contains the unique name of the group to add results
     * @param reader : the reader prototype initialized with a filepath
     * @param resCpy : the result that contains the group to add results
     * @param autoIndexForFindReader : a index that you must use later (in your "compute" method) to add real results of this reader
     * @return true if it was no error
     */
    bool addReaderResults(const CT_AutoRenameModels &inParentGroupModelAutoRename,
                          CT_AbstractReader *reader,
                          CT_OutResultModelGroupToCopyPossibilities *resCpy,
                          int &autoIndexForFindReader);

    /**
     * @brief Add results (model) of a reader in a group of an OUTPUT results
     * @param inParentGroupModelName : the name of the group to add results
     * @param reader : the reader prototype initialized with a filepath
     * @param res : the result that contains the input group to add results
     * @param step : the step where the result will be added or was added (your step)
     * @param autoIndexForFindReader : a index that you must use later (in your "compute" method) to add real results of this reader
     * @return true if it was no error
     */
    bool addReaderResults(const QString &inParentGroupModelName,
                          CT_AbstractReader *reader,
                          CT_OutResultModelGroup *res,
                          CT_VirtualAbstractStep *step,
                          int &autoIndexForFindReader);

    //// REAL ////

    /**
     * @brief Add results of a reader in a group
     * @param outRes : the result of the group
     * @param group : the group where add results
     * @param reader : the reader to take results
     * @param autoIndexForFindReader : the index used in method "createOutResultModelListProtected" that link this reader
     * @return true if it was no error
     */
    bool addReaderResults(CT_ResultGroup *outRes,
                          CT_StandardItemGroup *group,
                          CT_AbstractReader* reader,
                          int &autoIndexForFindReader);

private:
    QList< QHash<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > > m_links;
};

#endif // CT_READERRESULTADDINGTOOLS_H
