#include "ct_cloudmetrics.h"

#include "ct_iterator/ct_pointiterator.h"

// percentiles must be in ascending order (25 and 75 must be kept to compute the interquartileDistance !)
int CT_CloudMetrics::PERCENTILE_COEFF[CT_CloudMetrics::PERCENTILE_ARRAY_SIZE] = {1, 5, 10, 20, 25, 30, 40, 50, 60, 70, 75, 80, 90, 95, 99};

CT_CloudMetrics::CT_CloudMetrics(QString pluginName) : SuperClass(pluginName)
{
    declareAttributes();
    m_configAndResults.minZ = -1;
}

CT_CloudMetrics::CT_CloudMetrics(const CT_CloudMetrics &other) : SuperClass(other)
{
    declareAttributes();
    m_configAndResults = other.m_configAndResults;
}

CT_CloudMetrics::Config CT_CloudMetrics::metricConfiguration() const
{
    return m_configAndResults;
}

void CT_CloudMetrics::setMetricConfiguration(const CT_CloudMetrics::Config &conf)
{
    m_configAndResults = conf;
}

void CT_CloudMetrics::saveSettings(SettingsWriterInterface &writer) const
{
    writer.addParameter(this, "MinZ", m_configAndResults.minZ);

    SuperClass::saveSettings(writer);
}

bool CT_CloudMetrics::restoreSettings(SettingsReaderInterface &reader)
{
    QVariant value;
    if(reader.parameter(this, "MinZ", value))
        m_configAndResults.minZ = value.toDouble();

    return SuperClass::restoreSettings(reader);
}

QString CT_CloudMetrics::getShortDisplayableName() const
{
    return tr("Indicateurs statistiques standards (Ht)");
}

QString CT_CloudMetrics::getShortDescription() const
{
    return tr("Calcul des indicateurs statistiques standards, tels que le minimum, la maximum, la moyenne, l'écart-type,...<br>"
              "Ces calculs sont effectués sur la coordonnée Z des points.<br>"
              "Cependant ils n'ont d'intérêt que pour un nuage de points en Hauteur, et non en Altitude.<br>"
              "Il faut donc s'assurer que le nuage de points a été pré-traité, pour soustraire l'altitude du sol à tous les points.");
}

QString CT_CloudMetrics::getDetailledDescription() const
{
    return tr("Les indicateurs suivants sont calculés :"
              "<ul>"
              "<li><strong>N</strong> : Nombre de points</li>"
              "<li><strong>Min</strong> : Hauteur minimum</li>"
              "<li><strong>Max</strong> : Hauteur maximum</li>"
              "<li><strong>Mean</strong> : Hauteur moyenne</li>"
              "<li><strong>Mode</strong> : Mode des hauteurs. La gamme de hauteur est divisée en 64 classes, le mode est le centre de la classe avec le plus grand nombre de points. En cas d'égalité, la classe la plus basse est renvoyée.</li>"
              "<li><strong>StdDev</strong> : Ecart-type des hauteurs</li>"
              "<li><strong>Var</strong> : Variance des hauteurs</li>"
              "<li><strong>CV</strong> : Coefficient de variation des hauteurs</li>"
              "<li><strong>IQDist</strong> : Distance interquartile des hauteurs</li>"
              "<li><strong>Skew</strong> : Skewness des hauteurs</li>"
              "<li><strong>Kurt</strong> : Kurtosis des hauteurs</li>"
              "<li><strong>AAD</strong> : Moyenne des écarts absolus à la moyenne</li>"
              "<li><strong>MADMed</strong> : Médiane des écarts absolus à la médiane</li>"
              "<li><strong>MADMode</strong> : Médiane des écarts absolus au mode</li>"
              "<li><strong>L1 à L4</strong> : L-Moments</li>"
              "<li><strong>LCV</strong> : Coefficient de variation L-Moments des hauteurs</li>"
              "<li><strong>LSkew</strong> : Skewness L-Moments des hauteurs</li>"
              "<li><strong>LKurt</strong> : Kurtosis L-Moments des hauteurs</li>"
              "<li><strong>P01 à P99</strong> : 1er, 5ième, 10ième, 15ième,... 90ième, 95ième et 99ième percentiles de hauteurs (interpolation linéaire entre les observations)</li>"
              "<li><strong>CRR</strong> : ((Moyenne - Minimum) / (Maximum – Minimum))</li>"
              "<li><strong>QuadMean</strong> : Moyenne quadratique des hauteurs</li>"
              "<li><strong>CubMean</strong> : Moyenne cubique des hauteurs</li>"
              "</ul>");
}

