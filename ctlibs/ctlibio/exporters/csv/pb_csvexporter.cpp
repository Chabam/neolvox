#include "pb_csvexporter.h"

#include "pb_csvexporterconfiguration.h"
#include "views/exporters/csv/pbg_csvconfigurationdialog.h"
#include "pb_csvexportercolumn.h"

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"
#include "ct_log/ct_logmanager.h"

#include <QFile>
#include <QTextStream>
#include <QVector>
#include <limits>
#include <QFileInfo>

PB_CSVExporter::PB_CSVExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    _configuration = nullptr;

    addNewExportFormat(FileFormat("csv", tr("Fichiers csv")));

    setToolTip(tr("Exporte tous les attributs d'un niveau d'items (une ligne pour chaque instance d'item)"));
}

PB_CSVExporter::PB_CSVExporter(const PB_CSVExporter &other) : SuperClass(other)
{
    mItems = other.mItems;
    mItemsModels = other.mItemsModels;

    _configuration = ((other._configuration == nullptr) ? nullptr : new PB_CSVExporterConfiguration(*other._configuration));
}

PB_CSVExporter::~PB_CSVExporter()
{
    delete _configuration;
}

QString PB_CSVExporter::displayableName() const
{
    return tr("Export d'attributs (csv)");
}

void PB_CSVExporter::setItemsToExport(const QList<const CT_AbstractSingularItemDrawable*>& list)
{
    setMustUseModels(false);

    mItems = list;
}

bool PB_CSVExporter::configure()
{
    constructItemsModels();

    if(_configuration == nullptr)
        _configuration = new PB_CSVExporterConfiguration(mItemsModels);

    PBG_CSVConfigurationDialog dialog(*_configuration);

    const bool ret = (dialog.exec() == QDialog::Accepted) && (!_configuration->getColumns().isEmpty());

    return ret;
}

void PB_CSVExporter::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    if(_configuration == nullptr)
        return;

    const auto& columns = _configuration->getColumns();

    int i = 0;
    for(const auto& column : columns)
    {
        const int id = writer.addParameter(this, "Column", i++);
        writer.addParameterInfo(this, id, "ItemModelUniqueIndex", column.first->uniqueIndex());
        writer.addParameterInfo(this, id, "ItemAttributeModelUniqueIndex", column.second->uniqueIndex());
    }
}

bool PB_CSVExporter::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    mItemsModels.clear();
    constructItemsModels();

    delete _configuration;
    _configuration = nullptr;

    const int nColumns = reader.parameterCount(this, "Column");

    if((nColumns > 0) && mItemsModels.isEmpty())
    {
        if(!mustUseModels())
            PS_LOG->addErrorMessage(LogInterface::exporter, tr("La méthode \"setItemsToExport(...)\" n'a pas été appelée avant de restaurer la configuration de l'exporter CSV."));

        return false;
    }

    _configuration = new PB_CSVExporterConfiguration(mItemsModels);

    QVariant value;
    for(int i=0; i<nColumns; ++i)
    {
        const int id = reader.parameter(this, "Column", value);

        if((id <= 0) || (value.toInt() != i))
            return false;

        if(!reader.parameterInfo(this, id, "ItemModelUniqueIndex", value))
            return false;

        const CT_OutAbstractSingularItemModel* itemModel = itemModelByUniqueIndex(value.value<CT_OutAbstractModel::UniqueIndexType>());

        if(itemModel == nullptr)
            return false;

        if(!reader.parameterInfo(this, id, "ItemAttributeModelUniqueIndex", value))
            return false;

        const CT_OutAbstractItemAttributeModel* itemAttributeModel = itemAttributeModelByUniqueIndex(itemModel, value.value<CT_OutAbstractModel::UniqueIndexType>());

        if(itemAttributeModel == nullptr)
            return false;

        _configuration->addColumn(itemModel, itemAttributeModel);
    }

    return true;
}

void PB_CSVExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
}

CT_AbstractExporter::ExportReturn PB_CSVExporter::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString path = exportPathInfo.path();
    const QString baseName = exportPathInfo.baseName();
    const QString suffix = "csv";

    const QString currentFilePath = QString("%1/%2.%4").arg(path).arg(baseName).arg(suffix);

    QFile file(currentFilePath);

    if(file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream txtStream(&file);

        constructItemsToExport();

        const auto columns = writeHeader(txtStream);

        // ecriture des données
        int currentItemIndex = 0;
        int lastCurrentItemIndex;
        bool hasAtLeastOneNextValue;

        size_t completeSize = 0;
        size_t currentPos = 0;

        for(PB_CSVExporterColumn* column : columns)
            completeSize += column->size();

        const int lastColumn = columns.size()-1;

        do
        {
            txtStream << "\n";

            lastCurrentItemIndex = std::numeric_limits<int>::max();
            hasAtLeastOneNextValue = false;

            // pour chaque colonne
            int c = 0;
            for(PB_CSVExporterColumn* column : columns)
            {
                // si cette colonne va exporter une valeur
                // d'un item dont l'index dans la liste est le même
                // que les autres colonnes
                if((column->currentItemIndex() == currentItemIndex)
                        && column->hasNextValue())
                {
                    // on exporte la valeur
                    txtStream << column->nextValueToString();

                    if (c != lastColumn)
                        txtStream << ";";

                    // on garde en mémoire l'index de l'item
                    if(column->currentItemIndex() < lastCurrentItemIndex)
                        lastCurrentItemIndex = column->currentItemIndex();

                    // si il y aura encore une valeur a exporter
                    if(column->hasNextValue()
                            && !hasAtLeastOneNextValue)
                        hasAtLeastOneNextValue = true;

                    ++currentPos;
                    setExportProgress(int((currentPos*95)/completeSize));
                }
                else
                {
                    if (c != lastColumn)
                        txtStream << ";";
                }

                ++c;
            }

            currentItemIndex = lastCurrentItemIndex;

        // tant qu'il reste au moins une valeur à exporter
        }while(hasAtLeastOneNextValue);

        qDeleteAll(columns.begin(), columns.end());

        file.close();
        setExportProgress(100);

        if(mustUseModels())
            return (mIteratorItemBegin == mIteratorItemEnd) ? NoMoreItemToExport : ExportCanContinue;
    }

    return NoMoreItemToExport;
}

