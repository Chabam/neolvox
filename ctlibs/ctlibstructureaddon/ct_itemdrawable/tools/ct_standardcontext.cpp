#include "ct_standardcontext.h"

#include "ct_step/abstract/ct_virtualabstractstep.h"
#include "ct_itemdrawable/model/outModel/abstract/ct_outabstractitemmodel.h"

CT_StandardContext::CT_StandardContext()
{
    _step = NULL;
}

CT_StandardContext::~CT_StandardContext()
{
}

void CT_StandardContext::setStep(const CT_VirtualAbstractStep *step)
{
    _step = (CT_VirtualAbstractStep*)step;
}

void CT_StandardContext::addModel(const CT_AbstractModel *model)
{
    _models.append(model);
}


void CT_StandardContext::clear()
{
    _models.clear();
    _data.clear();
}

void CT_StandardContext::add(QString key, const void *value)
{
    _data.insertMulti(key, (void*)value);
}

QList<void*> CT_StandardContext::get(QString key)
{
    return _data.values(key);
}
