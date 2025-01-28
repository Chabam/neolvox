#ifndef GRAPHICSVIEWINTERFACE_H
#define GRAPHICSVIEWINTERFACE_H

#include "indocumentviewinterface.h"

#include <Eigen/Core>

#include <QColor>
#include <QtOpenGL/QGL>

class CT_Normal;
class CT_Color;
class ActionOptionsInterface;
class CameraInterface;
class CT_AbstractItemDrawable;
class CT_OutAbstractItemModel;
class CT_AbstractCloudIndexRegistered;
class CT_StandardColorCloudRegistered;
class CT_StandardNormalCloudRegistered;

using CT_SPCIR = QSharedPointer<CT_AbstractCloudIndexRegistered>;
using CT_SFCIR = QSharedPointer<CT_AbstractCloudIndexRegistered>;
using CT_SECIR = QSharedPointer<CT_AbstractCloudIndexRegistered>;


// If you change object type in AMKgl Viewer you must change it here too !
enum GraphicsObjectType {
    GOT_Point = 0,              // local point (not from global point cloud)
    GOT_Ellipse,                // ellipse
    GOT_Line,                   // line
    GOT_Box,                    // box or voxel
    GOT_Sphere,                 // sphere
    GOT_Cylinder,               // cylinder
    GOT_Quads,                  // rectangle
    GOT_Triangle,               // triangle
    GOT_Pyramid,                // pyramid
    GOT_PointGlobalCloud,       // point from global point cloud
    GOT_LineGlobalCloud,        // line (edges of mesh) from global point cloud
    GOT_TriangleGlobalCloud     // triangle (faces of mesh) from global point cloud
};

// Flags that can be used to set an object selected or visible or set a specified developper flags in
// action per example.
// !!! Selected and Visible must not be modified !!!
enum GraphicsObjectFlag {
    GOF_Selected = 1,
    GOF_Visible = 2,
    GOF_DevelopperFlag1 = 4,
    GOF_DevelopperFlag2 = 8,
    GOF_DevelopperFlag3 = 16,
    GOF_DevelopperFlag4 = 32,
    GOF_DevelopperFlag5 = 64,
    GOF_Reserved = 128
};

Q_DECLARE_FLAGS(GraphicsObjectFlags, GraphicsObjectFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicsObjectFlags)

/**
 * @brief Property of a GraphicsObjectFlag (see GraphicsViewInterface for more information)
 */
struct GraphicsObjectFlagProperty
{
    /**
     * @brief Color of objects when the flag is set
     */
    QColor onColor;
};

/**
 * @brief With this class you can query the status of an object
 */
class ObjectStatusQueryiorInterface
{
public:
    virtual ~ObjectStatusQueryiorInterface() {}

    /**
     * @brief Returns the number of objects available
     */
    virtual size_t getNumberOfObjects() const = 0;

    /**
     * @brief Returns the number of vertex per object
     */
    virtual size_t getNumberOfVertexPerObject() const = 0;

    /**
     * @brief Returns true if informations (visible, selected) is available, false otherwise
     */
    virtual bool isInfosAvailable() const = 0;

    /**
     * @brief Returns if the specified object is visible or not
     * @warning If method "isInfosAvailable" return false this method will return an
     *          invalid value and generate an assert in debug mode
     */
    virtual bool isVisible(const size_t& objectIndex) const = 0;

    /**
     * @brief Returns if the specified object is selected or not
     * @warning If method "isInfosAvailable" return false this method will return an
     *          invalid value and generate an assert in debug mode
     */
    virtual bool isSelected(const size_t& objectIndex) const = 0;

    /**
     * @brief Returns if the flag of the specified object is on or not
     * @warning If method "isInfosAvailable" return false this method will return an
     *          invalid value and generate an assert in debug mode
     */
    virtual bool isFlagOn(const size_t& objectIndex,
                          const GraphicsObjectFlag& flag) const = 0;

    /**
     * @brief Returns the complete flags value of the specified object
     */
    virtual GraphicsObjectFlags getFlagsValue(const size_t& objectIndex) const = 0;

    /**
     * @brief Returns true if colors is available, false otherwise
     */
    virtual bool isColorsAvailable() const = 0;