CT_AbstractConfigurableWidget* CT_CloudMetrics::createConfigurationWidget()
{
    CT_GenericConfigurableWidget *wid = new CT_GenericConfigurableWidget();

    wid->addDouble(tr("Ne conserver que les points avec H >= "), "m", -1e+10, 1e+10, 4, m_configAndResults.minZ);
    wid->addEmpty();

    addAllVaBToWidget(wid);

    return wid;
}

CT_AbstractConfigurableElement* CT_CloudMetrics::copy() const
{
    return new CT_CloudMetrics(*this);
}

void CT_CloudMetrics::computeMetric()
{
    if(attributes().isEmpty())
        return;

    initValues();

    size_t nPoints = 0;
    double zValue;
    double tmp;
    const CT_AreaShape2DData *area = plotArea();
    std::vector<double> zValues(pointCloud()->size());

    CT_PointIterator it(pointCloud());

    while(it.hasNext())
    {
        const CT_Point& point = it.next().currentPoint();

        if(((area == nullptr) || area->contains(point(CT_Point::X), point(CT_Point::Y))) && (point(CT_Point::Z) >= m_configAndResults.minZ))
        {
            zValue = point(CT_Point::Z);

            m_configAndResults.minimum.value = qMin(m_configAndResults.minimum.value, zValue);
            m_configAndResults.maximum.value = qMax(m_configAndResults.maximum.value, zValue);
            m_configAndResults.mean.value += zValue;
            tmp = zValue*zValue;
            m_configAndResults.elevationQuadraticMean.value += tmp;
            m_configAndResults.elevationCubicMean.value += tmp*zValue;
            zValues[nPoints] = zValue;

            ++nPoints;
        }
    }

    m_configAndResults.totalNumberOfReturns.value = nPoints;

    if(nPoints > 0) {
        double nD = nPoints;

        // resize to fit nPoints
        zValues.resize(nPoints);

        // sort Z values
        std::sort(zValues.begin(), zValues.end());

        m_configAndResults.mean.value /= nD;

        int percentile50Index = 0;

        for(int i=0; i<CT_CloudMetrics::PERCENTILE_ARRAY_SIZE; ++i) {
            const int coeff = CT_CloudMetrics::PERCENTILE_COEFF[i];
            m_configAndResults.percentileValues[i].value = computePercentile(zValues, nPoints, (coeff/100.0));

            // percentile coeff is in ascending order
            if(coeff == 25)
                m_configAndResults.interquartileDistance.value = m_configAndResults.percentileValues[i].value;
            else if(coeff == 75)
                m_configAndResults.interquartileDistance.value = m_configAndResults.percentileValues[i].value - m_configAndResults.interquartileDistance.value;
            else if(coeff == 50)
                percentile50Index = i;
        }

        m_configAndResults.mode.value = computeMode(zValues, nPoints);
        std::vector<double> modeValues(nPoints);
        std::vector<double> medianValues(nPoints);

        // second pass to compute variance, etc...
        double medianValue = m_configAndResults.percentileValues[percentile50Index].value;
        double CL1, CR1, CL2, CL3, CR2, CR3;

        for(size_t index=0; index<nPoints; ++index) {
            zValue = zValues[index];

            double diff = zValue - m_configAndResults.mean.value;

            m_configAndResults.aad.value += std::fabs(diff);

            tmp = diff*diff; // pow(diff, 2)
            m_configAndResults.variance.value += tmp;

            tmp *= diff; // pow(diff, 3)
            m_configAndResults.skewness.value += tmp;

            m_configAndResults.kurtosis.value += (tmp*diff);  // pow(diff, 4)

            medianValues[index] = fabs(zValue - medianValue);
            modeValues[index] = fabs(zValue - m_configAndResults.mode.value);

            CL1 = index;
            CL2 = CL1*((CL1-1.0)/2.0);
            CL3 = CL2*((CL1-2.0)/3.0);
            CR1 = nPoints-(index+1);
            CR2 = CR1*((CR1-1.0)/2.0);
            CR3 = CR2*((CR1-2.0)/3.0);

            m_configAndResults.lMoments[0].value += zValue;
            m_configAndResults.lMoments[1].value += ((CL1 - CR1) * zValue);
            m_configAndResults.lMoments[2].value += ((CL2 - (2.0*CL1*CR1) + CR2) * zValue);
            m_configAndResults.lMoments[3].value += ((CL3 - (3.0*CL2*CR1) + (3.0*CL1*CR2) - CR3) * zValue);
        }

        CL1 = nPoints;
        CL2 = CL1*((CL1-1.0)/2.0);
        CL3 = CL2*((CL1-2.0)/3.0);
        CR1 = CL3*((CL1-3.0)/4.0);

        double doubleInfinity = std::numeric_limits<double>::infinity();

        m_configAndResults.lMoments[0].value /= CL1;

        if(CL2 != 0)
            m_configAndResults.lMoments[1].value /= (CL2*2.0); // same as (m_configAndResults.lMoments[1].value/CL2)/2.0
        else
            m_configAndResults.lMoments[1].value = doubleInfinity;

        if(CL3 != 0)
            m_configAndResults.lMoments[2].value /= (CL3*3.0); // same as (m_configAndResults.lMoments[1].value/CL3)/3.0
        else
            m_configAndResults.lMoments[2].value = doubleInfinity;

        if(CR1 != 0)
            m_configAndResults.lMoments[3].value /= (CR1*4.0); // same as (m_configAndResults.lMoments[1].value/CR1)/4.0
        else
            m_configAndResults.lMoments[3].value = doubleInfinity;

        if((m_configAndResults.lMoments[0].value != 0) && (CL2 != 0))
            m_configAndResults.lMomentsCoeffOfVariation.value = m_configAndResults.lMoments[1].value/m_configAndResults.lMoments[0].value;
        else
            m_configAndResults.lMomentsCoeffOfVariation.value = doubleInfinity;

        if((CL2 != 0) && (CL3 != 0))
            m_configAndResults.lMomentsSkewness.value = m_configAndResults.lMoments[2].value/m_configAndResults.lMoments[1].value;
        else
            m_configAndResults.lMomentsSkewness.value = doubleInfinity;

        if((CL2 != 0) && (CR1 != 0))
            m_configAndResults.lMomentsKurtosis.value = m_configAndResults.lMoments[3].value/m_configAndResults.lMoments[1].value;
        else
            m_configAndResults.lMomentsKurtosis.value = doubleInfinity;

        std::sort(medianValues.begin(), medianValues.end());
        std::sort(modeValues.begin(), modeValues.end());

        m_configAndResults.madMedian.value = computePercentile(medianValues, nPoints, 0.5);
        m_configAndResults.madMode.value = computePercentile(modeValues, nPoints, 0.5);

        if(nPoints > 1) {
            m_configAndResults.variance.value /= (nD-1);
            m_configAndResults.standardDeviation.value = std::sqrt(m_configAndResults.variance.value);
            m_configAndResults.skewness.value /= ( (nD-1) * (std::pow(m_configAndResults.standardDeviation.value, 3)));
            m_configAndResults.kurtosis.value /= ( (nD-1) * (std::pow(m_configAndResults.standardDeviation.value, 4)));

            if(m_configAndResults.mean.value != 0)
                m_configAndResults.coeffOfVariation.value = m_configAndResults.standardDeviation.value / m_configAndResults.mean.value;
            else
                m_configAndResults.coeffOfVariation.value = doubleInfinity;
        } else {
            m_configAndResults.variance.value = doubleInfinity;
            m_configAndResults.standardDeviation.value = doubleInfinity;
            m_configAndResults.skewness.value = doubleInfinity;
            m_configAndResults.kurtosis.value = doubleInfinity;
            m_configAndResults.coeffOfVariation.value = doubleInfinity;
        }

        m_configAndResults.aad.value /= nD;
        m_configAndResults.elevationQuadraticMean.value /= nD;
        m_configAndResults.elevationCubicMean.value /= nD;
        m_configAndResults.elevationQuadraticMean.value = std::sqrt(m_configAndResults.elevationQuadraticMean.value);
        m_configAndResults.elevationCubicMean.value = std::pow(m_configAndResults.elevationCubicMean.value, 1.0/3.0);
        tmp = (m_configAndResults.maximum.value - m_configAndResults.minimum.value);

        if(tmp != 0)
            m_configAndResults.canopyReliefRatio.value = (m_configAndResults.mean.value - m_configAndResults.minimum.value) / tmp;
        else
            m_configAndResults.canopyReliefRatio.value = doubleInfinity;
    }

    setAttributeValueVaB(m_configAndResults.totalNumberOfReturns);
    setAttributeValueVaB(m_configAndResults.minimum);
    setAttributeValueVaB(m_configAndResults.maximum);
    setAttributeValueVaB(m_configAndResults.mean);
    setAttributeValueVaB(m_configAndResults.mode);
    setAttributeValueVaB(m_configAndResults.standardDeviation);
    setAttributeValueVaB(m_configAndResults.variance);
    setAttributeValueVaB(m_configAndResults.coeffOfVariation);
    setAttributeValueVaB(m_configAndResults.interquartileDistance);
    setAttributeValueVaB(m_configAndResults.skewness);
    setAttributeValueVaB(m_configAndResults.kurtosis);
    setAttributeValueVaB(m_configAndResults.aad);
    setAttributeValueVaB(m_configAndResults.madMedian);
    setAttributeValueVaB(m_configAndResults.madMode);

    for(int i=0; i<4; ++i)
        setAttributeValueVaB(m_configAndResults.lMoments[i]);

    setAttributeValueVaB(m_configAndResults.lMomentsCoeffOfVariation);
    setAttributeValueVaB(m_configAndResults.lMomentsSkewness);
    setAttributeValueVaB(m_configAndResults.lMomentsKurtosis);

    for(int i=0; i<CT_CloudMetrics::PERCENTILE_ARRAY_SIZE; ++i)
        setAttributeValueVaB(m_configAndResults.percentileValues[i]);

    setAttributeValueVaB(m_configAndResults.canopyReliefRatio);
    setAttributeValueVaB(m_configAndResults.elevationQuadraticMean);
    setAttributeValueVaB(m_configAndResults.elevationCubicMean);
}

