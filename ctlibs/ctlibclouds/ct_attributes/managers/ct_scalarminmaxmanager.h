#ifndef CT_SCALARMINMAXMANAGER_H
#define CT_SCALARMINMAXMANAGER_H

#include <QHash>

#include <limits>

template<typename SCALAR>
class CT_ScalarMinMaxManager
{
public:
    CT_ScalarMinMaxManager()
    {
    }

    ~CT_ScalarMinMaxManager()
    {
        for(const Backup* backup : mMinMaxByMask)
        {
            qDeleteAll(backup->mMinMaxAttributes.begin(), backup->mMinMaxAttributes.end());
            delete backup;
        }
    }

    template<typename SCALART, typename ATT>
    void registerAttribute(ATT* attribute, SCALART min, SCALART max)
    {
        Backup* globalBackup = mMinMaxByMask.value(attribute->mask(), nullptr);

        if(globalBackup == nullptr)
        {
            globalBackup = new BackupT<SCALART>(min, max);
            mMinMaxByMask.insert(attribute->mask(), globalBackup);
        }
        else
        {
            BackupT<SCALART>* globalBackupCasted = static_cast<BackupT<SCALART>*>(globalBackup);
            globalBackupCasted->mMin = qMin(globalBackupCasted->mMin, min);
            globalBackupCasted->mMax = qMax(globalBackupCasted->mMax, max);
        }

        globalBackup->mMinMaxAttributes.insert(attribute, new BackupT<SCALART>(min, max));
    }

    template<typename SCALART, typename ATT>
    void updateAttribute(ATT* attribute, SCALART min, SCALART max)
    {
        Backup* globalBackup = mMinMaxByMask.value(attribute->mask(), nullptr);

        if(globalBackup == nullptr)
            return;

        Backup* attBackup = globalBackup->mMinMaxAttributes.value(attribute, nullptr);

        if(attBackup == nullptr)
            return;

        // update the local
        static_cast<BackupT<SCALART>*>(attBackup)->mMin = min;
        static_cast<BackupT<SCALART>*>(attBackup)->mMax = max;

        // update the global
        BackupT<SCALART>* globalBackupCasted = static_cast<BackupT<SCALART>*>(globalBackup);
        globalBackupCasted->mMin = min;
        globalBackupCasted->mMax = max;

        for(const Backup* backup : globalBackupCasted->mMinMaxAttributes)
        {
            const BackupT<SCALART>* casted = static_cast<const BackupT<SCALART>*>(backup);

            globalBackupCasted->mMin = qMin(globalBackupCasted->mMin, casted->mMin);
            globalBackupCasted->mMax = qMax(globalBackupCasted->mMax, casted->mMax);
        }
    }

    template<typename SCALART, typename ATT>
    void unregisterAttribute(ATT* attribute)
    {
        Backup* globalBackup = mMinMaxByMask.value(attribute->mask(), nullptr);

        if(globalBackup == nullptr)
            return;

        delete globalBackup->mMinMaxAttributes.take(attribute);

        if(globalBackup->mMinMaxAttributes.isEmpty())
        {
            delete mMinMaxByMask.take(attribute->mask());
            return;
        }

        BackupT<SCALART>* firstAttBackupCasted = static_cast<BackupT<SCALART>*>(globalBackup->mMinMaxAttributes.begin().value());

        // update the global
        BackupT<SCALART>* globalBackupCasted = static_cast<BackupT<SCALART>*>(globalBackup);
        globalBackupCasted->mMin = firstAttBackupCasted->mMin;
        globalBackupCasted->mMax = firstAttBackupCasted->mMax;

        for(const Backup* backup : globalBackupCasted->mMinMaxAttributes)
        {
            const BackupT<SCALART>* casted = static_cast<const BackupT<SCALART>*>(backup);

            globalBackupCasted->mMin = qMin(globalBackupCasted->mMin, casted->mMin);
            globalBackupCasted->mMax = qMax(globalBackupCasted->mMax, casted->mMax);
        }
    }

    template<typename SCALART, typename ATT>
    SCALART min(const ATT* attribute) const
    {
        Backup* globalBackup = mMinMaxByMask.value(attribute->mask(), nullptr);

        if(globalBackup == nullptr)
            return SCALART();

        Backup* attBackup = globalBackup->mMinMaxAttributes.value(attribute, nullptr);

        if(attBackup == nullptr)
            return SCALART();

        return static_cast<BackupT<SCALART>*>(attBackup)->mMin;
    }

    template<typename SCALART, typename ATT>
    SCALART max(const ATT* attribute) const
    {
        Backup* globalBackup = mMinMaxByMask.value(attribute->mask(), nullptr);

        if(globalBackup == nullptr)
            return SCALART();

        Backup* attBackup = globalBackup->mMinMaxAttributes.value(attribute, nullptr);

        if(attBackup == nullptr)
            return SCALART();

        return static_cast<BackupT<SCALART>*>(attBackup)->mMax;
    }

    static void InitMinMax(SCALAR& min, SCALAR& max)
    {
        InternalInitMinMaxIfBool(min, max, std::integral_constant<bool, std::is_same<SCALAR, bool>::value>());
    }

private:

    class Backup
    {
    public:
        virtual ~Backup() {}

        QHash<const void*, Backup*> mMinMaxAttributes;
    };

    template<typename SCALART>
    class BackupT : public Backup
    {
    public:
        BackupT(SCALART min, SCALART max) : mMin(min), mMax(max) {}

        SCALART  mMin;
        SCALART  mMax;
    };

    QHash<quint64, Backup*> mMinMaxByMask;

    static void InternalInitMinMaxIfBool(SCALAR& min, SCALAR& max, std::true_type)
    {
        min = true;
        max = false;
    }

    static void InternalInitMinMaxIfBool(SCALAR& min, SCALAR& max, std::false_type)
    {
        InternalInitMinMax(min, max, std::integral_constant<bool, std::numeric_limits<SCALAR>::is_signed>());
    }

    static void InternalInitMinMax(SCALAR& min, SCALAR& max, std::true_type)
    {
        min = std::numeric_limits<SCALAR>::max();
        max = -std::numeric_limits<SCALAR>::max();
    }

    static void InternalInitMinMax(SCALAR& min, SCALAR& max, std::false_type)
    {
        min = std::numeric_limits<SCALAR>::max();
        max = std::numeric_limits<SCALAR>::min();
    }
};

#endif // CT_SCALARMINMAXMANAGER_H
