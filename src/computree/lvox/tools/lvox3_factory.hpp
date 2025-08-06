#ifndef LVOX3_FACTORY_HPP
#define LVOX3_FACTORY_HPP

#include <QMap>
#include <QString>

// TODO: Check if really needed (FC)

template <typename T>
class Factory
{
public:
    template <typename TDerived>
    void registerType(QString name)
    {
        static_assert(std::is_base_of<T, TDerived>::value, "Factory::registerType doesn't accept this type because doesn't derive from base class");
        _createFuncs[name] = &createFunc<TDerived>;
    }

    T* create(QString name) {
        typename QMap<QString,PCreateFunc>::const_iterator it = _createFuncs.find(name);
        if (it != _createFuncs.end()) {
            return it.value()();
        }
        return nullptr;
    }

private:
    template <typename TDerived>
    static T* createFunc()
    {
        return new TDerived();
    }

    typedef T* (*PCreateFunc)();
    QMap<QString,PCreateFunc> _createFuncs;
};

#endif // LVOX3_FACTORY_HPP
