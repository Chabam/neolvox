#include "ct_cloud/tools/ct_globalcloudmanagert.h"


template<typename T, typename CLOUD>
CT_GlobalCloudManagerT<T, CLOUD>::CT_GlobalCloudManagerT() : CT_AbstractGlobalCloudManagerT<T>()
{
    m_nCallToMultipleDelete = 0;
}

template<typename T, typename CLOUD>
template< class IndexMemoryOptim, class IndexAccessOptim >
typename CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractNotModifiableCIR CT_GlobalCloudManagerT<T, CLOUD>::createNewCloud(size_t size, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim)
{
    size_t beginIndex = m_cloud.size();

    // redimensionne le nuage de points global
    m_cloud.resize(m_cloud.size() + size);

    // retourne un nuage d'index enregistré
    return createCIR<IndexMemoryOptim, IndexAccessOptim>(beginIndex, size, optim);
}

template<typename T, typename CLOUD>
template< class IndexMemoryOptim, class IndexAccessOptim >
typename CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractNotModifiableCIR CT_GlobalCloudManagerT<T, CLOUD>::copyCloud(typename CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractCIR cir, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim)
{
    size_t beginIndex = m_cloud.size();
    size_t size = cir->size();

    // redimensionne le nuage de points global pour ajouter les nouveaux points
    m_cloud.resize(beginIndex + size);

    // crée un nuage de points enregistré (il contient le nuage de points global et le nuage d'index)
    CT_AbstractNotModifiableCIR newCir = createCIR<IndexMemoryOptim, IndexAccessOptim>(beginIndex, size, optim);

    // puis on recopie les points

    // si c'est un nuage non modifiable
    const CT_AbstractNotModifiableCloudIndexRegisteredT<T> *cirNotModifiable = dynamic_cast< const CT_AbstractNotModifiableCloudIndexRegisteredT<T>* >(cir.data());

    if(cirNotModifiable != nullptr)
    {
        // celui-ci contient juste le début et la taille du nuage de points

        // dont on peut recopier les données plus rapidement
        // internalCopyData(index de destination, index de source, taille)
        m_cloud.internalCopyData(beginIndex, cirNotModifiable->first(), size);
    }
    // sinon
    else
    {
        const CT_AbstractCloudIndexT<T> *sourceIndex = cir->abstractCloudIndexT();

        // on boucle sur les index pour la copie
        size_t p = beginIndex;

        for(size_t i=0; i<size; ++i)
        {
            m_cloud[p] = m_cloud[(*sourceIndex)[i]];

            ++p;
        }
    }

    return newCir;
}

template<typename T, typename CLOUD>
template< class IndexMemoryOptim, class IndexAccessOptim >
typename CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractNotModifiableCIR CT_GlobalCloudManagerT<T, CLOUD>::copyCloud(const CT_AbstractCloudIndexT<T> *index, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim)
{
    size_t beginIndex = m_cloud.size();
    size_t size = index->size();

    // redimensionne le nuage de points global pour ajouter les nouveaux points
    m_cloud.resize(beginIndex + size);

    // crée un nuage de points enregistré (il contient le nuage de points global et le nuage d'index)
    CT_AbstractNotModifiableCIR newCir = createCIR<IndexMemoryOptim, IndexAccessOptim>(beginIndex, size, optim);

    // puis on recopie les points

    // on boucle sur les index pour la copie
    size_t p = beginIndex;

    for(size_t i=0; i<size; ++i)
    {
        m_cloud[p] = m_cloud[(*index)[i]];

        ++p;
    }

    return newCir;
}

template<typename T, typename CLOUD>
typename CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractNotModifiableCIR CT_GlobalCloudManagerT<T, CLOUD>::resizeCloudAndCloudIndex(typename CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractNotModifiableCIR cir, const size_t &newSize)
{
    size_t s = newSize - cir->abstractCloudIndexT()->size();

    if(!this->m_cirArray.contains(cir.data())
            || (cir->last() != (m_cloud.size()-1)))
        return CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractNotModifiableCIR(nullptr);

    cir->abstractCloudIndexT()->resize(newSize);

    // informe les gestionnaire de synchronisation de nuage qu'un nouveau nuage a été ajouté
    this->informThatCloudAdded(s);

    m_cloud.resize(m_cloud.size() + s);

    return cir;
}

