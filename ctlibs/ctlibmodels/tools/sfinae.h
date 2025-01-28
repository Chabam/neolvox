#ifndef SFINAE_H
#define SFINAE_H

#include <QtGlobal>

/**
 * @brief SFINAE tool to check more than one condition in same time. Using it like this :
 *        std::integral_constant<bool, SFINAE_And_<IsAnInputAbstractHandle<T>, IsAnInputModel<T>>::value>()
 */
template<typename... Conds>
struct SFINAE_And_
        : std::true_type
{ };

template<typename Cond, typename... Conds>
struct SFINAE_And_<Cond, Conds...>
        : std::conditional<Cond::value, SFINAE_And_<Conds...>, std::false_type>::type
{ };

/**
 * @brief SFINAE tool to know if the model was an input result model that produce copies
 */
template <typename Type>
class IsAResultModelCopy
{
   class yes { char m;};
   class no { yes m[2];};
   struct BaseMixin
   {
     int nOutResultModelCopiesCreated() const {return 0;}
   };
   struct Base : public Type, public BaseMixin {};
   template <typename T, T t>  class Helper{};
   template <typename U>
   static no deduce(U*, Helper<int (BaseMixin::*)() const, &U::nOutResultModelCopiesCreated>* = 0);
   static yes deduce(...);
public:
   static const bool value = sizeof(yes) == sizeof(deduce((Base*)(0)));
};

/**
 * @brief SFINAE tool to know if the model is an output model
 */
template <typename Type>
class IsAnOutputModel
{
   class yes { char m;};
   class no { yes m[2];};
   struct BaseMixin
   {
     int uniqueIndex() const {return 0;}
   };
   struct Base : public Type, public BaseMixin {};
   template <typename T, T t>  class Helper{};
   template <typename U>
   static no deduce(U*, Helper<int (BaseMixin::*)() const, &U::uniqueIndex>* = 0);
   static yes deduce(...);
public:
   static const bool value = sizeof(yes) == sizeof(deduce((Base*)(0)));
};

/**
 * @brief SFINAE tool to know if the model is an input model. If the method "uniqueIndex() const" is present
 *        we deduce that it is an output model.
 */
template <typename Type>
class IsAnInputModel
{
   class yes { char m;};
   class no { yes m[2];};
   struct BaseMixin
   {
     int uniqueIndex() const {return 0;}
   };
   struct Base : public Type, public BaseMixin {};
   template <typename T, T t>  class Helper{};
   template <typename U>
   static no deduce(U*, Helper<int (BaseMixin::*)() const, &U::uniqueIndex>* = 0);
   static yes deduce(...);
public:
   static const bool value = sizeof(no) == sizeof(deduce((Base*)(0)));
};


template <typename T, typename = void>
struct HasApplicableToPoint : std::false_type{};

template <typename T>
struct HasApplicableToPoint<T, decltype((void)T::ApplicableToPoint, void())> : std::true_type {};

template <typename T, typename = void>
struct HasApplicableToEdge : std::false_type{};

template <typename T>
struct HasApplicableToEdge<T, decltype((void)T::ApplicableToEdge, void())> : std::true_type {};

template <typename T, typename = void>
struct HasApplicableToFace : std::false_type{};

template <typename T>
struct HasApplicableToFace<T, decltype((void)T::ApplicableToFace, void())> : std::true_type {};

/**
 * @brief SFINAE tool to know if the handle is an abstract input handle. If the enum "MinValue" is present
 *        we deduce that it is an concrete input handle (not abstract).
 */
// You need void_t to avoid a warning about the lhs of the comma operator
// having no effect. C++ 17 has std::void_t
template<class...> using void_t = void;

template<class T, class = void>
struct IsAnInputAbstractHandle
{
    static constexpr bool value = false;
};

template<class T>
struct IsAnInputAbstractHandle<T, void_t<decltype(T::MinValue)>>
{
    static constexpr bool value = true;
};

#endif // SFINAE_H
