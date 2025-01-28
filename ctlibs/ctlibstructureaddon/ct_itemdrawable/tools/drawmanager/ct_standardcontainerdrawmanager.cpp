#include "ct_standardcontainerdrawmanager.h"

#include "ct_itemdrawable/ct_container.h"

CT_StandardContainerDrawManager::CT_StandardContainerDrawManager(QString drawConfigurationName) : CT_AbstractItemDrawableDrawManager(drawConfigurationName.isEmpty() ? CT_Container::staticName() : drawConfigurationName)
{
}

CT_StandardContainerDrawManager::~CT_StandardContainerDrawManager()
{
}

void CT_StandardContainerDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    const CT_Container &item = dynamic_cast<const CT_Container&>(itemDrawable);

    painter.enableSetColor(false);

    QListIterator<CT_AbstractItemDrawable*> it(*item.getList());

    while(it.hasNext())
    {
        it.next()->draw(view, painter);
    }

    painter.enableSetColor(true);
}

