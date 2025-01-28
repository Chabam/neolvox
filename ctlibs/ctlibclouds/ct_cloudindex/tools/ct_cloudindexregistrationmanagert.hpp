#include "ct_cloudindex/tools/ct_cloudindexregistrationmanagert.h"

#include "ct_cloudindex/registered/ct_standardmodifiablecloudindexregisteredt.h"
#include "ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.h"

#include "ct_cloudindex/ct_cloudindexlessmemoryt.h"

#include "ct_cloud/tools/ct_globalcloudmanagert.h"

template<typename T, class CloudIndexLessMemory>
CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::CT_CloudIndexRegistrationManagerT(const CT_AbstractGlobalCloudManagerT<T> &gcManager) : CT_AbstractCloudIndexRegistrationManagerT<T>()
{
    m_gcManager = const_cast<CT_AbstractGlobalCloudManagerT<T>*>(&gcManager);
    m_gcManager->addGlobalCloudListener(this);
}

template<typename T, class CloudIndexLessMemory>
typename CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::CT_AbstractModifiableCIR CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::registerIndex(CT_AbstractModifiableCloudIndexT<T> *index)
{
    QMutexLocker locker(this->m_mutex);

    if(index == nullptr)
        return CT_AbstractModifiableCIR(nullptr);

    CT_AbstractModifiableCIR cir = CT_AbstractModifiableCIR(this->template createNewIndexRegistered< CT_StandardModifiableCloudIndexRegisteredT<T>, CT_AbstractModifiableCloudIndexT<T> >(index));

    CT_AbstractCloudIndexRegistrationManagerT<T>::registerIndex(cir);

    return cir;
}

template<typename T, class CloudIndexLessMemory>
typename CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::CT_AbstractNotModifiableCIR CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::mergeCloudContiguous(const QList< CT_AbstractCIR > &cir_collection)
{
    QMutexLocker locker(this->m_mutex);

    if(cir_collection.isEmpty())
        return CT_AbstractNotModifiableCIR(nullptr);

    size_t begin = cir_collection.first()->first();
    size_t end = cir_collection.first()->last()+1;

    QListIterator< CT_AbstractCIR > it(cir_collection);
    it.next();

    while(it.hasNext())
    {
        CT_AbstractCIR cir = it.next();

        if(cir->first() != end)
            return CT_AbstractNotModifiableCIR(nullptr);

        end = cir->last()+1;
    }

    size_t size = end-begin;

    CT_AbstractNotModifiableCIR cir = CT_AbstractNotModifiableCIR(this->template createNewIndexRegistered< CT_StandardNotModifiableCloudIndexRegisteredT<T>, CloudIndexLessMemory >(new CloudIndexLessMemory(begin, size)));

    CT_AbstractCloudIndexRegistrationManagerT<T>::registerIndex(cir);

    return cir;
}

template<typename T, class CloudIndexLessMemory>
void CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::cloudDeleted(const size_t &beginIndex, const size_t &size)
{
    QMutexLocker locker(this->m_mutex);

    syncModifiableIndexRegistered(beginIndex, size);
    syncLessMemoryIndexRegistered(beginIndex, size);
}

template<typename T, class CloudIndexLessMemory>
void CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::syncModifiableIndexRegistered(const size_t &beginIndex, const size_t &size)
{
    QList< CT_AbstractModifiableCloudIndexRegisteredT<T>* > toUnregister;

    // fin du nuage qui va être supprimé
    size_t endIndex = (beginIndex+size)-1;

    // on parcours la liste des index enregistré
    QListIterator< CT_AbstractModifiableCloudIndexRegisteredT<T>* > it(this->m_cirArray);

    while(it.hasNext())
    {
        const CT_AbstractModifiableCloudIndexRegisteredT<T> *cir = it.next();

        CT_AbstractCloudIndexT<T> *index = cir->abstractCloudIndexT();

        size_t isize = index->size();

        if(isize > 0)
        {
            if(index->sortType() == CT_AbstractCloudIndex::NotSorted)
            {
                CT_AbstractModifiableCloudIndexT<T> *indexModif = cir->abstractModifiableCloudIndexT();
                size_t shiftV = size;

                FindShiftRemoveContext context;
                context.beginIndex = beginIndex;
                context.endIndex = endIndex;

                indexModif->removeIfOrShiftIf(&CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::staticFindIf,
                                              &CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::staticRemoveIf,
                                              &CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::staticShiftIf,
                                              shiftV,
                                              true,
                                              &context);
            }
            else
            {
                size_t b;
                size_t e;

                index->indexAt(0, b);
                index->indexAt(isize-1, e);

                // si le nuage d'index se trouve entre le début et la fin du nuage supprimé
                if((b >= beginIndex)
                        && (e <= endIndex))
                {
                    CT_AbstractModifiableCloudIndexT<T> *indexModif = cir->abstractModifiableCloudIndexT();

                    // on supprime tout les index du nuage puisqu'ils ne sont plus valides
                    indexModif->clear();
                }
                // sinon si le nuage d'index est à cheval sur le nuage supprimé
                else if((b <= endIndex)
                            && (e >= beginIndex))
                {
                    size_t i = 0;

                    // recherche de l'emplacement de l'index qui est supérieur ou égual à la valeur beginIndex
                    if(b < beginIndex)
                        i = index->lowerBound(beginIndex);

                    size_t j = isize;

                    // recherche de l'emplacement de l'index qui est supérieur à la valeur endIndex
                    if(e > endIndex)
                        j = index->upperBound(endIndex);

                    CT_AbstractModifiableCloudIndexT<T> *indexModif = cir->abstractModifiableCloudIndexT();

                    // on décale les index entre j et la fin et on supprime les éléments entre i et j
                    indexModif->eraseBetweenAndShiftRest(i, j-i, size, true);

                    // on décale tous les index qui sont après la fin du nuage supprimé
                    /*while(j < isize)
                    {
                        const ct_index_type &tmpIndex = indexModif->constIndexAt(j);
                        indexModif->replaceIndex(i, tmpIndex - size, false);

                        ++j;
                        ++i;
                    }

                    //
                    if(i < isize)
                        indexModif->erase(i, isize-i);*/
                }
                // sinon le nuage d'index est après le nuage supprimé
                else if(b >= endIndex)
                {
                    // il faut juste décaler tout les éléments
                    cir->abstractModifiableCloudIndexT()->shiftAll(size, true);
                }
            }
        }

        // on garde en mémoire les nuages qu'il faut enlever de l'enregistrement (puisqu'ils n'ont plus d'index)
        if(index->mustBeUnregisteredWhenIsEmpty() && (index->size() == 0))
            toUnregister.append(const_cast<CT_AbstractModifiableCloudIndexRegisteredT<T>*>(cir));
    }

    // on supprime les nuages qui n'ont plus d'index
    while(!toUnregister.isEmpty())
        CT_AbstractCloudIndexRegistrationManagerT<T>::unregisterIndex(toUnregister.takeFirst());
}

