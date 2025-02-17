#include "lvox3_stepmergegrids.h"

//Tools
#include "ct_view/tools/ct_configurablewidgettodialog.h"
#include "datastructures/lvox3_implgrid3d.h"
#include "view/mergegridsconfiguration.h"
#include "ct_log/ct_logmanager.h"

#include <liblvox/algorithms/mergegrids.h>

//Workers

#include <QDialog>

LVOX3_StepMergeGrids::LVOX3_StepMergeGrids() :
    CT_AbstractStep()
{

}

QString LVOX3_StepMergeGrids::description() const
{
    return tr("4 - Merge grids from multiple scans (LVOX 3)");
}

// Step detailed description
QString LVOX3_StepMergeGrids::detailledDescription() const
{
    return tr("This step allows for merging grids created from multiple scans of a point cloud scene. A new version of the grids is generated.");
}

CT_VirtualAbstractStep *LVOX3_StepMergeGrids::createNewInstance() const
{
    return new LVOX3_StepMergeGrids();
}

/*
 * FIXME: should use a label instead of an enum int for the reducer type. If
 * the list of reducers is modified, then the setting will be invalid, but the
 * user will not get the configuration dialog.
 */

void LVOX3_StepMergeGrids::savePostSettings(SettingsWriterInterface &writer) const
{
    CT_AbstractStep::savePostSettings(writer);

    writer.addParameter(this, "effectiveRayThreshold", m_reducerOptions.effectiveRaysThreshold);
    writer.addParameter(this, "ignoreVoxelZeroDensity", m_reducerOptions.ignoreVoxelZeroDensity);
    writer.addParameter(this, "reducerLabel", QString::fromStdString(m_reducerOptions.reducerLabel));
    writer.addParameter(this, "isRelativeThreshold", m_reducerOptions.isRelativeThreshold);
}

bool LVOX3_StepMergeGrids::restorePostSettings(SettingsReaderInterface &reader)
{
    if(!CT_AbstractStep::restorePostSettings(reader))
        return false;

    QVariant value;

    reader.parameter(this, "effectiveRayThreshold", value); m_reducerOptions.effectiveRaysThreshold = value.toInt();
    reader.parameter(this, "ignoreVoxelZeroDensity", value); m_reducerOptions.ignoreVoxelZeroDensity = value.toBool();
    reader.parameter(this, "reducerLabel", value); m_reducerOptions.reducerLabel = value.toString().toStdString();
    reader.parameter(this, "isRelativeThreshold", value); m_reducerOptions.isRelativeThreshold = value.toBool();

    return lvox::MergeGrids::doReducerExists(m_reducerOptions.reducerLabel);
}

QString LVOX3_StepMergeGrids::parametersDescription()
{
    return CT_AbstractStep::parametersDescription() +
            "<strong>" + tr("Step parameters:") + "</strong>" +
            "<ul>" +
                "<li>" + tr("Merging method") + ": " + tr("String") + " <em>" + tr("with the following possible values") + "</em> = " +
                    "<ul>" +
                        "<li>" + tr("Max density") + " : " + tr("Compute with ") + " <em>" + "max(nbi)" + "</em>" + "</li>" +
                        "<li>" + tr("Max trust") + " : " + tr("Compute with ") + " <em>" + "max(nt - nb)" + "</em>" + "</li>" +
                        "<li>" + tr("Max trust ratio") + " : " + tr("Compute with ") + " <em>" + "max((nt - nb)/nt)" + "</em>" + "</li>" +
                        "<li>" + tr("Max hits") + " : " + tr("Compute with ") + " <em>" + "max(ni)" + "</em>" + "</li>" +
                        "<li>" + tr("Sum ratio") + " : " + tr("Compute with ") + " <em>" + "sum(ni)/sum(nt - nb)" + "</em>" + "</li>" +
                        "<li>" + tr("Weighted rdi") + " : " + tr("Compute with ") + " <em>" + "sum((nt - nb) * rdi)/sum(nt - nb)" + "</em>" + "</li>" +
                        "<li>" + tr("Weighted rdi alt.") + " : " + tr("Compute with ") + " <em>" + "sum((nt - nb) / (nt * rdi)) / sum((nt - nb) / nt)" + "</em>" + "</li>" +
                    "</ul>"+
                "</li>" +
                "<li>" + tr("Ignore zero density voxel") + ": " + tr("boolean") + "<em>" + tr("default") + "</em> = " + tr("true") + "</li>" +
                "<li>" + tr("Minimum effective rays") + ": " + tr("integer") + "<em>" + tr("default") + "</em> = " + "5 " + tr("applies whenever using (nt - nb)") + "</li>" +
                "<li>" + tr("Use relative threshold") + ": " + tr("boolean") + "<em>" + tr("default") + "</em> = " + tr("false") + " " + tr("applies whenever using (nt - nb)") + "</li>" +
            "</ul>";
}