    /**
     * @brief Returns the color of the specified vertex of the specified object
     * @warning If method "isColorsAvailable" return false this method will return an
     *          invalid value and generate an assert in debug mode
     */
    virtual const CT_Color& getColor(const size_t& objectIndex,
                                     const size_t& vertexIndex) const = 0;

    /**
     * @brief Returns true if normals is available, false otherwise
     */
    virtual bool isNormalsAvailable() const = 0;

    /**
     * @brief Returns the normal of the specified vertex of the specified object
     * @warning If method "isNormalsAvailable" return false this method will return an
     *          invalid value and generate an assert in debug mode
     */
    virtual const CT_Normal& getNormal(const size_t& objectIndex,
                                       const size_t& vertexIndex) const = 0;
};

/**
 * @brief With this class you can modify an object
 */
class ObjectsModifierInterface : public ObjectStatusQueryiorInterface
{
public:
    virtual ~ObjectsModifierInterface() {}

    /**
     * @brief Apply a transformation matrix to all vertex of all objects. Use it if you want
     *        to translate/rotate/scale objects.
     *
     *        Do nothing for global points/edges/faces !
     *
     * @warning Be carreful when you translate or scale an object because if its
     *          coordinates change from one coordinate system to another the system
     *          won't detect it and the draw of it can be altered !
     */
    virtual void transform(const Eigen::Matrix4f& trMatrix) = 0;

    /**
     * @brief Apply a translation to objects.
     *
     *        Do nothing for global points/edges/faces !
     *
     * @warning Be carreful when you translate an object because if its
     *          coordinates change from one coordinate system to another the system
     *          won't detect it and the draw of the object can be altered !
     */
    virtual void translate(const Eigen::Vector3f& translation) = 0;

    /**
     * @brief Apply a rotation to objects.
     *
     *        Do nothing for global points/edges/faces !
     */
    virtual void rotate(const Eigen::AngleAxis<float>& rotation) = 0;
    virtual void rotate(const Eigen::Quaternion<float>& rotation) = 0;

    /**
     * @brief Scale objects.
     *
     *        Do nothing for global points/edges/faces !
     *
     * @warning Be carreful when you scale the object because if its
     *          coordinates change from one coordinate system to another the system
     *          won't detect it and the draw of the object can be altered !
     */
    virtual void scale(const Eigen::Vector3f& scaling) = 0;

    /**
     * @brief Apply a transformation matrix to all vertex of the specified objects. Use it if you want
     *        to translate/rotate/scale objects.
     *
     *        Do nothing for global points/edges/faces !
     *
     * @warning Be carreful when you translate or scale an object because if its
     *          coordinates change from one coordinate system to another the system
     *          won't detect it and the draw of it can be altered !
     */
    virtual void transform(const size_t& objectIndex,
                           const Eigen::Matrix4f& trMatrix) = 0;

    /**
     * @brief Apply a translation to the specified object.
     *
     *        Do nothing for global points/edges/faces !
     *
     * @warning Be carreful when you translate an object because if its
     *          coordinates change from one coordinate system to another the system
     *          won't detect it and the draw of the object can be altered !
     */
    virtual void translate(const size_t& objectIndex,
                           const Eigen::Vector3f& translation) = 0;

    /**
     * @brief Apply a rotation to the specified object.
     *
     *        Do nothing for global points/edges/faces !
     */
    virtual void rotate(const size_t& objectIndex,
                        const Eigen::AngleAxis<float>& rotation) = 0;
    virtual void rotate(const size_t& objectIndex,
                        const Eigen::Quaternion<float>& rotation) = 0;

    /**
     * @brief Scale the specified object.
     *
     *        Do nothing for global points/edges/faces !
     *
     * @warning Be carreful when you scale the object because if its
     *          coordinates change from one coordinate system to another the system
     *          won't detect it and the draw of the object can be altered !
     */
    virtual void scale(const size_t& objectIndex,
                       const Eigen::Vector3f& scaling) = 0;

    /**
     * @brief Set the new color to all vertex of all objects
     */
    virtual void setColor(const CT_Color& newColor) = 0;

    /**
     * @brief Set the new color to all vertex of the specified object
     */
    virtual void setColor(const size_t& objectIndex,
                          const CT_Color& newColor) = 0;

    /**
     * @brief Set the new color to the specified vertex of the specified object
     */
    virtual void setColor(const size_t& objectIndex,
                          const size_t& vertexIndex,
                          const CT_Color& newColor) = 0;

