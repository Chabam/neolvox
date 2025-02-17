#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <map>
#include <string>

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
    std::map<std::string, PCreateFunc> _createFuncs;
};

#endif // FACTORY_HPP