void CT_CloudMetrics::initValues()
{
    m_configAndResults.totalNumberOfReturns.value = 0;
    m_configAndResults.minimum.value = std::numeric_limits<double>::max();
    m_configAndResults.maximum.value = -m_configAndResults.minimum.value;
    m_configAndResults.mean.value = 0;
    m_configAndResults.mode.value = 0;
    m_configAndResults.standardDeviation.value = 0;
    m_configAndResults.variance.value = 0;
    m_configAndResults.coeffOfVariation.value = 0;
    m_configAndResults.interquartileDistance.value = 0;
    m_configAndResults.skewness.value = 0;
    m_configAndResults.kurtosis.value = 0;
    m_configAndResults.aad.value = 0;
    m_configAndResults.madMedian.value = 0;
    m_configAndResults.madMode.value = 0;

    for(int i=0; i<4; ++i)
        m_configAndResults.lMoments[i].value = 0;

    m_configAndResults.lMomentsCoeffOfVariation.value = 0;
    m_configAndResults.lMomentsSkewness.value = 0;
    m_configAndResults.lMomentsKurtosis.value = 0;

    for(int i=0; i<15; ++i)
        m_configAndResults.percentileValues[i].value = 0;

    m_configAndResults.canopyReliefRatio.value = 0;
    m_configAndResults.elevationQuadraticMean.value = 0;
    m_configAndResults.elevationCubicMean.value = 0;
}