    /**
     * @brief Set the new normal to all objects
     */
    virtual void setNormal(const CT_Normal& newNormal) = 0;

    /**
     * @brief Set the new normal to the specified object
     */
    virtual void setNormal(const size_t& objectIndex,
                           const CT_Normal& newNormal) = 0;

    /**
     * @brief Set the new normal to the specified vertex of the specified object
     */
    virtual void setNormal(const size_t& objectIndex,
                           const size_t& vertexIndex,
                           const CT_Normal& newNormal) = 0;

    /**
     * @brief Set if all objects is visible or not
     */
    virtual void setVisible(bool visible) = 0;

    /**
     * @brief Set if the specified object is visible or not
     */
    virtual void setVisible(const size_t& objectIndex,
                            bool visible) = 0;

    /**
     * @brief Toggle all objects visibility
     */
    void toggleVisibility() { toggleFlags(GOF_Visible); }

    /**
     * @brief Toggle the specified object visibility
     */
    void toggleVisibility(const size_t& objectIndex) { toggleFlags(objectIndex, GOF_Visible); }

    /**
     * @brief Set if all objects is selected or not
     */
    virtual void setSelected(bool selected) = 0;

    /**
     * @brief Set if the specified object is selected or not
     */
    virtual void setSelected(const size_t& objectIndex,
                             bool selected) = 0;
    /**
     * @brief Toggle all objects selection
     */
    void toggleSelection() { toggleFlags(GOF_Selected); }

    /**
     * @brief Toggle the specified object selection
     */
    void toggleSelection(const size_t& objectIndex) { toggleFlags(objectIndex, GOF_Selected); }

    /**
     * @brief Set one or multiple flags on/off in the same time for all objects.
     */
    virtual void setFlags(const GraphicsObjectFlags& flags,
                          bool on) = 0;

    /**
     * @brief Set one or multiple flags on/off in the same time for the specified object.
     */
    virtual void setFlags(const size_t& objectIndex,
                          const GraphicsObjectFlags& flags,
                          bool on) = 0;
    /**
     * @brief Toggle one or multiple flags on/off in the same time for all objects.
     */
    virtual void toggleFlags(const GraphicsObjectFlags& flags) = 0;

    /**
     * @brief Toggle one or multiple flags on/off in the same time for the specified object.
     */
    virtual void toggleFlags(const size_t& objectIndex,
                             const GraphicsObjectFlags& flags) = 0;
};

/**
 * @brief This class is provided by the gui to follow you
 *        when you visit objects. It will be useful to modify or get
 *        access to information of objects.
 */
class ObjectsVisitorFollowerInterface
{
public:
    virtual ~ObjectsVisitorFollowerInterface() {}

    /**
     * @brief Returns the number of objects available
     */
    virtual size_t getNumberOfObjects() const = 0;

    /**
     * @brief Returns the number of vertex per object
     */
    virtual size_t getNumberOfVertexPerObject() const = 0;

    /**
     * @brief Returns the current object type
     */
    virtual GraphicsObjectType currentObjectType() const = 0;

    /**
     * @brief Returns the current item model used
     */
    virtual CT_OutAbstractItemModel* currentItemModel() const = 0;

    /**
     * @brief Create an interface that allow the developper to query the status of all new objects
     */
    virtual QSharedPointer<ObjectStatusQueryiorInterface> createObjectsStatusQueryior() = 0;

    /**
     * @brief Create an interface that allow the developper to modify all new objects
     */
    virtual QSharedPointer<ObjectsModifierInterface> createObjectsModifier() = 0;
};

/**
 * @brief Inherit from this class in your plugin to visit objects
 *        in a scene
 */
class ObjectsVisitorInterface
{
public:
    virtual ~ObjectsVisitorInterface() {}

    /**
     * @brief Set the follower that allow ou to get some informations and the possibility
     *        to create a objects modifier
     */
    virtual void setFollower(const ObjectsVisitorFollowerInterface* follower) = 0;

    /**
     * @brief You must return true if you want to continue the visit, false to abort it
     */
    virtual bool continueVisit() const = 0;

    /**
     * @brief Called when new objects is available.
     *
     *        If you want to modify colors/normals/visibility/etc.... you must call the
     *        method "createObjectsModifier" that allow you to do this type of operations.
     *
     *        If you want to know the status of elements you must call the method
     *        "createObjectsStatusQueryior" that allow you to do this type of operations.
     */
    virtual void newObjectsAvailable() = 0;
};

