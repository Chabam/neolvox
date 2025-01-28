/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michal KREBS (ARTS/ENSAM)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/


#ifndef CT_ABSTRACTRESULT_H
#define CT_ABSTRACTRESULT_H

#include "ctlibstructure_global.h"
#include "ct_model/interfacesformodels.h"
#include "ct_model/outModel/abstract/ct_outabstractresultmodel.h"
#include "ct_item/abstract/ct_abstractitem.h"

#include <QList>
#include <QMutex>

class CT_AbstractItemDrawable;

/**
 * @brief Class that represent a result
 */
class CTLIBSTRUCTURE_EXPORT CT_AbstractResult : public PrototypeModelInterfaceInheritTool<CT_AbstractItem, IResultForModel>
{
    Q_OBJECT
public:

    CT_AbstractResult();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Displayable name
     *
     *        What is initialized differently :
     *          - Parent step is set to nullptr
     *          - Result model is set to nullptr
     *          - Number of items visible is set to 0
     *          - Clear from memory progress value is set to represent a result cleared
     */
    CT_AbstractResult(const CT_AbstractResult& other);
    virtual ~CT_AbstractResult();

    /**
     * @brief Set or not the result busy
     *
     * Call this method with "true" when you want to access to element of the result (per example if you iterate over group or item or ...)
     * Call this method with "false" when you have finished to access element
     *
     * When you call this method with true it will block your thread if another thread as set the result busy
     * You will be unlocked when the resource is available
     */
    void setBusy(bool busy);

    /**
     * @brief Returns true if another thread as set this result busy
     */
    bool isBusy() const;

    /**
        @brief Returns a unique type for a result
      * @warning DON'T FORGET to overload this method if you INHERIT from this class or a subclass
      */
    virtual QString type() const;

    /**
      * @brief Returns a unique type for a result
      * @warning DON'T FORGET to overload this method if you INHERIT from this class or a subclass
      */
    static QString staticType();

    /**
     * @brief Returns true if the result is empty
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Set the step which has created this result
     */
    void setParentStep(const IStepForModel* step);

    /**
     * @brief Returns the step which has created this result
     */
    IStepForModel* parentStep() const;

    /**
     * @brief Returns the plugin statically cast in the class that you want
     */
    template<class T = CT_VirtualAbstractStep>
    T* parentStepStaticCastT() const { return static_cast<T*>(parentStep()); }

    /**
     * @brief Returns true if at least one element of this result is visible in a document
     */
    bool isVisible() const;

    /**
     * @brief Set the result name (used in gui)
     */
    void setDisplayableName(const QString& name);

    /**
     * @brief Returns the result name. If the name is empty return the result of the method "getType"
     */
    QString displayableName() const;

    /**
     * @brief Returns a tooltip when the mouse cursor get over the result in gui. By default returns the
     *        displayable name.
     */
    virtual QString toolTip() const;

    /**
     * @brief Returns the clear memory progress (0 if clear is complete)
     */
    int clearFromMemoryProgressValue() const;

    /**
     * @brief Returns true if this result is cleared from memory (the result is empty)
     */
    bool isClearedFromMemory() const;

    /**
     * @brief Clear this result : remove all elements from this result
     */
    void clearFromMemory();

    /**
     * @brief Call it when you have finished to fill the result and you will never add or remove something from it or in the
     *        struture . The method "isClearedFromMemory" will returns false after that.
     */
    virtual void setComplete();

    /**
     * @brief Called from CT_AbstractItemDrawable to inform the result that a new item is added to a document.
     *        The signal visibilityChange(true); will be emitted if the number of item visible change to 1
     */
    void incrementItemVisible();

    /**
     * @brief Called from CT_AbstractItemDrawable to inform the result that a new item is removed from a document.
     *        The signal visibilityChange(false); will be emitted if the number of item visible change to 0
     */
    void decrementItemVisible();

    /**
     * @brief Returns the tool for model
     */
    IPrototypeToolForModel* prototypeToolForModel() const override;

    /**
     * @brief Returns the tool for model
     */
    IResultToolForModel* resultToolForModel() const override;

    /**
     * @brief Returns a copy of this result and use the new model to know what must be copied or not.
     * @param newModelToUse : the new model to use and to set to the copy
     * @return A copy of this result. If somewhere in the hierarchy somethiing is missing it will not be copied.
     */
    virtual CT_AbstractResult* copy(const CT_OutAbstractResultModel* newModelToUse) const = 0;

protected:
    /**
     * @brief Overload this method to clear elements of your result
     */
    virtual void internalClearFromMemory() = 0;

protected slots:
    /**
     * @brief Call this method to inform the progress of the clear memory process
     */
    void setClearFromMemoryProgress(int progress);

private:

    class ResultToolForModel : public IPrototypeToolForModel, public IResultToolForModel {
    public:
        CT_AbstractResult* m_pointer;

        void setModel(const CT_OutAbstractModel* model) override {
            m_pointer->setModel(model);
        }

        CT_OutAbstractModel* model() const override {
            return m_pointer->model();
        }

        void setResultName(const QString& name) override {
            m_pointer->setDisplayableName(name);
        }

        CT_OutAbstractResultModel* resultModel() const override {
            return static_cast<CT_OutAbstractResultModel*>(m_pointer->model());
        }

        IResultForModel* copyResult(const CT_OutAbstractResultModel* newModelToUse) const override {
            return m_pointer->copy(newModelToUse);
        }
    };

    /**
     * @brief The tool for model
     */
    ResultToolForModel          m_resultTool;

    /**
     * @brief The step which has created this result
     */
    IStepForModel*              m_parentStep;

    /**
     * @brief A mutex to lock the result when it was busy
     */
    QMutex*                     m_mutexBusy;

    /**
     * @brief The displayable name
     */
    QString                     m_displayableName;

    /**
     * @brief Current number of items of this result visible (added to documents)
     */
    int                         m_numberOfItemsVisible;

    /**
     * @brief Current clear memory progress value
     */
    int                         m_clearFromMemoryProgressValue;

signals:
    /**
     * @brief Emitted if at least one element is visible in at least one document (visible = true) or when the last element visible in the last document
     *        was removed (visible = false)
     */
    void visibilityChange(bool visible);

    /**
     * @brief Emitted when the clear memory progress value change
     */
    void clearFromMemoryInProgress(int progress);

    /**
     * @brief Emitted when the result busy state change
     */
    void busyStateChanged(bool busy);
};

#endif // CT_ABSTRACTRESULT_H