double CT_CloudMetrics::computePercentile(const std::vector<double> &array, const size_t &arraySize, const double &p)
{
    // Second Variant, show wikipedia "Percentile", Numpy
    double v = ((double)(arraySize-1)) * p;
    int ip1 = (int)v;
    double f = (v-ip1); // (arraySize-1)*p = ip1+f

    int ip2 = ip1 + 1;

    if(ip2 == static_cast<int>(arraySize))
        return array[ip1];

    if(f == 0)
        return array[ip1];

    return array[ip1] + (f * (array[ip2] - array[ip1]));
}

double CT_CloudMetrics::computeMode(const std::vector<double> &array, const size_t &arraySize)
{
    int numberOfClasses = 64;

    if(arraySize == 1)
    {
            return array[0];
    }

    double step = (array[arraySize-1] - array[0]) / (float)numberOfClasses;

    std::vector<double> classes(numberOfClasses + 1);
    classes[0] = array[0];

    for(size_t i=1; i < static_cast<size_t>((numberOfClasses + 1)); ++i)
    {
        classes[i] = classes[i-1] + step;
    }

    std::vector<size_t> res(numberOfClasses, 0);

    size_t j = 1;

    for(size_t i = 0; i < arraySize; ++i)
    {
        // if there was rounding problem we can have j > numberOfClasses
        // or if step == 0
        while((array[i] >= classes[j]) && (j < static_cast<size_t>(numberOfClasses)))
        {
            ++j;
        }

        res[j - 1] += 1;
    }

    size_t maxOccurence = res[0];
    size_t maxOccurenceIndex = 0;

    for(size_t i = 1; i < static_cast<size_t>(numberOfClasses); ++i)
    {
        if(res[i] > maxOccurence)
        {
            maxOccurence = res[i];
            maxOccurenceIndex = i;
        }
    }

    return (classes[maxOccurenceIndex + 1] + classes[maxOccurenceIndex]) / 2.0;
}