/*!
 *  \brief Représente les options de la vue
 */
class GraphicsViewOptionsInterface
{
public:
    virtual ~GraphicsViewOptionsInterface() {}

    virtual QColor getSelectedColor() const = 0;
    virtual QColor getBackgroundColor() const = 0;
    virtual float getPointSize() const = 0;
    virtual bool drawAxis() const = 0;
    virtual bool drawGrid() const = 0;
    virtual bool useColor() const = 0;
    virtual bool useTransparency() const = 0;
    virtual bool mustUseLight() const = 0;
    virtual QString getCameraTypeAsString() const = 0;
    virtual int getFastDrawTime() const = 0;

    virtual void setSelectedColor(QColor &color) { Q_UNUSED(color) }
    virtual void setBackgroudColor(QColor &color) { Q_UNUSED(color) }
    virtual void setPointSize(float size) { Q_UNUSED(size) }
    virtual void setDrawAxis(bool draw) { Q_UNUSED(draw) }
    virtual void setDrawGrid(bool draw) { Q_UNUSED(draw) }
    virtual void setUseColor(bool enable) { Q_UNUSED(enable) }
    virtual void setUseTransparency(bool use) { Q_UNUSED(use) }
    virtual void setUseLight(bool use) { Q_UNUSED(use) }
    virtual void setCameraType(QString type) { Q_UNUSED(type) }

};

class GraphicsViewInterface;

/**
 * @brief Represent a manager of signals for the GraphicsViewInterface
 */
class GraphicsViewSignalEmitterInterface : public QObject
{
    Q_OBJECT

public:
    virtual ~GraphicsViewSignalEmitterInterface() {}
    virtual GraphicsViewInterface* graphicsView() const = 0;

signals:
    void drawingStarted();
    void drawingFinished();
};


/**
 * @brief Represent a graphics view in a document
 */
class GraphicsViewInterface : public InDocumentViewInterface
{
public:
    // Selection mode
    enum SelectionMode{
        // don't change the order !!! for a serie the first must always be SELECT_... and the last REMOVE_ONE_...

        NONE = 0,       // do nothing
        SELECT,         // make a new selection, all itemdrawable selected before was removed from list
        ADD,            // add new itemdrawable selected to list
        REMOVE,         // remove new itemdrawable selected from list
        SELECT_ONE,     // add first itemdrawable selected to the list and remove other
        ADD_ONE,        // add first itemdrawable selected to the list
        REMOVE_ONE,     // remove first itemdrawable selected from the list
        // same but for points
        SELECT_POINTS,
        ADD_POINTS,
        REMOVE_POINTS,
        SELECT_ONE_POINT,
        ADD_ONE_POINT,
        REMOVE_ONE_POINT,
        // same but for faces
        SELECT_FACES,
        ADD_FACES,
        REMOVE_FACES,
        SELECT_ONE_FACE,
        ADD_ONE_FACE,
        REMOVE_ONE_FACE,
        // same but for edges
        SELECT_EDGES,
        ADD_EDGES,
        REMOVE_EDGES,
        SELECT_ONE_EDGE,
        ADD_ONE_EDGE,
        REMOVE_ONE_EDGE
    };

    // colors of elements
    enum ColorCloudType {
        CPointCloud,            // colors of points
        CFaceCloud,             // colors of faces
        CEdgeCloud              // colors of edges
    };

    // normals of elements
    enum NormalCloudType {
        NPointCloud,            // normals of points
        NFaceCloud,             // normals of faces
        NEdgeCloud              // normals of edges
    };

    // type of graphics view
    enum GraphicsViewType {
        GV2D,                   // 2D
        GV3D                    // 3D
    };

    virtual ~GraphicsViewInterface() {}

    virtual void validateOptions() {}


    /**
     * @brief Return the signal emitter of the graphics view
     */
    virtual GraphicsViewSignalEmitterInterface* signalEmitter() const = 0;

    /**
     * @brief Return the options of the graphics view
     */
    virtual GraphicsViewOptionsInterface& getOptions() = 0;

    /**
     * @brief Set the options of the graphics view
     */
    virtual void setOptions(const GraphicsViewOptionsInterface* options) = 0;

