#ifndef CT_HANDLEWITHMULTIPLEMODELT_H
#define CT_HANDLEWITHMULTIPLEMODELT_H

#include "ct_handlewithmultipleabstractmodel.h"
#include <QList>
#include <QVector>

template<class ModelT>
class CT_HandleWithMultipleModelT : public CT_HandleWithMultipleAbstractModel<typename ModelT::PublicSuperClass> {
public:
    using AbstractModelType = typename ModelT::PublicSuperClass;
    using ModelType = ModelT;
    using ModelsCollectionType = QVector<ModelType*>;

    CT_HandleWithMultipleModelT() {}
    CT_HandleWithMultipleModelT(const CT_HandleWithMultipleModelT& other) = delete;
    virtual ~CT_HandleWithMultipleModelT() { disconnectModels(); }

    void setModels(const ModelType* model) { disconnectModels(); m_models.clear(); m_models.append(const_cast<ModelType*>(model)); connectModels(); }
    void setModels(const QList<ModelType*>& models) { disconnectModels(); m_models = models.toVector(); connectModels(); }
    void setModels(const ModelsCollectionType& models) { disconnectModels(); m_models = models; connectModels(); }

    template<typename Visitor>
    bool visitModels(const Visitor& visitor) const {
        for(const ModelType* model : m_models) {
            if(!visitor(model))
                return false;
        }

        return true;
    }

    ModelType* findModelWithParent(const CT_OutAbstractModel* parentModel) const {
        const CT_OutAbstractModel* originalParentModel = parentModel->recursiveOriginalModel();

        for(ModelType* model : m_models) {
            if(static_cast<CT_OutAbstractModel*>(model->parentModel())->recursiveOriginalModel() == originalParentModel)
                return model;
        }

        return nullptr;
    }

    AbstractModelType* findAbstractModelWithParent(const CT_OutAbstractModel* parentModel) const final {
        return findModelWithParent(parentModel);
    }

    int nModels() const { return m_models.size(); }

    const ModelType* firstModel() const { return m_models[0]; }

    bool isValid() const { return !m_models.isEmpty(); }

private:
    ModelsCollectionType                        m_models;
    QHash<ModelType*, QMetaObject::Connection>  m_connections;

    void connectModels() {
        for(ModelType* model : m_models) {
            QMetaObject::Connection con = QObject::connect(model, &ModelType::destroyed, [this](QObject* obj) {
                m_models.removeOne(static_cast<ModelType*>(obj));
            });

            if(con)
                m_connections.insert(model, con);
        }
    }

    void disconnectModels() {
        QHashIterator<ModelType*, QMetaObject::Connection> it(m_connections);

        while(it.hasNext()) {
            it.next();

            if(it.value())
                QObject::disconnect(it.value());
        }

        m_connections.clear();
    }
};

#endif // CT_HANDLEWITHMULTIPLEMODELT_H
