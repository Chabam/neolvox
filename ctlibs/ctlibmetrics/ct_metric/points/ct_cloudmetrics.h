#ifndef CT_CLOUDMETRICS_H
#define CT_CLOUDMETRICS_H

#include "ctlibmetrics/ct_metric/abstract/ct_abstractmetric_xyz.h"
#include "ctlibmetrics/tools/ct_valueandbool.h"

class CTLIBMETRICS_EXPORT CT_CloudMetrics : public CT_AbstractMetric_XYZ
{
    Q_OBJECT
    using SuperClass = CT_AbstractMetric_XYZ;

public:
    static const int PERCENTILE_ARRAY_SIZE = 15;
    static const int LMOMENTS_ARRAY_SIZE = 4;

    struct CTLIBMETRICS_EXPORT Config {
        VaB<size_t>    totalNumberOfReturns;
        VaB<double>    minimum;
        VaB<double>    maximum;
        VaB<double>    mean;
        VaB<double>    mode;
        VaB<double>    standardDeviation;
        VaB<double>    variance;
        VaB<double>    coeffOfVariation;
        VaB<double>    interquartileDistance;
        VaB<double>    skewness;
        VaB<double>    kurtosis;
        VaB<double>    aad;         // average absolute deviation
        VaB<double>    madMedian;   // Median of the absolute deviations from the overall median
        VaB<double>    madMode;     // Median of the absolute deviations from the overall mode
        VaB<double>    lMoments[LMOMENTS_ARRAY_SIZE]; // 1, 2, 3, 4
        VaB<double>    lMomentsCoeffOfVariation;
        VaB<double>    lMomentsSkewness;
        VaB<double>    lMomentsKurtosis;
        VaB<double>    percentileValues[CT_CloudMetrics::PERCENTILE_ARRAY_SIZE]; // 1st, 5th, 10th , 20th, 25th, 30th, 40th, 50th, 60th, 70th, 75th, 80th, 90th, 95th, 99th percentiles
        VaB<double>    canopyReliefRatio;
        VaB<double>    elevationQuadraticMean;
        VaB<double>    elevationCubicMean;

        double         minZ;
    };

    CT_CloudMetrics(QString pluginName);
    CT_CloudMetrics(const CT_CloudMetrics &other);

    /**
     * @brief Returns the metric configuration
     */
    CT_CloudMetrics::Config metricConfiguration() const;

    /**
     * @brief Change the configuration of this metric
     */
    void setMetricConfiguration(const CT_CloudMetrics::Config &conf);

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    QString getShortDisplayableName() const override;
    QString getShortDescription() const override;
    QString getDetailledDescription() const override;

    CT_AbstractConfigurableWidget* createConfigurationWidget() override;

    CT_AbstractConfigurableElement* copy() const final;

protected:
    void declareAttributes();

    void computeMetric() override;

    void initValues();

    double computePercentile(const std::vector<double> &array, const size_t &arraySize, const double &p);
    double computeMode(const std::vector<double> &array, const size_t &arraySize);

    static int PERCENTILE_COEFF[PERCENTILE_ARRAY_SIZE];

private:
    Config  m_configAndResults;
};

#endif // CT_CLOUDMETRICS_H