    /**
     * @brief Return the color cloud for the type passed in parameter. Can be nullptr if it was not created.
     */
    virtual QSharedPointer<CT_StandardColorCloudRegistered> colorCloudOf(ColorCloudType type) const = 0;

    /**
     * @brief Return the normal cloud for the type passed in parameter. Can be nullptr if it was not created.
     */
    virtual QSharedPointer<CT_StandardNormalCloudRegistered> normalCloudOf(NormalCloudType type) const = 0;

    /**
     * @brief Change the color of the point. You must call dirtyColorsOfItemDrawablesWithPoints() if you want that change was visible.
     * @param globalIndexOfPoint : the global index of the point ot change his color
     * @param color : the new color
     */
    virtual void setColorOfPoint(const size_t &globalIndexOfPoint, const CT_Color &color) = 0;

    /**
     * @brief Returns the color of the point.
     * @warning if you change the value of the color you must call dirtyColorsOfItemDrawablesWithPoints() to update it.
     * @param globalIndexOfPoint : the global index of the point
     * @return the color of the point
     */
    virtual CT_Color& colorOfPoint(const size_t &globalIndexOfPoint) = 0;

    /**
     * @brief Returns the color of the point.
     * @param globalIndexOfPoint : the global index of the point
     * @return the color of the point
     */
    virtual const CT_Color& constColorOfPoint(const size_t &globalIndexOfPoint) const = 0;

    /**
     * @brief Change the normal of the point. You must call dirtyNormalsOfItemDrawablesWithPoints() if you want that change was visible.
     * @param globalIndexOfPoint : the global index of the point ot change his normal
     * @param normal : the new normal
     */
    virtual void setNormalOfPoint(const size_t &globalIndexOfPoint, const CT_Normal &normal) = 0;

    /**
     * @brief Returns the normal of the point.
     * @warning if you change the value of the normal you must call dirtyNormalsOfItemDrawablesWithPoints() to update it.
     * @param globalIndexOfPoint : the global index of the point
     * @return the normal of the point
     */
    virtual CT_Normal& normalOfPoint(const size_t &globalIndexOfPoint) = 0;

    /**
     * @brief Returns the normal of the point.
     * @param globalIndexOfPoint : the global index of the point
     * @return the normal of the point
     */
    virtual const CT_Normal& constNormalOfPoint(const size_t &globalIndexOfPoint) const = 0;

    /**
     * @brief Update colors of all elements in the graphics view that use global points
     */
    virtual void dirtyColorsOfItemDrawablesWithPoints() = 0;

    /**
     * @brief Update normals of all elements in the graphics view that use global points
     */
    virtual void dirtyNormalsOfItemDrawablesWithPoints() = 0;

    /**
     * @brief Modify the property of the specified flag for all objects
     * @param flag : the flag to change
     * @param property : the new property to use
     */
    virtual void setFlagProperty(const GraphicsObjectFlag& flag,
                                 const GraphicsObjectFlagProperty& property) = 0;

    /**
     * @brief Modify the property of the specified flag for the specified object
     * @param flag : the flag to change
     * @param typeOfObject : the type of the object that will use this property
     * @param property : the new property to use
     */
    virtual void setFlagProperty(const GraphicsObjectFlag& flag,
                                 const GraphicsObjectType& typeOfObject,
                                 const GraphicsObjectFlagProperty& property) = 0;

    /**
     * @brief Returns the flag property currently used for the specified flag and specified object
     */
    virtual GraphicsObjectFlagProperty getFlagProperty(const GraphicsObjectFlag& flag,
                                                       const GraphicsObjectType& typeOfObject) const = 0;

    /**
     * @brief Returns the flag property currently used for the specified flag and all objects
     * @param ok: false if one object type don't use the same property, true if all objects type use the same property.
     * @warning if "ok" is false the property returned can be undetermined
     */
    virtual GraphicsObjectFlagProperty getFlagProperty(const GraphicsObjectFlag& flag,
                                                       bool& ok) const = 0;

    /**
     * @brief Return the camera of the view
     */
    virtual CameraInterface* camera() const = 0;

    /**
     * @brief Return the type of the view
     */
    virtual GraphicsViewType type() const = 0;

    /**
     * @brief Set the selection mode
     */
    virtual void setSelectionMode(SelectionMode mode) = 0;

