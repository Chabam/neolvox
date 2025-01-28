#ifndef DOCUMENTINTERFACE_H
#define DOCUMENTINTERFACE_H

#include "ct_model/interfacesformodels.h"
#include "indocumentviewinterface.h"

class CT_AbstractItemDrawable;
class CT_AbstractResult;
class CT_AbstractAction;

/**
 * @brief Represent a document in the gui
 */
class DocumentInterface : public QObject, public IDocumentForModel
{
    Q_OBJECT	

public:
    virtual ~DocumentInterface() {}

    /**
      * \brief Title of the document
      */
    virtual QString getTitle() const = 0;

    /**
     * @brief The number of the document (unique)
     */
    virtual int getNumber() const = 0;

    /**
     * @brief Call this method if you plan to add multiple ItemDrawable in one times
     */
    virtual void beginAddMultipleItemDrawable() = 0;

    /**
      * \brief Ajout d'un ItemDrawable au document
      */
    virtual void addItemDrawable(CT_AbstractItemDrawable &item) = 0;

    /**
     * @brief Call this method after you add multiple ItemDrawable and if you have called the method 'beginAddMultipleItemDrawable'
     */
    virtual void endAddMultipleItemDrawable() = 0;

    /**
     * @brief Call this method if you plan to remove multiple ItemDrawable in one times
     */
    virtual void beginRemoveMultipleItemDrawable() = 0;

    /**
      * \brief Supprime l'ItemDrawable du document
      */
    virtual void removeItemDrawable(CT_AbstractItemDrawable &item) = 0;

    /**
     * @brief Call this method after you removed multiple ItemDrawable and if you have called the method 'beginRemoveMultipleItemDrawable'
     */
    virtual void endRemoveMultipleItemDrawable() = 0;

    /**
      * \brief Removes all ItemDrawable in the document that was in the result 'res'. Call the method 'beginRemoveMultipleItemDrawable' and 'endRemoveMultipleItemDrawable' automatically
      */
    virtual void removeAllItemDrawableOfResult(const CT_AbstractResult &res) = 0;

    /**
      * \brief Removes all ItemDrawable in the document that have the model 'model'. Call the method 'beginRemoveMultipleItemDrawable' and 'endRemoveMultipleItemDrawable' automatically
      */
    virtual void removeAllItemDrawableOfModel(const CT_OutAbstractModel &model) = 0;

    /**
      * \brief Removes all selected ItemDrawable in the document. Call the method 'beginRemoveMultipleItemDrawable' and 'endRemoveMultipleItemDrawable' automatically
      */
    virtual void removeAllSelectedItemDrawable() = 0;

    /**
      * \brief Removes all ItemDrawable in the document. Call the method 'beginRemoveMultipleItemDrawable' and 'endRemoveMultipleItemDrawable' automatically
      */
    virtual void removeAllItemDrawable() = 0;

    /**
      * \brief Select/DeSelect of ItemDrawable of the document
      */
    virtual void setSelectAllItemDrawable(bool select) = 0;

    /**
      * \brief Select/DeSelect of ItemDrawable of the document that have the model 'model'
      */
    virtual void setSelectAllItemDrawableOfModel(bool select, const CT_OutAbstractModel &model) = 0;

    /**
      * \brief Returns all ItemDrawable contained in this document
      */
    virtual const QList<CT_AbstractItemDrawable*>& getItemDrawable() const = 0;

    /**
      * \brief Returns all ItemDrawable selected
      */
    virtual QList<CT_AbstractItemDrawable*> getSelectedItemDrawable() const = 0;

    /**
     * @brief Returns true if the itemdrawable is contained in this document
     */
    virtual bool containsItemDrawable(const CT_AbstractItemDrawable *item) const = 0;

    /**
     * @brief Returns true if the itemdrawable or at least one children is contained in this document
     */
    virtual bool containsItemDrawableOrAtLeastOneChildren(const CT_AbstractItemDrawable *item) const = 0;

    /**
     * @brief Returns true if this document use item color.
     */
    virtual bool useItemColor() const = 0;

    /**
     * @brief Set the color of the item passed in parameter. Do nothing if useItemColor() return false.
     */
    virtual void setColor(const CT_AbstractItemDrawable *item, const QColor &color) = 0;