template<typename T, class CloudIndexLessMemory>
void CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::syncLessMemoryIndexRegistered(const size_t &beginIndex, const size_t &size)
{
    QList< CT_AbstractNotModifiableCloudIndexRegisteredT<T>* > toUnregister;

    // fin du nuage qui va être supprimé
    size_t endIndex = (beginIndex+size)-1;

    // on parcours la liste des index enregistré
    QListIterator< CT_AbstractNotModifiableCloudIndexRegisteredT<T>* > it(this->m_cirMergedArray);

    while(it.hasNext())
    {
        CT_AbstractNotModifiableCloudIndexRegisteredT<T> *cir = it.next();

        // on sait que c'est un CloudIndexLessMemory puisqu'on l'a créé dans mergeCloudContiguous(...)
        CloudIndexLessMemory *index = dynamic_cast<CloudIndexLessMemory*>(cir->abstractCloudIndexT());

        size_t isize = index->size();

        if(isize > 0)
        {
            size_t b;
            size_t e;

            index->indexAt(0, b);
            index->indexAt(isize-1, e);

            // si le nuage d'index se trouve entre le début et la fin du nuage supprimé
            if((b >= beginIndex)
                    && (e <= endIndex))
            {
                // le seul cas possible c'est si on a fait appel à la méthode mergeCloudContiguous() avec un seul nuage.
                // donc on doit supprimer ce nuage d'index.
                index->clear();
            }
            // sinon si le nuage d'index est plus grand que le nuage qu'on supprime
            else if((b <= beginIndex)
                        && (e >= endIndex))
            {
                if(!index->cut(beginIndex, endIndex))
                {
                    qFatal("erreur de synchronisation d'un nuage d'index de type less memory. contacter le développeur ! (methode cut)");
                }
            }
            // sinon le nuage d'index est après le nuage supprimé
            else if(b >= endIndex)
            {
                // il faut juste décaler tout les éléments
                index->shiftAll(size, true);
            }
            else
            {
                // impossible
                qFatal("erreur de synchronisation d'un nuage d'index de type less memory. contacter le développeur !");
            }
        }

        // on garde en mémoire les nuages qu'il faut enlever de l'enregistrement (puisqu'ils n'ont plus d'index)
        if(index->mustBeUnregisteredWhenIsEmpty() && (index->size() == 0))
            toUnregister.append(cir);
    }

    // on supprime les nuages qui n'ont plus d'index
    while(!toUnregister.isEmpty())
        CT_AbstractCloudIndexRegistrationManagerT<T>::unregisterIndex(toUnregister.takeFirst());
}

template<typename T, class CloudIndexLessMemory>
bool CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::staticFindIf(void *context, const size_t &value)
{
    return value >= static_cast<FindShiftRemoveContext*>(context)->beginIndex;
}

template<typename T, class CloudIndexLessMemory>
bool CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::staticShiftIf(void *context, const size_t &value)
{
    return value > static_cast<FindShiftRemoveContext*>(context)->endIndex;
}

template<typename T, class CloudIndexLessMemory>
bool CT_CloudIndexRegistrationManagerT<T, CloudIndexLessMemory>::staticRemoveIf(void *context, const size_t &value)
{
    return (value >= static_cast<FindShiftRemoveContext*>(context)->beginIndex) && (value <= static_cast<FindShiftRemoveContext*>(context)->endIndex);
}