void CT_CloudMetrics::declareAttributes()
{
    registerAttributeVaB(m_configAndResults.totalNumberOfReturns, CT_AbstractCategory::DATA_NUMBER, "N");
    registerAttributeVaB(m_configAndResults.minimum, CT_AbstractCategory::DATA_Z, "Min");
    registerAttributeVaB(m_configAndResults.maximum, CT_AbstractCategory::DATA_Z, "Max");
    registerAttributeVaB(m_configAndResults.mean, CT_AbstractCategory::DATA_Z, "Mean");
    registerAttributeVaB(m_configAndResults.mode, CT_AbstractCategory::DATA_Z, "Mode");
    registerAttributeVaB(m_configAndResults.standardDeviation, CT_AbstractCategory::DATA_Z, "StdDev");
    registerAttributeVaB(m_configAndResults.variance, CT_AbstractCategory::DATA_Z, "Var");
    registerAttributeVaB(m_configAndResults.coeffOfVariation, CT_AbstractCategory::DATA_Z, "CV");
    registerAttributeVaB(m_configAndResults.interquartileDistance, CT_AbstractCategory::DATA_Z, "IQDist");
    registerAttributeVaB(m_configAndResults.skewness, CT_AbstractCategory::DATA_Z, "Skew");
    registerAttributeVaB(m_configAndResults.kurtosis, CT_AbstractCategory::DATA_Z, "Kurt");
    registerAttributeVaB(m_configAndResults.aad, CT_AbstractCategory::DATA_Z, "AAD");
    registerAttributeVaB(m_configAndResults.madMedian, CT_AbstractCategory::DATA_Z, "MADMed");
    registerAttributeVaB(m_configAndResults.madMode, CT_AbstractCategory::DATA_Z, "MADMode");

    for(int i=0; i<CT_CloudMetrics::LMOMENTS_ARRAY_SIZE; ++i)
        registerAttributeVaB(m_configAndResults.lMoments[i], CT_AbstractCategory::DATA_Z, QString("L%1").arg(i+1));

    registerAttributeVaB(m_configAndResults.lMomentsCoeffOfVariation, CT_AbstractCategory::DATA_Z, "LCV");
    registerAttributeVaB(m_configAndResults.lMomentsSkewness, CT_AbstractCategory::DATA_Z, "LSkew");
    registerAttributeVaB(m_configAndResults.lMomentsKurtosis, CT_AbstractCategory::DATA_Z, "LKurt");

    for(int i=0; i<CT_CloudMetrics::PERCENTILE_ARRAY_SIZE; ++i)
        registerAttributeVaB(m_configAndResults.percentileValues[i], CT_AbstractCategory::DATA_Z, QString("P%1%2").arg((PERCENTILE_COEFF[i] < 10 ? "0" : "")).arg(PERCENTILE_COEFF[i]));

    registerAttributeVaB(m_configAndResults.canopyReliefRatio, CT_AbstractCategory::DATA_Z, "CRR");
    registerAttributeVaB(m_configAndResults.elevationQuadraticMean, CT_AbstractCategory::DATA_Z, "QuadMean");
    registerAttributeVaB(m_configAndResults.elevationCubicMean, CT_AbstractCategory::DATA_Z, "CubMean");
}
