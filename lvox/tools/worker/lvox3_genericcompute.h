/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_GENERICCOMPUTE_H
#define LVOX3_GENERICCOMPUTE_H

#include "tools/worker/lvox3_worker.h"

#include "tools/3dgrid/lvox_grid3d.h"
#include "tools/lvox3_genericconfiguration.h"

#include "muParser.h"

/**
 * @brief Use a parser, a formula and multiple input grids to compute the output grid
 */
class LVOX3_GenericCompute : public LVOX3_Worker
{
    Q_OBJECT

public:
    struct Input {
        char gridLetterInFormula;
        CT_AbstractGrid3D* grid;
    };

    /**
     * @brief Create a worker that will use the formula to compute the output grid
     * @param inputs : list of input grid
     * @param formula : formula to use
     * @param output : output grid
     */
    LVOX3_GenericCompute(const QList<Input>& inputs,
                         const QList<lvox::CheckConfiguration>& checksFormula,
                         const std::string& finalFormula,
                         CT_AbstractGrid3D* output);
    ~LVOX3_GenericCompute();

protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:

    struct InternalInput {
        int indexInVariables;
        CT_AbstractGrid3D* grid;
    };

    QVector<InternalInput>  m_inputs;
    QVector<mu::Parser*>    m_checksErrorParsers;

    std::string             m_finalFormula;
    CT_AbstractGrid3D*   m_output;

    QVector<mu::Parser*>    m_checksParsers;
    mu::Parser              m_finalParser;

    QVector<double*>        m_variables;
    double                  m_outputVariable;

    void initParser(mu::Parser& parser, const std::string &formula);
};

#endif // LVOX3_GENERICCOMPUTE_H