template<typename T, typename CLOUD>
CT_AbstractCloud* CT_GlobalCloudManagerT<T, CLOUD>::globalAbstractCloud() const
{
    return (CT_AbstractCloud*)&m_cloud;
}

template<typename T, typename CLOUD>
CT_AbstractCloudT<T>* CT_GlobalCloudManagerT<T, CLOUD>::globalAbstractCloudT() const
{
    return (CT_AbstractCloudT<T>*)&m_cloud;
}

template<typename T, typename CLOUD>
CLOUD* CT_GlobalCloudManagerT<T, CLOUD>::globalCloud() const
{
    return (CLOUD*)&m_cloud;
}

template<typename T, typename CLOUD>
void CT_GlobalCloudManagerT<T, CLOUD>::beginMultiDelete()
{
    ++m_nCallToMultipleDelete;
}

template<typename T, typename CLOUD>
void CT_GlobalCloudManagerT<T, CLOUD>::endMultiDelete()
{
    --m_nCallToMultipleDelete;

    if(m_nCallToMultipleDelete == 0)
        deleteMultipleCloud();
}

template<typename T, typename CLOUD>
void CT_GlobalCloudManagerT<T, CLOUD>::resizeCloud(const size_t &newSize)
{
    m_cloud.resize(newSize);
}

template<typename T, typename CLOUD>
template< class IndexMemoryOptim, class IndexAccessOptim >
typename CT_GlobalCloudManagerT<T, CLOUD>::CT_AbstractNotModifiableCIR CT_GlobalCloudManagerT<T, CLOUD>::createCIR(const size_t &beginIndex, const size_t &size, typename CT_AbstractGlobalCloudManagerT<T>::IndexOptimization optim)
{
    // informe les gestionnaire de synchronisation de nuage qu'un nouveau nuage a été ajouté
    this->informThatCloudAdded(size);

    CT_AbstractNotModifiableCloudIndexRegisteredT<T> *cir = nullptr;

    if(optim == CT_AbstractGlobalCloudManagerT<T>::MemoryOptimized)
    {
        cir = this->template createNewIndexRegistered< CT_StandardNotModifiableCloudIndexRegisteredT<T>, IndexMemoryOptim >(new IndexMemoryOptim(beginIndex, size));
    }
    else
    {       
        IndexAccessOptim *nIndex = new IndexAccessOptim(size);

        std::vector< ct_index_type > *data = this->template getInternalDataOfIndex< IndexAccessOptim >(nIndex);

        ct_index_type a = ct_index_type(beginIndex);

        for(size_t i=0; i<size; ++i)
        {
            (*data)[i] = a;
            ++a;
        }

        cir = this->template createNewIndexRegistered< CT_StandardNotModifiableCloudIndexRegisteredT<T>, IndexAccessOptim >(nIndex);
    }

    this->m_cirArray.append(cir);

    return CT_AbstractNotModifiableCIR(cir, &CT_GlobalCloudManagerT<T, CLOUD>::staticDeleteCIR);
}

template<typename T, typename CLOUD>
void CT_GlobalCloudManagerT<T, CLOUD>::staticDeleteCIR(CT_AbstractNotModifiableCloudIndexRegisteredT<T> *cir)
{
    CT_GlobalCloudManagerT<T, CLOUD> *thisPtr = dynamic_cast<CT_GlobalCloudManagerT<T, CLOUD>*>(PS_REPOSITORY->globalCloudManager<T>());

    thisPtr->internalUnregisterIndex(cir, true, true);
}

