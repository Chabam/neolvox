#ifndef INTERFACESFORMODELS_H
#define INTERFACESFORMODELS_H

#include "settingsinterfaces.h"

class CT_OutAbstractModel;
class CT_OutAbstractItemModel;
class CT_OutAbstractItemAttributeModel;
class CT_OutAbstractResultModel;

class IPrototypeForModel;
class IItemAttributeForModel;
class IItemDrawableForModel;
class IResultForModel;
class IStepForModel;

class ICategoryForModel {
public:
    virtual ~ICategoryForModel() {}

    virtual QString uniqueName() const = 0;
    virtual bool isEquivalentTo(const QString& categoryUniqueName) const = 0;
};

class IPrototypeToolForModel {
public:
    virtual ~IPrototypeToolForModel() {}
    virtual void setModel(const CT_OutAbstractModel* model) = 0;
    virtual CT_OutAbstractModel* model() const = 0;
};

class IPrototypeForModel {
public:
    virtual ~IPrototypeForModel() {}

    virtual IPrototypeToolForModel* prototypeToolForModel() const = 0;
};

class IItemAttributeToolForModel {
public:
    virtual ~IItemAttributeToolForModel() {}
    virtual ICategoryForModel* category() const = 0;
    virtual int valueType() const = 0;
    virtual QString valueTypeToString() const = 0;
    virtual IItemAttributeForModel* copyItemAttribute() const = 0;
};

class IItemAttributeForModel : public IPrototypeForModel {
public:
    virtual IItemAttributeToolForModel* itemAttributeToolForModel() const = 0;
};

class IItemDrawableToolForModel {
public:
    using ItemAttributesVisitor = std::function<bool(const IItemAttributeForModel*)>;

    virtual ~IItemDrawableToolForModel() {}
    virtual QString displayableName() const = 0;
    virtual QString nameFromType() const = 0;
    virtual QString itemInheritPath() const = 0;
    virtual bool visitItemHisDefaultAttributes(const ItemAttributesVisitor& visitor) const = 0;
    virtual IItemDrawableForModel* copyItem() const = 0;
};

class IItemDrawableForModel : public IPrototypeForModel {
public:
    virtual IItemDrawableToolForModel* itemToolForModel() const = 0;
};

class ISingularItemDrawableForModel {
public:
    virtual ~ISingularItemDrawableForModel() {}
};

class IGroupForModel {
public:
    virtual ~IGroupForModel() {}
};

class IResultToolForModel {
public:
    virtual ~IResultToolForModel() {}
    virtual void setResultName(const QString& name) = 0;
    virtual CT_OutAbstractResultModel* resultModel() const = 0;
    virtual IResultForModel* copyResult(const CT_OutAbstractResultModel* newModelToUse) const = 0;
};

class IResultForModel : public IPrototypeForModel {
public:
    virtual IResultToolForModel* resultToolForModel() const = 0;
};

class IResultGroupForModel {
public:
    virtual ~IResultGroupForModel() {}
};

class IDocumentForModel {
public:
    virtual ~IDocumentForModel() {}
};

class IStepToolForModel {
public:
    using OutResultsVisitor = std::function<bool(const IResultForModel* model)>;
    virtual ~IStepToolForModel() {}
    virtual QString displayableCustomName() const = 0;
    virtual bool isStepAPrototype() const = 0;
    virtual bool visitStepHisOutResults(const OutResultsVisitor& visitor) const = 0;
};

class IStepForModel {
public:
    virtual ~IStepForModel() {}
    virtual IStepToolForModel* stepToolForModel() const = 0;
};

#endif // INTERFACESFORMODELS_H