QString LVOX3_StepMergeGrids::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Root group [Group]:" +
            indent("LVOX3_Grid3D<int32> [Theoretical (merged)]") +
            indent("LVOX3_Grid3D<int32> [Hits (merged)]") +
            indent("LVOX3_Grid3D<int32> [Before (merged)]")+
            indent("LVOX3_Grid3D<float> [Density (merged)]");

}


void LVOX3_StepMergeGrids::declareInputModels(CT_StepInModelStructureManager& manager)
{
    /*
     * Requirements:
     *  - relative density 3D grid
     *  - theoretical rays 3D grid
     *  - blocked rays 3D grid
     *  - incident rays 3D grid
     */

    manager.addResult(_inResult, tr("Grids"));
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);

    manager.addItem(_inGroup, _inGrid_ni, "Hits");
    manager.addItem(_inGroup, _inGrid_nt, "Theoretical");
    manager.addItem(_inGroup, _inGrid_nb, "Before");
    manager.addItem(_inGroup, _inGrid_density, "Density grid");
}

bool LVOX3_StepMergeGrids::postInputConfigure()
{
    MergeGridsConfiguration widget;
    widget.setOptions(m_reducerOptions);

    if(CT_ConfigurableWidgetToDialog::exec(&widget) == QDialog::Rejected)
        return false;

    m_reducerOptions = widget.getOptions();

    // qDebug() << "reducer options:"
    //          << m_reducerOptions.reducerLabel
    //          << m_reducerOptions.effectiveRaysThreshold
    //          << m_reducerOptions.ignoreVoxelZeroDensity;

    setSettingsModified(true);
    return true;
}


void LVOX3_StepMergeGrids::declareOutputModels(CT_StepOutModelStructureManager& manager)
{    
    manager.addResult(_outResult, tr("Merged grids"));
    manager.setRootGroup(_outResult, _outRootGroup);
    manager.addItem(_outRootGroup, _outGrid_ni, "Hits (merged)");
    manager.addItem(_outRootGroup, _outGrid_nt, "Theoretical (merged)");
    manager.addItem(_outRootGroup, _outGrid_nb, "Before (merged)");
    manager.addItem(_outRootGroup, _out_Density, "Density (merged)");
}

