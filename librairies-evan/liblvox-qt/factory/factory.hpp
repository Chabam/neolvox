#ifndef LVOX3_FACTORY_HPP
#define LVOX3_FACTORY_HPP

#include <map>
#include <string>

/**
 * @class Factory
 * @brief A generic factory class for creating objects of derived types.
 *
 * This class provides functionality for creating objects of derived types from a base class.
 * It allows registration of derived types with corresponding creation functions, which are then used
 * to create objects based on the given type name.
 *
 * @tparam T The base class type from which the derived types are derived.
 */
template <typename T, typename TKey = std::string>
class Factory
{
public:
    /**
     * @brief Register a derived type with the factory.
     * @tparam TDerived The derived type to register with the factory.
     * @param name The name associated with the derived type.
     *
     * This function registers a derived type with the factory, allowing it to be created later
     * based on its associated name.
     *
     * @note The derived type (TDerived) must be derived from the base class (T).
     */
    template <typename TDerived>
    void registerType(TKey name)
    {
        static_assert(std::is_base_of<T, TDerived>::value,
                      "Factory::registerType doesn't accept this type because it doesn't derive from the base class");
        _createFuncs[name] = &createFunc<TDerived>;
    }

    /**
     * @brief Create an object of the specified derived type.
     * @param name The name of the derived type.
     * @return A pointer to the created object.
     *
     * This function creates an object of the specified derived type based on the given name.
     * If the specified type is registered with the factory, the corresponding creation function
     * is used to create the object. If not, a nullptr is returned.
     */
    T* create(TKey& name)
    {
        typename std::map<TKey, PCreateFunc>::const_iterator it = _createFuncs.find(name);
        if (it != _createFuncs.end())
        {
            return it->second();
        }
        return nullptr;
    }

private:
    /**
     * @brief Create function for a specific derived type.
     * @tparam TDerived The derived type to create.
     * @return A pointer to the created object.
     *
     * This function is used to create an object of a specific derived type (TDerived).
     */
    template <typename TDerived>
    static T* createFunc()
    {
        return new TDerived();
    }

    typedef T* (*PCreateFunc)();  ///< Function pointer type for creating objects.
    std::map<TKey, PCreateFunc> _createFuncs; ///< Map to store type names and associated create functions.
};

#endif // LVOX3_FACTORY_HPP
