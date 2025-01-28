#include "ct_readerresultaddingtools.h"

#include "ct_result/model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.h"
#include "ct_result/model/outModel/tools/ct_iteratorgroupsmodelforresultmodelgroup.h"
#include "ct_result/model/outModel/ct_outresultmodelgroup.h"
#include "ct_tools/model/ct_outmodelcopyactionaddreaderresultmodelingroup.h"

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_result/ct_resultgroup.h"

#include "ct_model/tools/ct_modelsearchhelper.h"

CT_ReaderResultAddingTools::CT_ReaderResultAddingTools()
{
}

CT_ReaderResultAddingTools::~CT_ReaderResultAddingTools()
{
    clear();
}

void CT_ReaderResultAddingTools::clear()
{
    QListIterator< QHash<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > > it(m_links);

    while(it.hasNext()) {
        QHash<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > h = it.next();
        QHashIterator<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > itH(h);

        while(itH.hasNext()) {
            itH.next();
            qDeleteAll(itH.value().begin(), itH.value().end());
        }
    }

    m_links.clear();
}

bool changeModelOfElement(CT_AbstractItemDrawable *item,
                          const QHash<QString, CT_AutoRenameModels*> &links)
{
    // search the AutoRename tools created for the model of this group
    CT_AutoRenameModels *modelCreationAutoRename = links.value(item->model()->uniqueName(), NULL);

    if(modelCreationAutoRename == NULL)
        return false;

    item->setModel(modelCreationAutoRename->completeName());

    return true;
}

bool changeModelOfItems(const QHash<QString, CT_GroupItemDrawableContainer*> &itemsContainers,
                        const QHash<QString, CT_AutoRenameModels*> &links)
{
    QHashIterator<QString, CT_GroupItemDrawableContainer*> it(itemsContainers);

    while(it.hasNext()) {
        it.next();

        if(!changeModelOfElement(it.value()->item(), links))
            return false;
    }

    return true;
}

bool recursiveChangeModelOfGroups(const QHash<QString, CT_Container*> &groupsContainers,
                                  const QHash<QString, CT_AutoRenameModels*> &links)
{
    QHashIterator<QString, CT_Container*> it(groupsContainers);

    while(it.hasNext()) {
        it.next();

        QList<CT_AbstractItemDrawable*> *groups = it.value()->getList();
        QListIterator<CT_AbstractItemDrawable*> itG(*groups);

        while(itG.hasNext()) {
            CT_AbstractItemGroup *group = dynamic_cast<CT_AbstractItemGroup*>(itG.next());

            if(!recursiveChangeModelOfGroups(group->groups(), links))
                return false;

            if(!changeModelOfItems(group->items(), links))
                return false;

            if(!changeModelOfElement(group, links))
                return false;
        }
    }

    return true;
}

bool addGroupToParentAndChangeModelOfChildrens(CT_AbstractItemGroup *groupToCompute,
                                               CT_StandardItemGroup *parent,
                                               const QHash<QString, CT_AutoRenameModels*> &links)
{
    if(!recursiveChangeModelOfGroups(groupToCompute->groups(), links))
        return false;

    if(!changeModelOfItems(groupToCompute->items(), links))
        return false;

    return parent->addGroup(groupToCompute);
}

bool CT_ReaderResultAddingTools::addReaderResults(const QString &inParentGroupModelName,
                                                  const QString &inReaderModelName,
                                                  CT_OutResultModelGroupToCopyPossibilities *resCpy,
                                                  int &autoIndexForFindReader)
{
    if(resCpy == NULL)
        return false;

    QListIterator<CT_OutResultModelGroupCopy*> it(resCpy->getModels());

    while(it.hasNext())
    {
        CT_OutResultModelGroupCopy *model = it.next();

        CT_OutResultModelGroupToCopyPossibility *poss = model->outResultModelCopy();
        CT_VirtualAbstractStep *step = model->step();

        if((step == NULL) || (poss == NULL))
            return false;

        CT_OutModelCopyActionAddReaderResultModelInGroup action(inParentGroupModelName,
                                                                inReaderModelName);

        if(!action.execute(step, poss))
            return false;

        QHash<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > linksByModel = action.takeAutoRenameLinksByGroupModel();

        autoIndexForFindReader = m_links.size();
        m_links.insert(autoIndexForFindReader, linksByModel);
    }

    return true;
}

bool CT_ReaderResultAddingTools::addReaderResults(const CT_AutoRenameModels &inParentGroupModelAutoRename,
                                                  CT_AbstractReader *reader,
                                                  CT_OutResultModelGroupToCopyPossibilities *resCpy,
                                                  int &autoIndexForFindReader)
{
    if(resCpy == NULL)
        return false;

    QListIterator<CT_OutResultModelGroupCopy*> it(resCpy->getModels());

    while(it.hasNext())
    {
        CT_OutResultModelGroupCopy *model = it.next();

        CT_OutResultModelGroupToCopyPossibility *poss = model->outResultModelCopy();
        CT_VirtualAbstractStep *step = model->step();

        if((step == NULL) || (poss == NULL))
            return false;

        CT_OutModelCopyActionAddReaderResultModelInGroup action(inParentGroupModelAutoRename,
                                                                reader);

        if(!action.execute(step, poss))
            return false;

        QHash<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > linksByModel = action.takeAutoRenameLinksByGroupModel();

        autoIndexForFindReader = m_links.size();
        m_links.insert(autoIndexForFindReader, linksByModel);
    }

    return true;
}

