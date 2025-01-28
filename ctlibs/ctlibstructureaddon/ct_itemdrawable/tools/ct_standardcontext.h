#ifndef CT_STANDARDCONTEXT_H
#define CT_STANDARDCONTEXT_H

#include "pluginShared_global.h"
#include "ct_model/abstract/ct_abstractmodel.h"
#include "ct_itemdrawable/model/outModel/abstract/ct_outabstractgroupmodel.h"
#include "ct_itemdrawable/model/inModel/abstract/ct_inabstractgroupmodel.h"
#include <QMap>

class CT_VirtualAbstractStep;

class PLUGINSHAREDSHARED_EXPORT CT_StandardContext
{
public:
    CT_StandardContext();
    virtual ~CT_StandardContext();

    void setStep(const CT_VirtualAbstractStep *step);
    inline CT_VirtualAbstractStep* step() const { return _step; }

    void addModel(const CT_AbstractModel *model);
    inline const CT_AbstractModel* getModel(int i) {return _models.at(i);}

    void clear();

    void add(QString key, const void *value);
    QList<void*> get(QString key);

private:
    CT_VirtualAbstractStep*                     _step;
    QVector<const CT_AbstractModel*>            _models;
    QMap<QString, void*>    _data;
};

#endif // CT_STANDARDCONTEXT_H
