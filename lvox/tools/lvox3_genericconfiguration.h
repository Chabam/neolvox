/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_GENERICCONFIGURATION_H
#define LVOX3_GENERICCONFIGURATION_H

#include <QString>
#include <QMetaType>
#include <QList>

#include "lvox_sharedfortest.h"

namespace lvox {
    /**
     * @brief Configuration for one input grid
     */
    struct InGridConfiguration {
        char            gridLetter;
        QString         gridDisplayableName;
        QString         gridType;

        InGridConfiguration() {
            gridLetter = 'a';
        }

        bool operator ==(const InGridConfiguration& other) const {
            return (other.gridType == gridType);
        }

        QString toString() const {
            return QObject::tr("\"%1\" = %2").arg(gridLetterToString(gridLetter)).arg(gridDisplayableName);
        }

        static QString gridLetterToString(char gridLetter) { return QString(gridLetter).toLower(); }
    };

    /**
     * @brief Configuration for conditionnal structure (IF)
     */
    struct CheckConfiguration {

        CheckConfiguration() {
            errorFormula = "-1";
        }

        void setFormula(const QString& f) {
            formula = f.simplified().toLower().toStdString();
        }

        const std::string& getFormula() const { return formula; }
        QString getFormulaInQStringFormat() const { return QString().fromStdString(formula); }

        void setErrorFormula(const QString& f) {
            errorFormula = f.simplified().toLower().toStdString();
        }

        const std::string& getErrorFormula() const { return errorFormula; }
        QString getErrorFormulaInQStringFormat() const { return QString().fromStdString(errorFormula); }

        static QString formulaToString(std::string formula) { return QString().fromStdString(formula).toLower(); }

    private:
        std::string formula;
        std::string errorFormula;
    };

    /**
     * @brief Configuration for one output grid
     */
    struct OutGridConfiguration {
        OutGridConfiguration() {
            gridLetter = 'a';
            gridDataType = QMetaType::Float;
            gridNAValue = -1;
            gridDefaultValue = -1;
        }

        char                        gridLetter;
        QString                     gridDisplayableName;
        QMetaType::Type             gridDataType;
        double                      gridNAValue;
        double                      gridDefaultValue;

        QList<CheckConfiguration>   checks;

        void setFormula(const QString& f) {
            formula = f.simplified().toLower().toStdString();
        }

        const std::string& getFormula() const { return formula; }
        QString getFormulaInQStringFormat() const { return QString().fromStdString(formula); }

        static QString formulaToString(std::string formula) { return QString().fromStdString(formula).toLower(); }

        QString getDisplayableString() const {
            return QObject::tr("\"%1\" [%2]").arg(gridLetterToString(gridLetter)).arg(gridDisplayableName);
        }

        static QString gridLetterToString(char gridLetter) { return QString(QString() + gridLetter + "O").toUpper(); }

    private:
        std::string formula;
    };

    /**
     * @brief Predefined configuration. A predefined configuration is a list of input grids and a list of ouput grids.
     */
    struct PredefinedConfiguration {
        QString                     displayableName;
        QString                     description;
        QList<InGridConfiguration>  input;
        QList<OutGridConfiguration> output;

        bool checkValidity() const {
            if(displayableName.isEmpty())
                return false;

            QSet<char> uniqueGridLetters;

            foreach (const InGridConfiguration& in, input) {
                if(uniqueGridLetters.contains(in.gridLetter))
                    return false;

                uniqueGridLetters.insert(in.gridLetter);
            }

            uniqueGridLetters.clear();

            foreach (const OutGridConfiguration& out, output) {
                if(uniqueGridLetters.contains(out.gridLetter))
                    return false;

                uniqueGridLetters.insert(out.gridLetter);

                if(out.checks.isEmpty() && out.getFormula().empty())
                    return false;
            }

            return true;
        }
    };
}
#endif // LVOX3_GENERICCONFIGURATION_H