bool CT_ReaderResultAddingTools::addReaderResults(const QString &inParentGroupModelName,
                                                  CT_AbstractReader *reader,
                                                  CT_OutResultModelGroup *res,
                                                  CT_VirtualAbstractStep *step,
                                                  int &autoIndexForFindReader)
{
    if(res == NULL)
        return false;

    // on récupère le groupe défini dans "createInResultModelListProtected()"
    // pour la recherche
    DEF_CT_AbstractGroupModelIn *inGroupModel = dynamic_cast< DEF_CT_AbstractGroupModelIn* >(PS_MODELS->searchModel(inParentGroupModelName, res, step));

    Q_ASSERT(inGroupModel != NULL);

    QHash<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > linksByModel;

    CT_IteratorGroupsModelForResultModelGroup it(res);

    if(it.recursiveBeginIterateGroups(inGroupModel))
    {
        DEF_CT_AbstractGroupModelOut *outGroupModel;

        while((outGroupModel = it.recursiveNextGroup()) != NULL)
        {
            if(!CT_OutModelCopyActionAddReaderResultModelInGroup::staticAddElementOfReaderToGroup(reader, outGroupModel, linksByModel)) {
                autoIndexForFindReader = m_links.size();
                m_links.insert(autoIndexForFindReader, linksByModel);
                return false;
            }
        }
    }

    autoIndexForFindReader = m_links.size();
    m_links.insert(autoIndexForFindReader, linksByModel);

    return true;
}

bool CT_ReaderResultAddingTools::addReaderResults(CT_ResultGroup *outRes,
                                                  CT_StandardItemGroup *group,
                                                  CT_AbstractReader *reader,
                                                  int &autoIndexForFindReader)
{
    if((autoIndexForFindReader < 0) || (autoIndexForFindReader >= m_links.size()))
        return false;

    // get links of this reader sorted by possibility (CT_AbstractModel* is one possiblity)
    QHash<CT_AbstractModel*, QHash<QString, CT_AutoRenameModels*> > linksByModel = m_links.at(autoIndexForFindReader);

    // check if links contains this possibility (the model of the group to add elements)
    if(!linksByModel.contains(group->model()))
        return false;

    // get links for this possibility (now we can link between a model defined in the reader and the new model created with a AutoRename tools)
    QHash<QString, CT_AutoRenameModels*> links = linksByModel.value(group->model());

    /////// recursive add groups and items  ///////

    // get models of group of this reader
    QListIterator<CT_OutStdGroupModel*> itGM(reader->outGroupsModel());

    while(itGM.hasNext())
    {
        CT_OutStdGroupModel *model = itGM.next();

        // search the AutoRename tools created for this model (a group)
        CT_AutoRenameModels *modelCreationAutoRename = links.value(model->uniqueName(), NULL);

        if(modelCreationAutoRename == NULL)
            return false;

        // take groups of this reader and set it the new model (searched by completeName() of the AutoRename tools)
        QList<CT_AbstractItemGroup*> groups = reader->takeGroupOfModel(model->uniqueName(), outRes, modelCreationAutoRename->completeName());

        QListIterator<CT_AbstractItemGroup*> itG(groups);

        while(itG.hasNext()) {
            bool ok = addGroupToParentAndChangeModelOfChildrens(itG.next(), group, links);
            Q_ASSERT_X(ok, "CT_ReaderResultAddingTools addReaderResults", "Problem to add a group or children of a reader !");
        }
    }

    /////// add itemdrawables  ///////

    QListIterator<CT_OutStdSingularItemModel*> it(reader->outItemDrawableModels());

    while(it.hasNext())
    {
        CT_OutStdSingularItemModel *model = it.next();

        // search the AutoRename tools created for this model (a itemdrawable)
        CT_AutoRenameModels *modelCreationAutoRename = links.value(model->uniqueName(), NULL);

        if(modelCreationAutoRename == NULL)
            return false;

        // take itemdrawables of this reader and set it the new model (searched by completeName() of the AutoRename tools)
        QList<CT_AbstractSingularItemDrawable*> items = reader->takeItemDrawableOfModel(model->uniqueName(), outRes, modelCreationAutoRename->completeName());
        QListIterator<CT_AbstractSingularItemDrawable*> itI(items);

        while(itI.hasNext()) {
            bool ok = group->addItemDrawable(itI.next());
            Q_ASSERT_X(ok, "CT_ReaderResultAddingTools addReaderResults", "Problem to add a item of a reader !");
        }
    }

    return true;
}