void PB_CSVExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_CSVExporter::clearAttributesClouds()
{
}

void PB_CSVExporter::constructItemsModels()
{
    if(mustUseModels())
    {
        QList<const CT_OutAbstractSingularItemModel*> list;

        auto itModels = m_hInItem.iterateSelectedOutputModels<CT_OutAbstractSingularItemModel>(m_handleResultExport);

        for(const CT_OutAbstractSingularItemModel* model : itModels)
        {
            if(!list.contains(model))
                list.append(model);
        }

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        if((list.size() != mItemsModels.size()) || !list.toSet().subtract(mItemsModels.toSet()).isEmpty())
#else
        if((list.size() != mItemsModels.size()) || !QSet<const CT_OutAbstractSingularItemModel*>(list.begin(), list.end()).subtract(QSet<const CT_OutAbstractSingularItemModel*>(mItemsModels.begin(), mItemsModels.end())).isEmpty())
#endif
        {
            delete _configuration;
            _configuration = nullptr;
            mItemsModels = list;
        }

        return;
    }

    QList<const CT_OutAbstractSingularItemModel*> list;

    for(const CT_AbstractSingularItemDrawable* item : mItems)
    {
        const CT_OutAbstractSingularItemModel* model = item->modelStaticT<CT_OutAbstractSingularItemModel>();

        if(!list.contains(model))
            list.append(model);
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    if((list.size() != mItemsModels.size()) || !list.toSet().subtract(mItemsModels.toSet()).isEmpty())
#else
    if((list.size() != mItemsModels.size()) || !QSet<const CT_OutAbstractSingularItemModel*>(list.begin(), list.end()).subtract(QSet<const CT_OutAbstractSingularItemModel*>(mItemsModels.begin(), mItemsModels.end())).isEmpty())
#endif
    {
        delete _configuration;
        _configuration = nullptr;
        mItemsModels = list;
    }
}

void PB_CSVExporter::constructItemsToExport()
{
    mItemsToExportByModel.clear();

    if(mustUseModels())
    {
        if(mIteratorItemBegin == mIteratorItemEnd)
        {
            auto iterator = m_hInItem.iterateInputs(m_handleResultExport);
            mIteratorItemBegin = iterator.begin();
            mIteratorItemEnd = iterator.end();
        }

        int nExported = 0;
        const int totalToExport = maximumItemToExportInFile(int(std::distance(mIteratorItemBegin, mIteratorItemEnd)));

        while((mIteratorItemBegin != mIteratorItemEnd)
              && (nExported < totalToExport))
        {
            const CT_AbstractSingularItemDrawable* item = *mIteratorItemBegin;

            mItemsToExportByModel.insert(item->modelStaticT<CT_OutAbstractSingularItemModel>(), item);

            ++nExported;
            ++mIteratorItemBegin;
        }
    }
    else
    {
        for(const CT_AbstractSingularItemDrawable* item : mItems)
        {
            mItemsToExportByModel.insert(item->modelStaticT<CT_OutAbstractSingularItemModel>(), item);
        }
    }
}

QList<PB_CSVExporterColumn*> PB_CSVExporter::writeHeader(QTextStream& stream)
{
    QList<PB_CSVExporterColumn*> csvColumns;

    const auto columns = _configuration->getColumns();
    const int end = columns.size()-1;

    int i = 0;
    for(const auto& column : columns)
    {
        const QString dispName = column.second->displayableName().remove(" ")
                .remove('\'')
                .remove('\"')
                .remove('\t')
                .remove(';')
                .remove(':')
                .remove(',')
                .remove('.');

        stream << dispName;

        if (i != end)
            stream << ";";

        csvColumns.append(new PB_CSVExporterColumn(mItemsToExportByModel.values(column.first), column.second));
        ++i;
    }

    return csvColumns;
}

const CT_OutAbstractSingularItemModel* PB_CSVExporter::itemModelByUniqueIndex(const CT_OutAbstractModel::UniqueIndexType& uid) const
{
    for(const auto model : mItemsModels)
    {
        if(model->uniqueIndex() == uid)
            return model;
    }

    return nullptr;
}

const CT_OutAbstractItemAttributeModel* PB_CSVExporter::itemAttributeModelByUniqueIndex(const CT_OutAbstractSingularItemModel* sItem, const CT_OutAbstractModel::UniqueIndexType& uid) const
{
    const CT_OutAbstractItemAttributeModel* model = nullptr;

    sItem->visitAttributes([&uid, &model](const CT_OutAbstractItemAttributeModel* modelToTest) -> bool
    {
        if(modelToTest->uniqueIndex() == uid)
        {
            model = modelToTest;
            return false;
        }

        return true;
    });

    return model;
}
