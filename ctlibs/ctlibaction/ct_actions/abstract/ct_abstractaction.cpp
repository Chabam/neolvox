#include "ct_abstractaction.h"

#include "ct_actions/view/abstract/ct_gabstractactionoptions.h"

#include <QEvent>

QString CT_AbstractAction::TYPE_SELECTION = "Selection";
QString CT_AbstractAction::TYPE_MODIFICATION = "Modification";
QString CT_AbstractAction::TYPE_INFORMATION = "Information";
QString CT_AbstractAction::TYPE_MEASURE = "Mesure";
QString CT_AbstractAction::TYPE_OTHER = "Autre";

CT_AbstractAction::CT_AbstractAction()
{
    m_doc = nullptr;
    m_finished = false;
    m_suspended = false;
}

QString CT_AbstractAction::uniqueName() const
{
    return metaObject()->className();
}

void CT_AbstractAction::setDocument(const DocumentInterface *doc)
{
    if(m_doc != doc)
    {
        m_doc = (DocumentInterface*)doc;
        documentChangedEvent();
    }
}

DocumentInterface* CT_AbstractAction::document() const
{
    return m_doc;
}

void CT_AbstractAction::init()
{
    setFinished(false);
    //PS_LOG->addMessage(LogInterface::info, LogInterface::action, uniqueName() + tr(" initialized"));
}

void CT_AbstractAction::trigger()
{
    //PS_LOG->addMessage(LogInterface::info, LogInterface::action, uniqueName() + tr(" triggered"));
}

void CT_AbstractAction::suspend()
{
    m_suspended = true;
    //PS_LOG->addMessage(LogInterface::info, LogInterface::action, uniqueName() + tr(" suspended"));
}

bool CT_AbstractAction::isSuspended() const
{
    return m_suspended;
}

void CT_AbstractAction::resume()
{
    m_suspended = false;
    //PS_LOG->addMessage(LogInterface::info, LogInterface::action, uniqueName() + tr(" resumed"));
}

void CT_AbstractAction::hideOptions()
{
    QListIterator<ActionOptionsInterface*> it(m_options);

    while(it.hasNext())
        it.next()->hide();
}

void CT_AbstractAction::showOptions()
{
    QListIterator<ActionOptionsInterface*> it(m_options);

    while(it.hasNext())
        it.next()->show();
}

void CT_AbstractAction::finish()
{
    setFinished(true);
    hideOptions();
    //PS_LOG->addMessage(LogInterface::info, LogInterface::action, uniqueName() + tr(" finished"));
}

bool CT_AbstractAction::isFinished() const
{
    return m_finished;
}

QString CT_AbstractAction::toolTip() const
{
    return description();
}

QString CT_AbstractAction::lastError() const
{
    return m_lastError;
}

bool CT_AbstractAction::event(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        // TODO translate types static variable
    }

    return false;
}

void CT_AbstractAction::setLastError(const QString &error)
{
    m_lastError = error;
}

void CT_AbstractAction::setFinished(bool val)
{
    m_finished = val;
}

void CT_AbstractAction::registerOption(CT_GAbstractActionOptions *options)
{
    m_options.append(options);
}

int CT_AbstractAction::nOptions() const
{
    return m_options.size();
}

ActionOptionsInterface* CT_AbstractAction::optionAt(const int &i) const
{
    return m_options.at(i);
}
