#ifndef CT_HANDLEWITHMODELT_H
#define CT_HANDLEWITHMODELT_H

#include "tools/modelsassert.h"

#include <QObject>

template<class ModelT>
class CT_HandleWithModelT {
public:
    using ModelType = ModelT;

    CT_HandleWithModelT() : m_model(nullptr) {}
    CT_HandleWithModelT(const CT_HandleWithModelT& other) = delete;
    virtual ~CT_HandleWithModelT() {}

    void setModel(const ModelType* model) { disconnectModel(); m_model = const_cast<ModelType*>(model); connectModel(); }
    ModelType* model() const { return m_model; }

    bool isValid() const { return (m_model != nullptr); }

private:
    ModelType* m_model;
    QMetaObject::Connection  m_connection;

    void connectModel() {
        if(m_model != nullptr)
            m_connection = QObject::connect(m_model, &ModelType::destroyed, [this](QObject*) {
                m_model = nullptr;
            });
    }

    void disconnectModel() {
        if(m_connection) {
            QObject::disconnect(m_connection);
            m_connection = QMetaObject::Connection();
        }
    }
};

#endif // CT_HANDLEWITHMODELT_H
