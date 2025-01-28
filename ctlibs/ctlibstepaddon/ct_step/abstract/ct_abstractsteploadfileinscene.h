#ifndef CT_ABSTRACTSTEPLOADFILEINSCENE_H
#define CT_ABSTRACTSTEPLOADFILEINSCENE_H

#include "ctlibstepaddon_global.h"

#include "ct_step/abstract/ct_abstractsteploadfile.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_defines.h"

#include <QFile>

/**
 * @brief Represent a step that load a file in a CT_Scene
 */
class CTLIBSTEPADDON_EXPORT CT_AbstractStepLoadFileInScene : public CT_AbstractStepLoadFile
{
    Q_OBJECT

public:
    CT_AbstractStepLoadFileInScene();

protected:
    /**
     * @brief Declare output models (the scene)
     */
    void declareOutputModels(CT_StepOutModelStructureManager& manager) override;

    /**
     * @brief Read the header file
     * @param f : the file to read
     * @return number of characters readed. -1 if it was an error.
     */
    virtual int readHeaderFile(QFile& f) = 0;

    /**
     * @brief Read the data file (after the header) and call the "createOutResult" method
     * @param f : the file to read
     * @param offset : number of characters already readed
     * @param little_endian : true if data is in littleEndian
     */
    virtual void readDataFile(QFile& f, int offset, bool little_endian = false);

    /**
     * @brief Read the file
     */
    void compute() override;

    /**
     * @brief Create the scene and add it to the result.
     * @param pcir : index cloud of points readed
     * @param xmin : bounding box xmin
     * @param xmax : bounding box ymin
     * @param ymin : bounding box zmin
     * @param ymax : bounding box xmax
     * @param zmin : bounding box ymax
     * @param zmax : bounding box zmax
     */
    virtual void createOutResult(CT_PCIR pcir, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

private:
    CT_HandleOutResultGroup             m_hOutResult;
    CT_HandleOutStdGroup                m_hOutRootGroup;
    CT_HandleOutSingularItem<CT_Scene>  m_hOutScene;
};

#endif // CT_ABSTRACTSTEPLOADFILEINSCENE_H
