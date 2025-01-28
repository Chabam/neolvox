#include "ct_abstractitemdrawabledrawmanager.h"

#include "ct_itemdrawable/abstract/ct_abstractitemdrawable.h"

CT_AbstractItemDrawableDrawManager::CT_AbstractItemDrawableDrawManager(const QString &drawConfigurationName)
{
    m_autoDeleteConfig = true;
    _configuration = nullptr;
    _drawConfigurationNameBackup = drawConfigurationName.isEmpty() ? CT_AbstractItemDrawable::staticName() : drawConfigurationName;
}

CT_AbstractItemDrawableDrawManager::~CT_AbstractItemDrawableDrawManager()
{
    if(m_autoDeleteConfig)
        delete _configuration;
}

void CT_AbstractItemDrawableDrawManager::initDrawConfiguration()
{
    if(m_autoDeleteConfig)
        delete _configuration;

    _configuration = new CT_ItemDrawableConfiguration( createDrawConfiguration(_drawConfigurationNameBackup) );
}

void CT_AbstractItemDrawableDrawManager::preDraw(GraphicsViewInterface&,
                                                 PainterInterface&,
                                                 const CT_AbstractItemDrawable&)
{
}

void CT_AbstractItemDrawableDrawManager::postDraw(GraphicsViewInterface&,
                                                  PainterInterface&,
                                                  const CT_AbstractItemDrawable&)
{
}

CT_ItemDrawableConfiguration* CT_AbstractItemDrawableDrawManager::drawConfiguration() const
{
    return _configuration;
}

CT_ItemDrawableConfiguration CT_AbstractItemDrawableDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    return CT_ItemDrawableConfiguration(drawConfigurationName);
}

void CT_AbstractItemDrawableDrawManager::internalSetDrawConfiguration(CT_ItemDrawableConfiguration* configuration)
{
    if(m_autoDeleteConfig)
        delete _configuration;

    _configuration = configuration;
}

void CT_AbstractItemDrawableDrawManager::internalSetAutoDeleteDrawConfiguration(bool e)
{
    m_autoDeleteConfig = e;
}
