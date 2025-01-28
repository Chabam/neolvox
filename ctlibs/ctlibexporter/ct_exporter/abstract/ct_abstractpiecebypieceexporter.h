#ifndef CT_ABSTRACTPIECEBYPIECEEXPORTER_H
#define CT_ABSTRACTPIECEBYPIECEEXPORTER_H

#include "ctlibexporter_global.h"

#include "ct_point.h"
#include "ct_mesh/ct_edge.h"
#include "ct_mesh/ct_face.h"
#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"

#include <QObject>
#include <functional>

class CTLIBEXPORTER_EXPORT CT_AbstractPieceByPieceExporter : public QObject
{
    Q_OBJECT

public:
    using PointFilter = std::function<bool (const size_t& /*globalIndex*/, const CT_Point& /*point*/)>;
    using EdgeFilter = std::function<bool (const size_t& /*globalIndex*/, const CT_Edge& /*edge*/)>;
    using FaceFilter = std::function<bool (const size_t& /*globalIndex*/, const CT_Face& /*face*/)>;

    CT_AbstractPieceByPieceExporter();
    virtual ~CT_AbstractPieceByPieceExporter() {}

    /**
     * @brief Set a point filter that will be called each time a point will be exported. It must returns
     *        false if the point must not be exported, true otherwise. If you set a nullptr filter all points
     *        will be exported.
     */
    void setPointFilter(PointFilter filter);

    /**
     * @brief Set an edge filter that will be called each time a edge will be exported. It must returns
     *        false if the edge must not be exported, true otherwise. If you set a nullptr filter all edges
     *        will be exported.
     */
    void setEdgeFilter(EdgeFilter filter);

    /**
     * @brief Set a point filter that will be called each time a point will be exported. It must returns
     *        false if the point must not be exported, true otherwise. If you set a nullptr filter all points
     *        will be exported.
     */
    void setFaceFilter(FaceFilter filter);

    /**
     * @brief Returns the current error message
     */
    QString errorMessage() const;

protected:
    friend class CT_AbstractExporter;

    QString     mErrorMessage;
    PointFilter mPointFilter;
    EdgeFilter  mEdgeFilter;
    FaceFilter  mFaceFilter;
    bool        mFileCreated;
    int         mProgress;
    bool        mMustCancel;

    /**
     * @brief Returns true if file has been already created
     */
    bool isFileCreated() const;

    /**
     * @brief Call it to create the file. Returns false if it was an error.
     */
    bool createFile();

    /**
     * @brief Call it open the file if it is already created. Returns false if it was an error.
     */
    bool openFile();

    /**
     * @brief Close the file
     */
    void closeFile();

    /**
     * @brief Finalize the piece by piece export
     */
    bool finalizeFile();

    /**
     * @brief Call it to change the error message
     */
    void setErrorMessage(const QString &err);

    /**
     * @brief Call it to change the current progress and emit a signal
     */
    void setProgress(int progress);

    /**
     * @brief Create the file (erase it if already exist)
     */
    virtual bool internalCreateFile() = 0;

    /**
     * @brief Open the file in append mode
     */
    virtual bool internalOpenFileInAppendMode() = 0;

    /**
     * @brief Close the file
     */
    virtual void internalCloseFile() = 0;

    /**
     * @brief Finalize the piece by piece export
     */
    virtual bool internalFinalizeFile() = 0;

public slots:
    /**
     * @brief Call to cancel the process
     */
    void cancel();

signals:
    /**
     * @brief Emit when the progress changed.
     */
    void progressChanged(int progress);
};

#endif // CT_ABSTRACTPIECEBYPIECEEXPORTER_H