template<typename T, typename CLOUD>
bool CT_GlobalCloudManagerT<T, CLOUD>::internalUnregisterIndex(CT_AbstractCloudIndexRegisteredT<T> *cir,
                                                               bool informCloudDeleted,
                                                               bool deleteCIR)
{
    if(m_nCallToMultipleDelete == 0)
    {
        int at = this->m_cirArray.indexOf(cir);

        if(at < 0)
        {
            if(deleteCIR)
                delete cir;

            return false;
        }

        const CT_AbstractCloudIndexT<T> *index = cir->abstractCloudIndexT();

        size_t beginIndex = index->first();
        size_t size = index->size();

        // on décale les nuages de points qui sont après le nuage supprimé (ils ne sont pas décalé
        // automatiquement puisqu'ils ne sont pas modifiables et non enregistré auprès du gestionnaire
        // de nuages d'index)
        this->shiftAllCloudIndexFrom(at+1, size, true);

        if(informCloudDeleted)
            this->informThatCloudWillBeDeleted(beginIndex, size);

        // on informe le gestionnaire d'index qu'on va supprimer le nuage afin qu'il
        // synchronise les nuages d'index.
        // et on informe les gestionnaire de synchronisation de nuages qu'on va supprimer le nuage
        if(informCloudDeleted)
            this->informThatCloudDeleted(beginIndex, size);

        // on supprime de la mémoire le nuage de points.
        // pour cela on decale le reste du nuage de points vers la gauche et on supprime la fin
        this->m_cloud.erase(beginIndex, size);

        this->m_cirArray.removeAt(at);
        this->m_cirArrayUnsync.removeOne(cir);

        if(deleteCIR)
            delete cir;
    }
    else
    {
        int at = this->m_cirArray.indexOf(cir);

        if(at < 0)
        {
            if(deleteCIR)
                delete cir;

            return false;
        }

        const CT_AbstractCloudIndexT<T> *index = cir->abstractCloudIndexT();

        CT_GlobalCloudIndex cloudIndex;
        cloudIndex.m_begin = index->first();
        cloudIndex.m_end = index->last();

        m_multipleDeleteCollection.push_back(cloudIndex);

        this->m_cirArray.removeAt(at);
        this->m_cirArrayUnsync.removeOne(cir);

        if(deleteCIR)
            delete cir;
    }

    return true;
}

template<typename T, typename CLOUD>
void CT_GlobalCloudManagerT<T, CLOUD>::deleteMultipleCloud()
{
    if(!m_multipleDeleteCollection.empty())
    {
        m_multipleDeleteCollection.sort();

        std::list< CT_GlobalCloudIndex >::iterator it = m_multipleDeleteCollection.begin();
        std::list< CT_GlobalCloudIndex >::iterator next = it;
        ++next;
        std::list< CT_GlobalCloudIndex >::iterator end = m_multipleDeleteCollection.end();

        // merge all index in the list
        while(next != end)
        {
            CT_GlobalCloudIndex &a = *it;
            CT_GlobalCloudIndex &b= *next;

            if((a.m_end+1) == b.m_begin)
            {
                a.m_end = b.m_end;
                next = this->m_multipleDeleteCollection.erase(next);

                end = this->m_multipleDeleteCollection.end();
            }
            else
            {
                ++it;
                ++next;
            }
        }

        it = this->m_multipleDeleteCollection.begin();
        end = this->m_multipleDeleteCollection.end();

        while(it != end)
        {
            CT_GlobalCloudIndex &a = *it;

            size_t beginIndex = a.m_begin;
            size_t size = (a.m_end - a.m_begin)+1;

            next = it;
            ++next;

            // shift all index after this
            while(next != end)
            {
                CT_GlobalCloudIndex &b = *next;

                b.m_begin -= size;
                b.m_end -= size;

                ++next;
            }

            // search CIR after this index
            int at = -1;
            int i = 0;

            QListIterator< CT_AbstractCloudIndexRegisteredT<T>* > cirIt(this->m_cirArray);

            while(cirIt.hasNext()
                  && (at < 0))
            {
                CT_AbstractCloudIndexRegisteredT<T> *cir = cirIt.next();

                if(cir->first() > a.m_end)
                    at = i;

                ++i;
            }

            // on informe les autres gestionnaires de nuages qu'on va supprimer un nuage
            this->informThatCloudWillBeDeleted(beginIndex, size);

            // on décale les nuages d'objets qui sont après le nuage supprimé (ils ne sont pas décalé
            // automatiquement puisqu'ils ne sont pas modifiables et non enregistré auprès du gestionnaire
            // de nuages d'index)
            if(at >= 0)
                this->shiftAllCloudIndexFrom(at, size, true);

            // inform all other manager that we have shifted all index that was after the cloud that we will delete
            this->informThatCloudIndexShiftedAfterThisCloud(beginIndex, size);

            // on supprime de la mémoire le nuage, pour cela on decale le reste
            // du nuage vers la gauche et on supprime la fin
            this->m_cloud.erase(beginIndex, size);

            // on informe les autres gestionnaires de nuages qu'on a enfin supprimé le nuage
            this->informThatCloudDeleted(beginIndex, size);

            ++it;
        }

        this->m_multipleDeleteCollection.clear();
    }
}
