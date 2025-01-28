#include "pb_groupdataexporter.h"

#include "ct_model/outModel/abstract/def_ct_abstractitemdrawablemodelout.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

PB_GroupDataExporter::PB_GroupDataExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("txt", tr("Fichier txt")));

    setToolTip(tr("Exporte tous les attributs de TOUS LES ITEMS d'un niveau ou plusieurs niveaux de groupes.<br>"
                  "Un fichier table ASCII est produit pour chaque niveau de groupe séléctionné.<br>"
                  "Dans chaque table, il y a une ligne par instance de groupe.<br>"
                  "Dans chaque table :<br>"
                  "- une première ligne d'entête donne le nom des items contenus dans le groupe,<br>"
                  "- une seconde  ligne d'entête donne le nom de l'attribut dans l'item.<br>"
                  "De plus les deux premières colonnes de chaque table permettent de faire des jointures entre niveaux de groupes :<br>"
                  "- GroupID  : identifiant Computree des groupes décris dans la table en cours,<br>"
                  "- ParentID : identifiant Computree des groupes parents, contenant les groupes de la table."));
}

PB_GroupDataExporter::PB_GroupDataExporter(const PB_GroupDataExporter& other) : SuperClass(other),
    mGroups(other.mGroups)
{
}

QString PB_GroupDataExporter::displayableName() const
{
    return tr("Export des attributs des items d'un groupe");
}

void PB_GroupDataExporter::setGroupsToExport(const QList<const CT_StandardItemGroup*>& list)
{
    setMustUseModels(false);

    mGroups = list;
}

void PB_GroupDataExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup, tr("Groupe à exporter"));
}

