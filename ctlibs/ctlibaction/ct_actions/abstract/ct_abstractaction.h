#ifndef CT_ABSTRACTACTION_H
#define CT_ABSTRACTACTION_H

#include "ctlibaction_global.h"
#include "ct_actions/actionoptionsinterface.h"

class DocumentInterface;
class CT_GAbstractActionOptions;

#define CT_ACTION_DECL_COPY(CLASSNAME) \
CT_AbstractAction* createInstance() const override { return new CLASSNAME(); }

/**
 * @brief Represent an action for the GUI
 */
class CTLIBACTION_EXPORT CT_AbstractAction : public QObject
{
    Q_OBJECT

public:
    CT_AbstractAction();

    /**
     * @brief Return the unique name of the action (for comparison). By
     *        default the name of the class.
     *
     *        A unique name is defined for each class. In this case copies
     *        of a action can be compared to know if it the same.
     */
    virtual QString uniqueName() const;

    /**
     * @brief Return the title of the action (added to the view at the side of the icon)
     */
    virtual QString title() const = 0;

    /**
     * @brief Return the description of the action
     */
    virtual QString description() const = 0;

    /**
     * @brief Return a text used for the tooltip of the button. By default return the description
     */
    virtual QString toolTip() const;

    /**
     * @brief Return the icon of the action
     */
    virtual QIcon icon() const = 0;

    /**
     * @brief Return the type of action
     */
    virtual QString type() const = 0;

    /**
     * @brief Define the document where the action has been added
     */
    void setDocument(const DocumentInterface *doc);

    /**
     * @brief Return the document of the action
     */
    DocumentInterface* document() const;

    /**
     * @brief Called for initialize the action. Called when the user select this action.
     */
    virtual void init();

    /**
     * @brief Called after initialization if the action does make a compute. If after this
     *        step the action is finished (isFinished()) it will be deleted.
     */
    virtual void trigger();

    /**
     * @brief Called when the mouse leave the view (mouseLeaveEvent())
     */
    virtual void suspend();

    /**
     * @brief Return true if action is suspended.
     */
    bool isSuspended() const;

    /**
     * @brief Called when the mouse enter in the view (mouseEnterEvent())
     */
    virtual void resume();

    /**
     * @brief Hide your options. Typically called after suspend. By default hide options
     *        registered (see method "registerOptions(...)" for more information)
     * @warning If you inherit this method don't forget to call the method of the superclass
     */
    virtual void hideOptions();

    /**
     * @brief Show your options. Typically called after resume or after added to a view. By default show
     *        options registered (see method "registerOptions(...)" for more information)
     * @warning If you inherit this method don't forget to call the method of the superclass
     */
    virtual void showOptions();

    /**
     * @brief Called when the user want to finish the action. Per example if he changes his action.
     *
     *        If you've created a options widget you must detach it from the document in this method.
     */
    virtual void finish();

    /**
     * @brief Return true if action is finished.
     */
    bool isFinished() const;

    /**
     * @brief Return the last error string
     */
    QString lastError() const;

    /**
     * @brief called when a mouse double click event occurs on the view
     *
     *        Return true if you do not want the event to be propagated.
     */
    virtual bool mouseDoubleClickEvent(QMouseEvent *e) { Q_UNUSED(e) return true; }

    /**
     * @brief called when a mouse move event occurs on the view
     *
     *        Return true if you do not want the event to be propagated.
     */
    virtual bool mouseMoveEvent(QMouseEvent *e) { Q_UNUSED(e) return false; }

    /**
     * @brief called when a mouse press event occurs on the view
     *
     *        Return true if you do not want the event to be propagated.
     */
    virtual bool mousePressEvent(QMouseEvent *e) { Q_UNUSED(e) return false; }

    /**
     * @brief called when a mouse release event occurs on the view
     *
     *        Return true if you do not want the event to be propagated.
     */
    virtual bool mouseReleaseEvent(QMouseEvent *e) { Q_UNUSED(e) return false; }

    /**
     * @brief called when a key press event occurs on the view
     *
     *        Return true if you do not want the event to be propagated.
     */
    virtual bool keyPressEvent(QKeyEvent *e) { Q_UNUSED(e) return false; }

    /**
     * @brief called when a key release event occurs on the view
     *
     *        Return true if you do not want the event to be propagated.
     */
    virtual bool keyReleaseEvent(QKeyEvent *e) { Q_UNUSED(e) return false; }

    /**
     * @brief called when a wheel event occurs on the view
     *
     *        Return true if you do not want the event to be propagated.
     */
    virtual bool wheelEvent(QWheelEvent *e) { Q_UNUSED(e) return false; }

    /**
     * @brief By default translate static Type (TYPE_SELECTION, etc...).
     * @warning If you inherit this method don't forget to call the method of the superclass
     */
    virtual bool event(QEvent *event);

    /**
     * @brief Use this type in the method "type()" to return the same type
     *        as other actions. If you want really use your custom type it's ok.
     */
    static QString  TYPE_SELECTION;
    static QString  TYPE_MODIFICATION;
    static QString  TYPE_INFORMATION;
    static QString  TYPE_MEASURE;
    static QString  TYPE_OTHER;

    /**
     * @brief Return a new empty action
     */
    virtual CT_AbstractAction* createInstance() const = 0;

protected:

    /**
     * @brief Set your error if exist
     */
    void setLastError(const QString &error);

    /**
     * @brief Set finished or not (by default called in method "init()" and "finish()")
     */
    void setFinished(bool val);

    /**
     * @brief Register a CT_GAbstractActionOptions to be manage automatically. The method
     *        hideOptions() and showOptions() hide and show all options registered.
     */
    virtual void registerOption(CT_GAbstractActionOptions* options);

    /**
     * @brief Return the number of options registered
     */
    int nOptions() const;

    /**
     * @brief Return the ActionOptionsInterface registered by index 'i'
     */
    ActionOptionsInterface* optionAt(const int &i) const;

    /**
     * @brief Called when the document changed
     */
    virtual void documentChangedEvent() {}

private:
    DocumentInterface                   *m_doc;
    QString                             m_lastError;
    bool                                m_finished;
    bool                                m_suspended;
    QList<ActionOptionsInterface*>      m_options;
};

#endif // CT_ABSTRACTACTION_H
