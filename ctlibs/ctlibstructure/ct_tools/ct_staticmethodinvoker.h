#ifndef CT_STATICMETHODINVOKER_H
#define CT_STATICMETHODINVOKER_H

/**
 * @brief Class tool that can invoke a static method at initialization
 */
class CT_StaticMethodInvoker
{
public:
    typedef void (*staticInit)();

    CT_StaticMethodInvoker(staticInit initMethod)
    {
        (*initMethod)();
    }
};

#endif // CT_STATICMETHODINVOKER_H
