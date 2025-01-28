#ifndef CT_DEFINES_H
#define CT_DEFINES_H

#include <QSharedPointer>

// class definition

class CT_Point;
class CT_PointData;
class CT_Face;
class CT_Edge;

class CT_InternalPointCloud;

class CT_PointIterator;
class CT_MutablePointIterator;
class CT_MutableIndexIterator;

class CT_AbstractCoordinateSystem;

template<typename T> class CT_ModifiableCloudIndexIteratorT;
template<typename T> class CT_CloudIndexIteratorT;

template<typename T> class CT_AbstractCloudT;
template<typename T> class CT_StandardCloudStdVectorT;

template<typename T> class CT_AbstractCloudIndexT;
template<typename T> class CT_AbstractModifiableCloudIndexT;
template<typename T> class CT_CloudIndexStdVectorT;
template<typename T> class CT_CloudIndexLessMemoryT;

template<typename T, class CloudIndexLessMemory> class CT_CloudIndexRegistrationManagerT;

class CT_AbstractCloudIndexRegistered;
class CT_AbstractModifiableCloudIndexRegistered;
template<typename T> class CT_AbstractCloudIndexRegisteredT;
template<typename T> class CT_AbstractNotModifiableCloudIndexRegisteredT;
template<typename T> class CT_AbstractModifiableCloudIndexRegisteredT;

class CT_CloudSyncToGlobalCloudManager;

template<typename T> class CT_AbstractGlobalCloudManagerT;
template<typename T, typename CLOUD> class CT_GlobalCloudManagerT;

class CT_StandardColorCloudRegistered;
class CT_StandardNormalCloudRegistered;

class CT_GlobalPointCloudManager;
class CT_GlobalFaceCloudManager;
class CT_GlobalEdgeCloudManager;

template<typename MeshType> class CT_MeshAllocatorT;
class CT_Mesh;

// typedef iterator

/**
 * @brief Iterator for points
 */
//typedef CT_MutablePointIterator                                                   CT_MutablePointIterator;
typedef CT_MutableIndexIterator                                                     CT_MutablePointIndexIterator;

/**
 * @brief Iterator for faces
 */
typedef CT_CloudIndexIteratorT<CT_Face>                                             CT_MutableFaceIterator;
typedef CT_MutableIndexIterator                                                     CT_MutableFaceIndexIterator;

/**
 * @brief Iterator for edges
 */
typedef CT_CloudIndexIteratorT<CT_Edge>                                             CT_MutableEdgeIterator;
typedef CT_MutableIndexIterator                                                     CT_MutableEdgeIndexIterator;


/**
 * @brief Iterator for points
 */
//typedef CT_PointIterator                                                          CT_PointIterator;

/**
 * @brief Iterator for faces
 */
typedef CT_CloudIndexIteratorT<CT_Face>                                             CT_FaceIterator;

/**
 * @brief Iterator for edges
 */
typedef CT_CloudIndexIteratorT<CT_Edge>                                             CT_EdgeIterator;


// typedef cloud

/**
 * @brief Abstract cloud of points
 */
typedef CT_AbstractCloudT<CT_PointData>                                             CT_AbstractPointCloud;

/**
 * @brief Abstract cloud of faces
 */
typedef CT_AbstractCloudT<CT_Face>                                                  CT_AbstractFaceCloud;

/**
 * @brief Abstract cloud of edges
 */
typedef CT_AbstractCloudT<CT_Edge>                                                  CT_AbstractEdgeCloud;

/**
 * @brief Cloud of points of type std::vector
 */
//typedef CT_InternalPointCloud                                                       CT_PointCloudStdVector;

/**
 * @brief Cloud of faces of type std::vector
 */
typedef CT_StandardCloudStdVectorT<CT_Face>                                         CT_FaceCloudStdVector;

/**
 * @brief Cloud of edges of type std::vector
 */
typedef CT_StandardCloudStdVectorT<CT_Edge>                                         CT_EdgeCloudStdVector;

// typedef cloud index

/**
 * @brief Abstract cloud index of points
 */
typedef CT_AbstractCloudIndexT<CT_PointData>                                        CT_AbstractPointCloudIndex;

/**
 * @brief Abstract cloud index of faces
 */
typedef CT_AbstractCloudIndexT<CT_Face>                                             CT_AbstractFaceCloudIndex;

/**
 * @brief Abstract cloud index of edges
 */
typedef CT_AbstractCloudIndexT<CT_Edge>                                             CT_AbstractEdgeCloudIndex;

/**
 * @brief Abstract cloud index of points
 */
typedef CT_AbstractModifiableCloudIndexT<CT_PointData>                              CT_AbstractModifiablePointCloudIndex;

/**
 * @brief Abstract cloud index of faces
 */
typedef CT_AbstractModifiableCloudIndexT<CT_Face>                                   CT_AbstractModifiableFaceCloudIndex;

/**
 * @brief Abstract cloud index of edges
 */