    /**
     * @brief Get the selection mode
     */
    virtual GraphicsViewInterface::SelectionMode selectionMode() const = 0;

    /**
     * @brief Returns true if the view must select points
     */
    virtual bool mustSelectPoints() const = 0;

    /**
     * @brief Returns true if the view must select edges
     */
    virtual bool mustSelectEdges() const = 0;

    /**
     * @brief Returns true if the view must select faces
     */
    virtual bool mustSelectFaces() const = 0;

    /**
     * @brief Returns true if the view must select items
     */
    virtual bool mustSelectItems() const = 0;

    /**
     * @brief Performs a selection in the scene with a polygon.
     */
    virtual void select(const QPolygon& polygon) = 0;

    /**
     * @brief Returns all points (indices) selected. Can be nullptr if no points where selected.
     */
    virtual CT_SPCIR getSelectedPoints() const = 0;

    /**
     * @brief Returns all faces (indices) selected. Can be nullptr if no faces where selected.
     */
    virtual CT_SFCIR getSelectedFaces() const = 0;

    /**
     * @brief Returns all edges (indices) selected. Can be nullptr if no edges where selected.
     */
    virtual CT_SECIR getSelectedEdges() const = 0;

    /**
     * @brief Returns all ItemDrawable selected
     */
    virtual QList<CT_AbstractItemDrawable*> getSelectedItems() const = 0;

    /**
     * @brief Add a point to selection. Pass the global index of the point.
     */
    virtual void addPointsIDToSelection(const size_t &id) = 0;

    /**
     * @brief Add multiple point to selection. Pass a vector of global index of points to add.
     */
    virtual void addMultiplePointsIDToSelection(const std::vector<size_t> &idList) = 0;

    /**
     * @brief Add a face to selection. Pass the global index of the face.
     */
    virtual void addFacesIDToSelection(const size_t &id) = 0;

    /**
     * @brief Add multiple face to selection. Pass a vector of global index of faces to add.
     */
    virtual void addMultipleFacesIDToSelection(const std::vector<size_t> &idList) = 0;

    /**
     * @brief Add a edge to selection. Pass the global index of the point.
     */
    virtual void addEdgesIDToSelection(const size_t &id) = 0;

    /**
     * @brief Add multiple edge to selection. Pass a vector of global index of edges to add.
     */
    virtual void addMultipleEdgesIDToSelection(const std::vector<size_t> &idList) = 0;

    /**
     * @brief Remove a point to selection. Pass the global index of the point.
     */
    virtual void removePointsIDFromSelection(const size_t &id) = 0;

    /**
     * @brief Remove multiple point to selection. Pass a vector of global index of points to remove.
     */
    virtual void removeMultiplePointsIDFromSelection(const std::vector<size_t> &idList) = 0;

    /**
     * @brief Remove a face to selection. Pass the global index of the face.
     */
    virtual void removeFacesIDFromSelection(const size_t &id) = 0;

    /**
     * @brief Remove multiple face to selection. Pass a vector of global index of faces to remove.
     */
    virtual void removeMultipleFacesIDFromSelection(const std::vector<size_t> &idList) = 0;

    /**
     * @brief Remove a edge to selection. Pass the global index of the point.
     */
    virtual void removeEdgesIDFromSelection(const size_t &id) = 0;

    /**
     * @brief Remove multiple edge to selection. Pass a vector of global index of edges to remove.
     */
    virtual void removeMultipleEdgesIDFromSelection(const std::vector<size_t> &idList) = 0;

    /**
     * @brief Call this method with true if you want to select all points, false if you want to unselect all points
     */
    virtual void setAllPointsSelected(bool select) = 0;

    /**
     * @brief Call this method with true if you want to select all faces, false if you want to unselect all faces
     */
    virtual void setAllFacesSelected(bool select) = 0;

    /**
     * @brief Call this method with true if you want to select all edges, false if you want to unselect all edges
     */
    virtual void setAllEdgesSelected(bool select) = 0;

    /**
     * @brief Toggle the selection of items
     */
    virtual void toggleItemsSelected() = 0;

    /**
     * @brief Toggle the selection of points
     */
    virtual void togglePointsSelected() = 0;

    /**
     * @brief Toggle the selection of faces
     */
    virtual void toggleFacesSelected() = 0;

    /**
     * @brief Toggle the selection of edges
     */
    virtual void toggleEdgesSelected() = 0;

