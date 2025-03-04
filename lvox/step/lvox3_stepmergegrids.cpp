#include "lvox3_stepmergegrids.h"

//Tools
#include "ct_view/tools/ct_configurablewidgettodialog.h"
#include "view/mergegridsconfiguration.h"
#include "ct_log/ct_logmanager.h"

//Workers
#include "tools/worker/lvox3_mergegridsworker.h"

#include "tools/lvox3_utils.h"

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
    writer.addParameter(this, "reducerLabel", m_reducerOptions.reducerLabel);
    writer.addParameter(this, "isRelativeThreshold", m_reducerOptions.isRelativeThreshold);
}

bool LVOX3_StepMergeGrids::restorePostSettings(SettingsReaderInterface &reader)
{
    if(!CT_AbstractStep::restorePostSettings(reader))
        return false;

    QVariant value;

    reader.parameter(this, "effectiveRayThreshold", value); m_reducerOptions.effectiveRaysThreshold = value.toInt();
    reader.parameter(this, "ignoreVoxelZeroDensity", value); m_reducerOptions.ignoreVoxelZeroDensity = value.toBool();
    reader.parameter(this, "reducerLabel", value); m_reducerOptions.reducerLabel = value.toString();
    reader.parameter(this, "isRelativeThreshold", value); m_reducerOptions.isRelativeThreshold = value.toBool();

    LVOX3_MergeGrids mg;
    std::unique_ptr<VoxelReducer> vr = mg.makeReducer(m_reducerOptions.reducerLabel);
    if (!vr) {
        return false;
    }

    return true;
}

QString LVOX3_StepMergeGrids::parametersDescription(bool defaultValue)
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
    for (CT_ResultGroup* result : _outResult.iterateOutputs()) {

        CT_StandardItemGroup* rootGroup = _outRootGroup.createInstance();
        result->addRootGroup(_outRootGroup, rootGroup);

        // qDebug() << "Step 0 : ";
        LVOX3_MergeGrids mg;
        std::unique_ptr<VoxelReducer> reducer = mg.makeReducer(m_reducerOptions.reducerLabel);
        if (!reducer) {
            PS_LOG->addFatalMessage(LogInterface::step, "Cannot find the requested grid reducer " + m_reducerOptions.reducerLabel);
            return;
        }

        QVector<LVOXGridSet*> gs; // holds the sets of each perspective
        // qDebug() << "Step 1 : ";
        for (const CT_StandardItemGroup* group : _inGroup.iterateInputs(_inResult))
        {
            if (isStopped()) {return;}

            lvox::Grid3Df* grid_density = const_cast<lvox::Grid3Df*>(group->singularItem(_inGrid_density));
            lvox::Grid3Di* grid_nt = const_cast<lvox::Grid3Di*>(group->singularItem(_inGrid_nt));
            lvox::Grid3Di* grid_nb = const_cast<lvox::Grid3Di*>(group->singularItem(_inGrid_nb));
            lvox::Grid3Di* grid_ni = const_cast<lvox::Grid3Di*>(group->singularItem(_inGrid_ni));

            if(!(grid_density && grid_nt && grid_nb && grid_ni))
                continue;

            //Each set is passed together for ease of iteration
            gs.append(new LVOXGridSet{grid_ni, grid_nt, grid_nb, grid_density});

        }
        // qDebug() << "Step 2 : ";

        if (gs.isEmpty())
            return;

        LVOXGridSet *a = gs.first(); //first set is put in merged to either use as comparison or as a base for increment
        LVOXGridSet merged;


        merged.ni = new lvox::Grid3Di(a->ni->minX(), a->ni->minY(), a->ni->minZ(), a->ni->xdim(), a->ni->ydim(), a->ni->zdim(), a->ni->resolution(), a->ni->NA(), a->ni->NA());
        merged.nt = new lvox::Grid3Di(a->nt->minX(), a->nt->minY(), a->nt->minZ(), a->nt->xdim(), a->nt->ydim(), a->nt->zdim(), a->nt->resolution(), a->nt->NA(), a->nt->NA());
        merged.nb = new lvox::Grid3Di(a->nb->minX(), a->nb->minY(), a->nb->minZ(), a->nb->xdim(), a->nb->ydim(), a->nb->zdim(), a->nb->resolution(), a->nb->NA(), a->nb->NA());
        merged.rd = new lvox::Grid3Df(a->rd->minX(), a->rd->minY(), a->rd->minZ(), a->rd->xdim(), a->rd->ydim(), a->rd->zdim(), a->rd->resolution(), a->rd->NA(), a->rd->NA());

        //Connecting workers and progress bar
        LVOX3_MergeGridsWorker worker(&merged, &gs, reducer.get());

        connect(&worker, SIGNAL(progressChanged(int)), this, SLOT(workerProgressChanged(int)), Qt::DirectConnection);
        connect(this, SIGNAL(stopped()), &worker, SLOT(cancel()), Qt::DirectConnection);
        worker.compute(); //computes the merging with mergegridworker

        // qDebug() << "Step 3 : ";
        //Prerequisites for visualizing
        merged.ni->computeMinMax();
        merged.nt->computeMinMax();
        merged.nb->computeMinMax();
        merged.rd->computeMinMax();

        rootGroup->addSingularItem(_outGrid_ni, merged.ni);
        rootGroup->addSingularItem(_outGrid_nt, merged.nt);
        rootGroup->addSingularItem(_outGrid_nb, merged.nb);
        rootGroup->addSingularItem(_out_Density, merged.rd);

        /*
    //write debug file
    QString f1 = "Scan1";
    QString f2 = "Scan2";
    QString fm = "Scanm";
    QFile file1(f1);
    QFile file2(f2);
    QFile filem(fm);
    if (file1.open(QIODevice::ReadWrite) && file2.open(QIODevice::ReadWrite) && filem.open(QIODevice::ReadWrite) && gs.size() > 1) {
        QTextStream stream1(&file1);
        QTextStream stream2(&file2);
        QTextStream streamm(&filem);
        for (int ci = 0; ci < merged.rd->nCells(); ci++){
            streamm << merged.rd->valueAtIndex(ci) << endl;
        }
        for (int foo = 0; foo < 2; foo++){
            LVOXGridSet *aa= gs.at(foo);
            for (int ci = 0; ci < aa->rd->nCells(); ci++){
                if(foo == 0){
                    stream1 << aa->nt->valueAtIndex(ci) << " "<< aa->nb->valueAtIndex(ci) << " "<< aa->rd->valueAtIndex(ci) << endl;
                }else{
                    stream2 << aa->nt->valueAtIndex(ci) << " "<< aa->nb->valueAtIndex(ci) << " "<< aa->rd->valueAtIndex(ci) << endl;
                }
            }
        }
        file1.close();
        file2.close();
        filem.close();
    }
    */

        // qDebug() << "Step 4 : ";
        // Deletes the grid set from memory
        qDeleteAll(gs);
    }
}

void LVOX3_StepMergeGrids::workerProgressChanged(int p)
{
    setProgress(p);
}