typedef CT_AbstractModifiableCloudIndexT<CT_Edge>                                   CT_AbstractModifiableEdgeCloudIndex;

/**
 * @brief Cloud index of points of type std::vector
 */
typedef CT_CloudIndexStdVectorT<CT_PointData>                                       CT_PointCloudIndexVector;

/**
 * @brief Cloud index of faces of type std::vector
 */
typedef CT_CloudIndexStdVectorT<CT_Face>                                            CT_FaceCloudIndexVector;

/**
 * @brief Cloud index of edges of type std::vector
 */
typedef CT_CloudIndexStdVectorT<CT_Edge>                                            CT_EdgeCloudIndexVector;

/**
 * @brief Cloud index of points with less memory management
 */
typedef CT_CloudIndexLessMemoryT<CT_PointData>                                      CT_PointCloudIndexLessMemory;

/**
 * @brief Cloud index of faces with less memory management
 */
typedef CT_CloudIndexLessMemoryT<CT_Face>                                           CT_FaceCloudIndexLessMemory;

/**
 * @brief Cloud index of edges with less memory management
 */
typedef CT_CloudIndexLessMemoryT<CT_Edge>                                           CT_EdgeCloudIndexLessMemory;


// typedef cloud index registered

/**
 * @brief Cloud index of ? registered
 */
typedef QSharedPointer< CT_AbstractCloudIndexRegistered >                           CT_CIR;

/**
 * @brief Cloud index of ? registered that indexes is modifiable
 */
typedef QSharedPointer<CT_AbstractModifiableCloudIndexRegistered>                   CT_MCIR;

/**
 * @brief Point cloud index registered
 */
typedef QSharedPointer< CT_AbstractCloudIndexRegisteredT<CT_PointData> >            CT_PCIR;

/**
 * @brief Face cloud index registered
 */
typedef QSharedPointer< CT_AbstractCloudIndexRegisteredT<CT_Face> >                 CT_FCIR;

/**
 * @brief Edge cloud index registered
 */
typedef QSharedPointer< CT_AbstractCloudIndexRegisteredT<CT_Edge> >                 CT_ECIR;

/**
 * @brief Point cloud index registered that indexes is not modifiable
 */
typedef QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<CT_PointData> >   CT_NMPCIR;

/**
 * @brief Face cloud index registered that indexes is not modifiable
 */
typedef QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<CT_Face> >    CT_NMFCIR;

/**
 * @brief Edge cloud index registered that indexes is not modifiable
 */
typedef QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<CT_Edge> >    CT_NMECIR;

/**
 * @brief Point cloud index registered that indexes is modifiable
 */
typedef QSharedPointer< CT_AbstractModifiableCloudIndexRegisteredT<CT_PointData> >  CT_MPCIR;

/**
 * @brief Face cloud index registered that indexes is modifiable
 */
typedef QSharedPointer< CT_AbstractModifiableCloudIndexRegisteredT<CT_Face> >       CT_MFCIR;

/**
 * @brief Edge cloud index registered that indexes is modifiable
 */
typedef QSharedPointer< CT_AbstractModifiableCloudIndexRegisteredT<CT_Edge> >       CT_MECIR;

/**
 * @brief Color cloud registered
 */
typedef QSharedPointer< CT_StandardColorCloudRegistered >                           CT_CCR;

/**
 * @brief Normal cloud registered
 */
typedef QSharedPointer< CT_StandardNormalCloudRegistered >                          CT_NCR;

// typedef cloud index registration manager

typedef CT_CloudIndexRegistrationManagerT<CT_PointData, CT_PointCloudIndexLessMemory >  CT_PointCloudIndexRegistrationManager;
typedef CT_CloudIndexRegistrationManagerT<CT_Face, CT_FaceCloudIndexLessMemory >        CT_FaceCloudIndexRegistrationManager;
typedef CT_CloudIndexRegistrationManagerT<CT_Edge, CT_EdgeCloudIndexLessMemory >        CT_EdgeCloudIndexRegistrationManager;

// typedef cloud sync manager

typedef CT_CloudSyncToGlobalCloudManager                                                CT_SyncPointCloudManager;
typedef CT_CloudSyncToGlobalCloudManager                                                CT_SyncFaceCloudManager;
typedef CT_CloudSyncToGlobalCloudManager                                                CT_SyncEdgeCloudManager;

// typedef global cloud manager

typedef CT_AbstractGlobalCloudManagerT<CT_PointData>                                    CT_AbstractGlobalPointCloudManager;
typedef CT_AbstractGlobalCloudManagerT<CT_Face>                                         CT_AbstractGlobalFaceCloudManager;
typedef CT_AbstractGlobalCloudManagerT<CT_Edge>                                         CT_AbstractGlobalEdgeCloudManager;

// typedef coordinate system

typedef QSharedPointer<CT_AbstractCoordinateSystem>                                     CT_CSR;

// typedef mesh

typedef CT_MeshAllocatorT<CT_Mesh>                                                      CT_MeshAllocator;
#endif // CT_DEFINES_H
