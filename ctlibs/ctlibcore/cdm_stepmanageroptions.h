#ifndef CDM_STEPMANAGEROPTIONS_H
#define CDM_STEPMANAGEROPTIONS_H

#include "computreeCore_global.h"

#include <QString>

/**
 * @brief Options of the CDM_StepManager
 */
class COMPUTREECORESHARED_EXPORT CDM_StepManagerOptions
{
public:
    CDM_StepManagerOptions();

    /**
     * @brief Set to true if you want that after the last step has do his process all results must be cleared from memory
     * @param enable : to true to clear all results from memory, false to keep it.
     */
    void setAutoClearResultFromMemory(bool enable);

    /**
     * @brief Set the local to use
     */
    void setQLocale(QString locale);

    /*!
     *  \brief QLocale utilisée
     */
    QString getQLocale();

    /**
     * @brief Change the number of jump to next valid waitForAckIfInDebugMode() in fast debug mode
     */
    bool changeFastJumpInDebugMode(int nJump);

    /**
     * @brief Time to sleep in ms between two call of ack debug mode
     */
    void setTimeToSleepInAutoDebugMode(int timeInMs);

    /**
     * @brief Change the number of jump to next valid waitForAckIfInDebugMode() in auto debug mode
     */
    void setNJumpInAutoDebugMode(int n);

    /**
     * @brief Returns true if after the last step has do his process all results must be cleared from memory
     */
    bool isAutoClearResultFromMemoryEnable() const;

    /**
     * @brief Number of jump to next valid waitForAckIfInDebugMode() in fast debug mode
     */
    int getFastJumpValueInDebugMode() const;

    /**
     * @brief Time to sleep in ms between two call of ack debug mode
     */
    int getTimeToSleepInAutoDebugMode() const;

    /**
     * @brief Number of jump to next valid waitForAckIfInDebugMode()
     */
    int getNJumpInAutoDebugMode() const;

    /**
     * @brief Load parameters from configuration file
     */
    void load();

    /**
     * @brief Save parameters to configuration file
     */
    bool save() const;

private:

    int         _nJumpInFastDebugMode;
    bool        _autoClearMemory;
    QString     _locale;
    int         m_timeToSleepInAutoDebugMode;   // wait in ms before call ackDebugMode
    int         m_nJumpInAutoDebugMode;         // n jump in auto debug mode
};

#endif // CDM_STEPMANAGEROPTIONS_H
