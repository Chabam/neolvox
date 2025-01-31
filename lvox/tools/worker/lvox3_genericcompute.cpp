#include "lvox3_genericcompute.h"

//This worker is used to create density grids for each set of (hits,theo,before)
LVOX3_GenericCompute::LVOX3_GenericCompute(const QList<Input>& inputs,
                                           const QList<lvox::CheckConfiguration> &checksFormula,
                                           const std::string &finalFormula,
                                           LVOX3_AbstractGrid3D *output)
{
    m_inputs.reserve(inputs.size());

    foreach (const Input& inp, inputs) {
        InternalInput ii;
        ii.grid = inp.grid;
        ii.indexInVariables = inp.gridLetterInFormula-'a';
        m_inputs.append(ii);
    }

    m_finalFormula = finalFormula;
    m_output = output;

    m_checksParsers.reserve(checksFormula.size());
    m_checksErrorParsers.reserve(checksFormula.size());
    m_variables.reserve(26);

    for(int i=0; i<26; ++i)
        m_variables.append(new double);

    char decimalPoint = QLocale::system().decimalPoint().toLatin1();

    foreach (const lvox::CheckConfiguration& c, checksFormula) {
        mu::Parser* parser = new mu::Parser();
        parser->SetDecSep(decimalPoint);

        initParser(*parser, c.getFormula());

        m_checksParsers.append(parser);

        parser = new mu::Parser();
        parser->SetDecSep(decimalPoint);

        initParser(*parser, c.getErrorFormula());
        m_checksErrorParsers.append(parser);
    }

    m_finalParser.SetDecSep(decimalPoint);

    if(!m_finalFormula.empty()) {
        initParser(m_finalParser, m_finalFormula);
    }
}

LVOX3_GenericCompute::~LVOX3_GenericCompute()
{
    qDeleteAll(m_checksParsers.begin(), m_checksParsers.end());
    qDeleteAll(m_checksErrorParsers.begin(), m_checksErrorParsers.end());
    qDeleteAll(m_variables.begin(), m_variables.end());
}

void LVOX3_GenericCompute::doTheJob()
{
    size_t nCells = m_output->nCells();
    int nCFormula = m_checksParsers.size(), j;

    bool checkVerified;

    setProgressRange(0, nCells);

    for(size_t i=0; (i<nCells) && !mustCancel(); ++i) {

        checkVerified = false;

        for(int k=0; k<m_inputs.size(); ++k) {
            const InternalInput& inp = m_inputs[k];

            (*m_variables[inp.indexInVariables]) = inp.grid->valueAtIndexAsDouble(i);
        }

        m_outputVariable = m_output->valueAtIndexAsDouble(i);

        for(j=0; !checkVerified && (j<nCFormula); ++j) {

            mu::Parser* parser = m_checksParsers[j];

            // the check formula returns true
            //you might want to use == here?
            //@TODO: to be verified
            if((checkVerified = (parser->Eval() != 0))) {
                // we must set the error code
                const double errorValue = m_checksErrorParsers[j]->Eval();
                m_output->setValueAtIndexFromDouble(i, errorValue);
            }
        }

        if(!checkVerified && !m_finalFormula.empty()) {
            m_output->setValueAtIndexFromDouble(i, m_finalParser.Eval());
        }

        setProgress(i);
    }

    m_output->computeMinMax();
}

void LVOX3_GenericCompute::initParser(mu::Parser &parser, const std::string& formula)
{
    int i = 0;

    for(char c = 'a'; c < 'z'; ++c) {
        mu::stringstream_type ss;
        ss << c;
        parser.DefineVar(ss.str(), m_variables[i++]);
    }
    mu::stringstream_type varss, exprss;
    varss << "this";
    exprss << formula.c_str();
    parser.DefineVar(varss.str(), &m_outputVariable);
    parser.SetExpr(exprss.str());
}