    /**
     * @brief Return the color of the item passed in parameter. Returns an invalid color if useItemColor() return false.
     */
    virtual QColor getColor(const CT_AbstractItemDrawable *item) = 0;

    /**
     * @brief Returns true if this document has the capacity of change the visibility of an itemdrawable
     */
    virtual bool canChangeVisibility() const = 0;

    /**
     * @brief Change the visibility of the item in the document
     */
    virtual void setVisible(const CT_AbstractItemDrawable *item, bool visible) = 0;

    /**
     * @brief Returns the visibility of the item in the document.
     */
    virtual bool isVisible(const CT_AbstractItemDrawable *item) const = 0;

public:
    /**
      * \brief Returns the number of ItemDrawable
      */
    virtual size_t nItemDrawable() const = 0;

    /**
      * \brief Return the ItemDrawable 'i' in the list
      */
    virtual CT_AbstractItemDrawable* getItemDrawable(int i) const = 0;

    /**
      * \brief Recherche des ItemDrawable dans la liste  partir du model pass en paramtre
      * \return une liste vide si aucun ItemDrawable n'est de ce type
      */
    virtual QList<CT_AbstractItemDrawable*> findItemDrawable(const CT_OutAbstractModel &model) const = 0;
    virtual void findItemDrawable(const CT_OutAbstractModel &model, QList<CT_AbstractItemDrawable*> &outList) const = 0;

    /**
      * \brief Recherche le premier ItemDrawable dans la liste a partir du modele
      * \return nullptr si aucun ItemDrawable n'est de ce type
      */
    virtual CT_AbstractItemDrawable* findFirstItemDrawable(const CT_OutAbstractModel &model) const = 0;

    /**
     * @brief Return true if the document accept this type of action
     */
    virtual bool acceptAction(const CT_AbstractAction *action) const = 0;

    /**
     * @brief Set the current action to the document. If the document contains graphics, each of
     *        them receives a copy of the action. If the document contains treeview, each of them
     *        receives a copy of the action. etc...
     *
     *        If the action don't exist in the actions manager it will be added automatically.
     *
     * @param action : action to set current
     * @param deleteAction : true if the action must be deleted if she exist in the actions manager (true by default)
     *
     * @return false if the document don't accept this type of action.
     */
    virtual bool setCurrentAction(CT_AbstractAction *action, bool deleteAction = true) = 0;

    /**
     * @brief Return the list of views in the document
     */
    virtual QList<InDocumentViewInterface*> views() const = 0;

    /**
      * \brief Refresh all graphics in the document (if he contains graphicsView)
      */
    virtual void redrawGraphics() = 0;

    virtual void updateItems(const QList<CT_AbstractItemDrawable*> &items) = 0;
};

/**
 * @brief Représente le gestionnaire des documents
 */
class DocumentManagerInterface
{
public:
    virtual ~DocumentManagerInterface() {}

    /**
     * @brief Create a new document with 3D View
     */
    virtual DocumentInterface* new3DDocument(float pointSize = 1.0, bool orthographic = false, QColor *color = nullptr) = 0;

    /**
     * @brief Create a new document with 2D View
     */
    virtual DocumentInterface* new2DDocument() = 0;

    /**
     * @brief Create a new document with TreeView
     */
    virtual DocumentInterface* newTreeViewDocument() = 0;

    /**
     * @brief Return the number of documents
     */
    virtual int nDocuments() const = 0;

    /**
     * @brief Return the document at index passed in parameter
     */
    virtual DocumentInterface* documentAt(const int &index) const = 0;

    /**
     * @brief Return the documents collection
     */
    virtual QList<DocumentInterface*> documents() const = 0;

    /**
     * @brief Return the active document
     */
    virtual DocumentInterface* activeDocument() = 0;

    /**
     * @brief Activate the document passed in parameter
     */
    virtual void activateDocument(DocumentInterface *doc) = 0;

    /**
     * @brief Close the document passed in parameter.
     *
     * @return false if it is the last document. One document must always be present.
     */
    virtual bool closeDocument(DocumentInterface *doc) = 0;

    /**
     * @brief Close the document at index passed in parameter.
     *
     * @return false if it is the last document. One document must always be present.
     */
    virtual bool closeDocument(const int &index) = 0;
};

#endif //DOCUMENTINTERFACE_H
