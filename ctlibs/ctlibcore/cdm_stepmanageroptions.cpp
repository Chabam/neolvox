#include "cdm_stepmanageroptions.h"

#include <QFile>
#include <QLocale>

#include "cdm_configfile.h"

CDM_StepManagerOptions::CDM_StepManagerOptions()
{
    _nJumpInFastDebugMode = 20;
    _autoClearMemory = false;
    _locale = QLocale(QLocale::English, QLocale::UnitedKingdom).name();
    m_timeToSleepInAutoDebugMode = 1;
    m_nJumpInAutoDebugMode = 1;
}

void CDM_StepManagerOptions::setAutoClearResultFromMemory(bool enable)
{
    _autoClearMemory = enable;
}

bool CDM_StepManagerOptions::changeFastJumpInDebugMode(int nJump)
{
    if(nJump > 0) {
        _nJumpInFastDebugMode = nJump;
        return true;
    }

    return false;
}

void CDM_StepManagerOptions::setTimeToSleepInAutoDebugMode(int timeInMs)
{
    m_timeToSleepInAutoDebugMode = timeInMs;
}

void CDM_StepManagerOptions::setNJumpInAutoDebugMode(int n)
{
    m_nJumpInAutoDebugMode = n;
}

bool CDM_StepManagerOptions::isAutoClearResultFromMemoryEnable() const
{
    return _autoClearMemory;
}

int CDM_StepManagerOptions::getFastJumpValueInDebugMode() const
{
    return _nJumpInFastDebugMode;
}

int CDM_StepManagerOptions::getTimeToSleepInAutoDebugMode() const
{
    return m_timeToSleepInAutoDebugMode;
}

int CDM_StepManagerOptions::getNJumpInAutoDebugMode() const
{
    return m_nJumpInAutoDebugMode;
}

void CDM_StepManagerOptions::setQLocale(QString locale)
{
    _locale = locale;
}

QString CDM_StepManagerOptions::getQLocale()
{
    return _locale;
}

void CDM_StepManagerOptions::load()
{
    CONFIG_FILE->beginGroup("StepManagerOptions");

    changeFastJumpInDebugMode(CONFIG_FILE->value("nJumpInFastDebugMode", getFastJumpValueInDebugMode()).toInt());
    setQLocale(CONFIG_FILE->value("locale", getQLocale()).toString());
    setNJumpInAutoDebugMode(CONFIG_FILE->value("nJumpInAutoDebugMode", m_nJumpInAutoDebugMode).toInt());
    setTimeToSleepInAutoDebugMode(CONFIG_FILE->value("timeToSleepInAutoDebugMode", m_timeToSleepInAutoDebugMode).toInt());
    CONFIG_FILE->endGroup();
}

bool CDM_StepManagerOptions::save() const
{
    CONFIG_FILE->beginGroup("StepManagerOptions");

    CONFIG_FILE->setValue("nJumpInFastDebugMode", getFastJumpValueInDebugMode());
    CONFIG_FILE->setValue("locale", _locale);
    CONFIG_FILE->setValue("nJumpInAutoDebugMode", m_nJumpInAutoDebugMode);
    CONFIG_FILE->setValue("timeToSleepInAutoDebugMode", m_timeToSleepInAutoDebugMode);

    CONFIG_FILE->endGroup();

    return true;
}