    /**
     * @brief Returns how many points is drawn
     */
    virtual size_t countPoints() = 0;

    /**
     * @brief Returns how many edges is drawn
     */
    virtual size_t countEdges() = 0;

    /**
     * @brief Returns how many faces is drawn
     */
    virtual size_t countFaces() = 0;

    /**
     * @brief Returns how many items is drawn
     */
    virtual size_t countItems() = 0;

    /**
     * @brief Call to visit all type of objects
     * @param visitor : is an object that will receive all objects
     */
    virtual void visitObjects(ObjectsVisitorInterface& visitor) = 0;

    /**
     * @brief Call to visit objects
     * @brief type : type of object to visit
     * @param visitor : is an object that will receive all objects
     *
     * @example scene->visitObjects(QList<GraphicsObjectType>() << GOT_Quads << GOT_Triangle, myVisitor);
     */
    virtual void visitObjects(const QList<GraphicsObjectType>& types,
                              ObjectsVisitorInterface& visitor) = 0;

    /**
     * @brief Create an interface that allow the developper to modify all objects
     *        of the specified type
     */
    virtual QSharedPointer<ObjectsModifierInterface> createObjectsModifier(GraphicsObjectType type) = 0;

    /**
     * @brief Create an interface that allow the developper to query the status of all objects
     *        of the specified type
     */
    virtual QSharedPointer<ObjectStatusQueryiorInterface> createObjectsStatusQueryior(GraphicsObjectType type) = 0;

    /**
     * @brief Call to visit all type of objects
     * @param visitor : is an object that will receive all objects
     */
    virtual void visitObjectsOfItem(const CT_AbstractItemDrawable* item,
                                    ObjectsVisitorInterface& visitor) = 0;

    /**
     * @brief Call to visit objects
     * @brief type : type of object to visit
     * @param visitor : is an object that will receive all objects
     *
     * @example scene->visitObjects(QList<GraphicsObjectType>() << GOT_Quads << GOT_Triangle, myVisitor);
     */
    virtual void visitObjectsOfItem(const CT_AbstractItemDrawable* item,
                                    const QList<GraphicsObjectType>& types,
                                    ObjectsVisitorInterface& visitor) = 0;

    /**
     * @brief Create an interface that allow the developper to modify all objects
     *        of the specified type for one item
     */
    virtual QSharedPointer<ObjectsModifierInterface> createObjectsModifierForItem(const CT_AbstractItemDrawable* item,
                                                                                  GraphicsObjectType type) = 0;

    /**
     * @brief Call to visit all type of objects of the current action
     * @param uniqueIndex : the unique index of the elements to visit or -1 to visit all objects of all elements.
     * @param visitor : is an object that will receive all objects
     */
    virtual void visitObjectsOfCurrentAction(const int& uniqueIndex,
                                             ObjectsVisitorInterface& visitor) = 0;

    /**
     * @brief Call to visit objects of the current action
     * @param uniqueIndex : the unique index of the elements to visit or -1 to visit all objects of all elements.
     * @brief type : type of object to visit
     * @param visitor : is an object that will receive all objects
     *
     * @example scene->visitObjects(QList<GraphicsObjectType>() << GOT_Quads << GOT_Triangle, myVisitor);
     */
    virtual void visitObjectsOfCurrentAction(const int& uniqueIndex,
                                             const QList<GraphicsObjectType>& types,
                                             ObjectsVisitorInterface& visitor) = 0;

    /**
     * @brief Returns the coordinates of the 3D point located at pixel (x,y) on screen.
     *
     * Calls a glReadPixel to get the pixel depth and applies an unprojectedCoordinatesOf() to
     * the result. found indicates whether a point was found or not (i.e. background pixel, result's
     * depth is zFar() in that case).
     *
     * x and y are expressed in pixel units with an origin in the upper left corner.
     */
    virtual Eigen::Vector3d pointUnderPixel(const QPoint &pixel, bool &found) const = 0;

    /**
     * @brief Gives the coefficients of a 3D half-line passing through the Camera eye and pixel (x,y).
     *
     * The origin of the half line (eye position) is stored in orig, while dir contains the properly oriented and normalized direction of the half line.
     *
     * x and y are expressed in Qt format (origin in the upper left corner).
     *
     * This method is useful for analytical intersection in a selection method.
     */
    virtual void convertClickToLine(const QPoint &pixel, Eigen::Vector3d &orig, Eigen::Vector3d &dir) const = 0;