CT_AbstractExporter::ExportReturn PB_GroupDataExporter::internalExportToFile()
{

    // Création d'une map de référence des modèles de groupes / modèles d'items / dataRéférences
    // QMap<Modèle de groupe, QMapMultiMap<Modèle d'item, dataRéférence> >
    // + une map listant les groupes par modèle
    // + une map pour les headers de fichiers (un par modèle de groupe)
    QMap<const CT_OutAbstractGroupModel*, QMultiMap<const CT_OutAbstractSingularItemModel*, const CT_OutAbstractItemAttributeModel*> > itemAttModelIndex;
    QMultiMap<const CT_OutAbstractGroupModel*, const CT_StandardItemGroup*> groupMap;

    const auto addToMap = [&itemAttModelIndex, &groupMap](const CT_StandardItemGroup* group)
    {
        const DEF_CT_AbstractGroupModelOut* model = group->modelStaticT<DEF_CT_AbstractGroupModelOut>();

        groupMap.insert(model, group);
        auto& itemMap = itemAttModelIndex[model];

        if(itemMap.isEmpty())
        {
            model->visitItems([&itemMap](const CT_OutAbstractSingularItemModel* itemModel) -> bool
            {
                itemModel->visitAttributes([&itemMap, &itemModel](const CT_OutAbstractItemAttributeModel* attModel) -> bool
                {
                    itemMap.insert(itemModel, attModel);
                    return true;
                });

                return true;
            });
        }
    };

    if(mustUseModels())
    {
        if(mIteratorGroupBegin == mIteratorGroupEnd)
        {
            auto iterator = m_hInGroup.iterateInputs(m_handleResultExport);
            mIteratorGroupBegin = iterator.begin();
            mIteratorGroupEnd = iterator.end();
        }

        int nExported = 0;
        const int totalToExport = maximumItemToExportInFile(int(std::distance(mIteratorGroupBegin, mIteratorGroupEnd)));

        // write data
        while((mIteratorGroupBegin != mIteratorGroupEnd)
              && (nExported < totalToExport))
        {
            const CT_StandardItemGroup* group = *mIteratorGroupBegin;

            addToMap(group);

            ++nExported;
            ++mIteratorGroupBegin;
        }
    }
    else
    {
        for(const CT_StandardItemGroup* group : mGroups)
        {
            addToMap(group);
        }
    }

    // Variables recyclées
    bool ok;
    bool exportCompleted = true;

    QFileInfo fileInfo = QFileInfo(filePath());
    QString basePath = QString("%1/%2").arg(fileInfo.absolutePath()).arg(fileInfo.baseName());
    QString suffix = ".txt";

    const bool multiFiles = (itemAttModelIndex.keys().size() > 1);

    int groupModelRank = 1;

    const int nGroupModel = itemAttModelIndex.size();
    int currentModelIndex = 0;

    // Parcours des modèles de groupes : pour chacun un fichier de sortie
    QMapIterator<const CT_OutAbstractGroupModel*, QMultiMap<const CT_OutAbstractSingularItemModel*, const CT_OutAbstractItemAttributeModel*> > it_itemAttModelIndex(itemAttModelIndex);
    while (it_itemAttModelIndex.hasNext())
    {
        if(isStopped())
            break;

        it_itemAttModelIndex.next();

        // Modèle de groupe
        const CT_OutAbstractGroupModel *groupModel = it_itemAttModelIndex.key();
        const QMultiMap<const CT_OutAbstractSingularItemModel*, const CT_OutAbstractItemAttributeModel*> &itemMap = it_itemAttModelIndex.value();

        if (multiFiles)
            suffix = QString("_%1_%2.txt").arg(groupModelRank++).arg(groupModel->displayableName());

        QFile file(QString("%1%2").arg(basePath).arg(suffix));

        // Ouverture du fichier d'export correspondant au modèle de groupe en cours
        if(file.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream txtStream(&file);

            // Création du header
            QString header_line1, header_line2;

            header_line1.append("GroupID\t");
            header_line2.append("GroupID\t");
            header_line1.append("ParentID\t");
            header_line2.append("ParentID\t");

            // Liste des modèles d'items du modèle de groupe
            const QList<const CT_OutAbstractSingularItemModel*> itemModelList = itemMap.uniqueKeys();

            for(const CT_OutAbstractSingularItemModel* itemModel : itemModelList)
            {
                // Liste des attributs du modèle d'item en cours
                const QList<const CT_OutAbstractItemAttributeModel*> itemAttModelList = itemMap.values(itemModel);

                for(const CT_OutAbstractItemAttributeModel* itemAttModel : itemAttModelList)
                {
                    header_line1.append(itemModel->displayableName());
                    header_line1.append("\t");

                    header_line2.append(itemAttModel->displayableName());
                    header_line2.append("\t");
                }
            }
            txtStream << header_line1 << "\n";
            txtStream << header_line2 << "\n";


            // Liste des groupes correspondant au modèle de groupe en cours
            const QList<const CT_StandardItemGroup*> groupListForThisModel = groupMap.values(groupModel);

            const int nGroupForThisModel = groupListForThisModel.size();
            int currentGroupIndex = 0;

            for(const CT_StandardItemGroup* group : groupListForThisModel)
            {
                if(isStopped())
                    break;

                txtStream << group->id() << "\t";

                const CT_StandardItemGroup* parentGroup = group->parentGroup();

                if (parentGroup != nullptr)
                    txtStream << parentGroup->id() << "\t";
                else
                    txtStream << "\t";

                // Liste des modèles d'items du modèle de groupe
                const QList<const CT_OutAbstractSingularItemModel*> itemModelList = itemMap.uniqueKeys();

                for(const CT_OutAbstractSingularItemModel* itemModel : itemModelList)
                {
                    // Liste des modèles des attributs d'item du modèle d'item en cours
                    const QList<const CT_OutAbstractItemAttributeModel*> itemAttModelList = itemMap.values(itemModel);

                    // Récupération de l'item correspondant au modèle d'item
                    CT_AbstractSingularItemDrawable* item = group->singularItemWithOutModel(dynamic_cast<const DEF_CT_AbstractItemDrawableModelOut*>(itemModel));

                    for(const CT_OutAbstractItemAttributeModel* itemAttModel : itemAttModelList)
                    {
                        // Si l'item existe, on exporte les attributs
                        if (item != nullptr)
                        {
                            CT_AbstractItemAttribute* att = item->itemAttributeWithOutModel(itemAttModel);
                            //if (att == NULL) {att = item->itemAttribute((CT_OutAbstractItemAttributeModel*) itemAttModel->originalModel());}

                            if(att != nullptr)
                            {
                                const QString value = att->toString(item, &ok);
                                if (ok)
                                    txtStream << value;
                            }
                        }
                        // Dans tous les cas on change de colonne
                        txtStream << "\t";
                    }
                }

                // Après chaque groupe on saute une ligne
                txtStream << "\n";

                ++currentGroupIndex;
                setExportProgress(((currentGroupIndex*100)/nGroupForThisModel)/nGroupModel + currentModelIndex);
            }

            // On ferme le fichier pour ce modèle de groupe
            file.close();
        }
        else // Si au moins un fichier a raté => renvoie false
        {
            exportCompleted = false;
        }

        currentModelIndex += 100;
    }

    if(!exportCompleted)
        return ErrorWhenExport;

    if(mustUseModels())
        return (mIteratorGroupBegin == mIteratorGroupEnd) ? NoMoreItemToExport : ExportCanContinue;

    return NoMoreItemToExport;
}

void PB_GroupDataExporter::clearIterators()
{
    mIteratorGroupBegin = HandleGroupType::const_iterator();
    mIteratorGroupEnd = mIteratorGroupBegin;
}

void PB_GroupDataExporter::clearAttributesClouds()
{
}
