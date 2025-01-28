#ifndef MODELSASSERT_H
#define MODELSASSERT_H

#include <QtGlobal>

#ifndef MODELS_ASSERT
#define MODELS_ASSERT(cond) Q_ASSERT(cond)
#endif

#ifndef MODELS_ASSERT_X
#define MODELS_ASSERT_X(cond, where, what) Q_ASSERT_X(cond, where, what)
#endif

#endif // MODELSASSERT_H
