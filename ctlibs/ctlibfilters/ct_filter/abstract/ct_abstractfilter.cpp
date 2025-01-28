#include "ct_abstractfilter.h"

CT_AbstractFilter::CT_AbstractFilter(QString pluginName) : CT_AbstractConfigurableElement(pluginName)
{
}

CT_AbstractFilter::CT_AbstractFilter(const CT_AbstractFilter &other) : CT_AbstractConfigurableElement(other)
{
}

void CT_AbstractFilter::saveSettings(SettingsWriterInterface& writer) const
{
    Q_UNUSED(writer)
}

bool CT_AbstractFilter::restoreSettings(SettingsReaderInterface& reader)
{
    Q_UNUSED(reader)

    return true;
}

