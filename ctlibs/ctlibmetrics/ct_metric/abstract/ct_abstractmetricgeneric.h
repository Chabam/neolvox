#ifndef CT_ABSTRACTMETRICGENERIC_H
#define CT_ABSTRACTMETRICGENERIC_H

#include "ctlibmetrics/ctlibmetrics_global.h"
#include "ctlibmetrics/tools/ct_valueandbool.h"

#include "ct_metric/abstract/ct_abstractmetric.h"
#include "ct_view/ct_genericconfigurablewidget.h"

class CTLIBMETRICS_EXPORT CT_AbstractMetricGeneric : public CT_AbstractMetric
{
    Q_OBJECT
    typedef CT_AbstractMetric SuperClass;

public:
    CT_AbstractMetricGeneric(QString pluginName) : CT_AbstractMetric(pluginName) {}
    CT_AbstractMetricGeneric(const CT_AbstractMetricGeneric &other)  : CT_AbstractMetric(other) {}
    ~CT_AbstractMetricGeneric() override;

    /**
     * @brief Declare a attribute that you will set in your compute method. Call this method in your constructor.
     * @example : registerAttributeVaB(m_classMemberThatWillBeComputedInComputeMetrics, CT_AbstractCategory::DATA_DIMENSION, tr("my attribute that was nice"));
     * @param classMember : a variable member of your class that will be used in "computeMetrics" method.
     * @param categoryName : the name of the category of the attribute (example : CT_AbstractCategory::DATA_AREA)
     * @param displayableName : a displayable name for this attribute (displayed in gui), must not be empty !
     */
    template<typename VType>
    void registerAttributeVaB(const VaB<VType> &classMember, const QString &categoryName, const QString &displayableName) {
        CT_AbstractVaB* vab = static_cast<CT_AbstractVaB*>(const_cast<VaB<VType>*>(&classMember));

        if(!m_vab.contains(vab)) {
            m_vab.append(vab);
            m_attributesVaB.insert(static_cast<void*>(vab), new CT_AbstractMetric::AttributeObject<VType>(displayableName, categoryName));
        }
    }

    /**
     * @brief Call setAttribute(....) in your "compute" method to set results of attributes declared with "addAttribute" method
     * @param classMemberAndValue : the variable member of your class that was filled with the good value and that you used in method "addAttribute"
     */
    template<typename VType>
    void setAttributeValueVaB(VaB<VType> &classMemberAndValue) {
        if(classMemberAndValue.used)
            setAttributeValue<VType>(classMemberAndValue.value);
    }

    /**
     * @brief Search and return the attribute from his displyable name if founded, otherwise return nullptr
     */
    CT_AbstractVaB* findAttributeValueVaBFromDisplayableName(const QString& dName) const;

    /**
     * @brief This method is called when the core will save settings of this object
     */
    void saveSettings(SettingsWriterInterface& writer) const override;

    /**
     * @brief This method is called when the core will restore settings of this object
     */
    bool restoreSettings(SettingsReaderInterface& reader) override;

    /**
     * @brief Returns a default widget to choose attributes to compute if you have used "addAttributeVaB" method
     */
    CT_AbstractConfigurableWidget* createConfigurationWidget() override;

    /**
     * @brief Add a checkBox for each VaB to the given widget
     *
     * Used by createConfigurationWidget();
     */
    void addAllVaBToWidget(CT_GenericConfigurableWidget *wid) const;


private:
    QList<CT_AbstractVaB*>  m_vab;
    AttributesContainer     m_attributesVaB;

protected:
    /**
     * @brief Inherit this method to add your unselectable attributes
     * @warning Don't forget to call this method ! CT_AbstractMetricGeneric::createAttributes()
     */
    void createAttributes() override;
};

#endif // CT_ABSTRACTMETRICGENERIC_H