void LVOX3_StepMergeGrids::compute()
{
    lvox::MergeGrids::Settings settings;
    settings.Method = lvox::MergeGrids::getMethodFromString(m_reducerOptions.reducerLabel);
    qDebug() << "Adding grids";
    for (CT_ResultGroup* result : _outResult.iterateOutputs())
    {
        CT_StandardItemGroup* rootGroup = _outRootGroup.createInstance();
        result->addRootGroup(_outRootGroup, rootGroup);

        std::vector<Grid3DSet> gss;

        // QVector<LVOXGridSet*> gs; // holds the sets of each perspective
        // qDebug() << "Step 1 : ";
        for (const CT_StandardItemGroup* group : _inGroup.iterateInputs(_inResult))
        {
            Grid3DSet gs;

            pluginlvox::Grid3Df* grid_density = const_cast<pluginlvox::Grid3Df*>(group->singularItem(_inGrid_density));
            pluginlvox::Grid3Di* grid_nt = const_cast<pluginlvox::Grid3Di*>(group->singularItem(_inGrid_nt));
            pluginlvox::Grid3Di* grid_nb = const_cast<pluginlvox::Grid3Di*>(group->singularItem(_inGrid_nb));
            pluginlvox::Grid3Di* grid_ni = const_cast<pluginlvox::Grid3Di*>(group->singularItem(_inGrid_ni));

            LVOX3_ImplGrid3d<int> * i_grid_nt = new LVOX3_ImplGrid3d<int>(grid_nt);
            LVOX3_ImplGrid3d<int> * i_grid_nb = new LVOX3_ImplGrid3d<int>(grid_nb);
            LVOX3_ImplGrid3d<int> * i_grid_ni = new LVOX3_ImplGrid3d<int>(grid_ni);
            LVOX3_ImplGrid3d<float>* i_grid_density = new LVOX3_ImplGrid3d<float>(grid_density);

            gs.Nb = i_grid_nb;
            gs.Ni = i_grid_ni;
            gs.Nt = i_grid_nt;
            gs.Density = i_grid_density;


            //Each set is passed together for ease of iteration
            gss.push_back(gs);

        }
        // qDebug() << "Step 2 : ";

        if (gss.empty())
        {
            PS_LOG->addErrorMessage(LogInterface::step, "No input grids");
            return;
        }

        qDebug() << "Merged grid";


        Grid3DSet a = gss.front(); //first set is put in merged to either use as comparison or as a base for increment

        Grid3DSet merged;
        pluginlvox::Grid3Di* grid_nt = new pluginlvox::Grid3Di(a.Nt->getMinX(), a.Nt->getMinY(), a.Nt->getMinZ(), a.Nt->getDimensionX(), a.Nt->getDimensionY(), a.Nt->getDimensionZ(), a.Nt->getResolution(), a.Nt->getResolution(), a.Nt->getResolution(), a.Nt->getNAValue(), a.Nt->getNAValue());
        pluginlvox::Grid3Di* grid_nb = new pluginlvox::Grid3Di(a.Nb->getMinX(), a.Nb->getMinY(), a.Nb->getMinZ(), a.Nb->getDimensionX(), a.Nb->getDimensionY(), a.Nb->getDimensionZ(), a.Nb->getResolution(), a.Nb->getResolution(), a.Nb->getResolution(), a.Nb->getNAValue(), a.Nb->getNAValue());
        pluginlvox::Grid3Di* grid_ni = new pluginlvox::Grid3Di(a.Ni->getMinX(), a.Ni->getMinY(), a.Ni->getMinZ(), a.Ni->getDimensionX(), a.Ni->getDimensionY(), a.Ni->getDimensionZ(), a.Ni->getResolution(), a.Ni->getResolution(), a.Ni->getResolution(), a.Ni->getNAValue(), a.Ni->getNAValue());
        pluginlvox::Grid3Df* grid_density = new pluginlvox::Grid3Df(a.Density->getMinX(), a.Density->getMinY(), a.Density->getMinZ(), a.Density->getDimensionX(), a.Density->getDimensionY(), a.Density->getDimensionZ(), a.Density->getResolution(), a.Density->getResolution(), a.Density->getResolution(), a.Density->getNAValue(), a.Density->getNAValue());

        LVOX3_ImplGrid3d<int>* i_grid_nt = new LVOX3_ImplGrid3d<int>(grid_nt);
        LVOX3_ImplGrid3d<int>* i_grid_nb = new LVOX3_ImplGrid3d<int>(grid_nb);
        LVOX3_ImplGrid3d<int>* i_grid_ni = new LVOX3_ImplGrid3d<int>(grid_ni);
        LVOX3_ImplGrid3d<float>* i_grid_density = new LVOX3_ImplGrid3d<float>(grid_density);

        merged.Ni = i_grid_nt;
        merged.Nt = i_grid_nb;
        merged.Nb = i_grid_ni;
        merged.Density = i_grid_density;

        qDebug() << "Merge grid compute";
        lvox::MergeGrids algo(gss, merged, settings);
        algo.compute();

        // qDebug() << "Step 3 : ";
        //Prerequisites for visualizing


        rootGroup->addSingularItem(_outGrid_ni, grid_nt);
        rootGroup->addSingularItem(_outGrid_nt, grid_nb);
        rootGroup->addSingularItem(_outGrid_nb, grid_ni);
        rootGroup->addSingularItem(_out_Density, grid_density);

        grid_nt->computeMinMax();
        grid_nb->computeMinMax();
        grid_ni->computeMinMax();
        grid_density->computeMinMax();
    }
}

void LVOX3_StepMergeGrids::workerProgressChanged(int p)
{
    setProgress(p);
}