    /**
     * @brief Returns the screen projected coordinates (pixel) of the variable position.
     */
    virtual void convert3DPositionToPixel(const Eigen::Vector3d &position, QPoint &pixel) const = 0;

    /**
     * @brief Add option of a action to the GraphicsView
     *
     *        If you want to remove the widget from the view you can delete it or
     *        call method "removeActionOptions"
     *
     * @warning You must be in the GUI thread to call this method !
     */
    virtual void addActionOptions(ActionOptionsInterface *options) = 0;

    /**
     * @brief Delete the widget. If you want to hide them call
     *        method "QWidget::hide()".
     */
    virtual void removeActionOptions(ActionOptionsInterface *options) = 0;

    /**
     * @brief eturn the width of the graphics
     */
    virtual int width() const = 0;

    /**
     * @brief Return the height of the graphics
     */
    virtual int height() const = 0;

    /**
     * @brief Allow to force 2D mode (top view and orthographic)
     */
    virtual void active2DView(bool e) = 0;

    /**
     * @brief get intermediate color from current selected gradient
     * @param key : number between 0 and 1, specifing color to retrieve
     */
    virtual QColor intermediateColorFromSelectedGradient(double key) = 0;


};

class CameraInterface : public QObject
{
    Q_OBJECT

public:
    enum CameraType {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    virtual ~CameraInterface() {}

    virtual double x() const = 0;
    virtual double y() const = 0;
    virtual double z() const = 0;
    virtual double cx() const = 0;
    virtual double cy() const = 0;
    virtual double cz() const = 0;
    virtual double rx() const = 0;
    virtual double ry() const = 0;
    virtual double rz() const = 0;

    virtual CameraInterface::CameraType type() const = 0;

    /**
     * @brief Returns the physical distance between the user's eyes and the screen (in meters).
     */
    virtual float focusDistance() const = 0;

    virtual const GLdouble* orientationMatrix() const = 0;
    virtual const GLdouble* orientationMatrixInversed() const = 0;

    virtual void modelViewMatrix(GLdouble m[16]) const = 0;

    // returns quaternion
    virtual void getOrientation(double &q0, double &q1, double &q2, double &q3) const = 0;

    /**
     * @brief Returns the normalized up vector of the Camera, defined in the world coordinate system.
     *       It corresponds to the Y axis.
     */
    virtual Eigen::Vector3d upVector() const = 0;

    /**
     * @brief Returns the normalized right vector of the Camera, defined in the world coordinate system.
     *       It corresponds to the X axis.
     */
    virtual Eigen::Vector3d rightVector() const = 0;

public slots:
    virtual void setX(double x) = 0;
    virtual void setY(double y) = 0;
    virtual void setZ(double z) = 0;

    virtual void setCX(double cx) = 0;
    virtual void setCY(double cy) = 0;
    virtual void setCZ(double cz) = 0;

    virtual void setRX(double xRot) = 0;
    virtual void setRY(double yRot) = 0;
    virtual void setRZ(double zRot) = 0;

    virtual void setType(CameraInterface::CameraType type) = 0;
    virtual void setPosition(double x, double y, double z, bool redrawView = true) = 0;
    virtual void setSceneCenter(double cx, double cy, double cz, bool redrawView = true) = 0;
    virtual void setViewDirection(double rx, double ry, double rz, bool redrawView = true) = 0;
    virtual void setOrientation(double q0, double q1, double q2, double q3, bool redrawView = true) = 0;

    virtual void showEntireScene() = 0;
    virtual void homePosition() = 0;
    virtual void alignCameraToXAxis() = 0;
    virtual void alignCameraToYAxis() = 0;
    virtual void alignCameraToZAxis() = 0;
    virtual void initCameraCenter() = 0;
    virtual void fixCameraCenterToItemsBarycenter() = 0;
    virtual void fixCameraCenterToSelectedItemsBarycenter() = 0;
    virtual void fitCameraToVisibleItems() = 0;
    virtual void fitToSpecifiedBox(const Eigen::Vector3d &bot, const Eigen::Vector3d &top) = 0;


signals:
    void coordinatesChanged();
};


#endif //GRAPHICSVIEWINTERFACE_H
