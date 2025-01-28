#ifndef CT_ABSTRACTREADER_H
#define CT_ABSTRACTREADER_H

#include "ctlibreader_global.h"
#include "ct_view/tools/fileformat.h"
#include "ct_reader/tools/ct_readeroutmodelstructuremanager.h"
#include "ct_itemdrawable/ct_standarditemgroup.h"

#include "ct_handle/ct_handleoutstdgroup.h"
#include "ct_handle/ct_handleoutsingularitem.h"
#include "ct_handle/ct_handleoutstditemattribute.h"
#include "ct_handle/ct_handleoutstditemattributewrapper.h"

#include "ct_itemdrawable/ct_fileheader.h"

#include <QObject>
#include <QFile>


#define READER_ALL_COPY_IMP(argClass) CT_AbstractReader* createInstance() const override { return new argClass(); } \
                                      CT_AbstractReader* copyFull() const override { return new argClass(*this); }

//#define READER_COPY_IMP(argClass) CT_AbstractReader* createInstance() const override { return new argClass(); }
//#define READER_COPY_FULL_IMP(argClass) CT_AbstractReader* copyFull() const override { return new argClass(*this); }

class CTLIBREADER_EXPORT CT_AbstractReader : public QObject
{
    Q_OBJECT

public:
    CT_AbstractReader(int subMenuLevel = 0);
    CT_AbstractReader(const CT_AbstractReader& other);
    ~CT_AbstractReader();

    /**
     * @brief Returns a displayable name of the reader. By default return the result of method "uniqueName()"
     */
    virtual QString displayableName() const;

    /**
     * @brief Returns the unique name (you can use it to compare readers because it was unique). By default
     *        return the result of "metaObject()->className()"
     */
    virtual QString uniqueName() const;

    /**
     * @brief Set the sub menu level where we can store this reader
     */
    void setSubMenuLevel(int level);

    /**
     * @brief Returns the sub menu level where we can store this reader
     */
    int subMenuLevel() const;

    /**
      * @brief Set the filepath of the file to read. The signal "filePathModified()" will be emitted if the filepath is accepted.
      * @warning You must NOT change parameter of your class in this method, just check if the file is valid ! Change your parameter in method
      *          "configure()" if you want.
      * @return false if the filepath can not be modified (see method "filePathCanBeModified()" for more information) or if the file can not
      *         be read or open or does not exist. True otherwise.
      */
    virtual bool setFilePath(const QString& filepath);

    /**
      * @brief Return the filepath of the file
      */
    QString filepath() const;

    // For buffer including multiples files
    struct FileBuffer {
        QString             filename;
        qint64              nPoints;
        std::vector<qint64> indexList;
    };

    /**
      * @brief Set the multiple files information for buffer to read.
      * @return false if one of the file can not be read or open or does not exist. True otherwise.
      */
    virtual bool setMultipleFilePath(const QList<FileBuffer>& fileBufferList);

    /**
      * @brief Return the multiple files information for buffer
      */
    const QList<FileBuffer>& multipleFilepath() const;

    /**
     * @brief Enable/Disable the modification of the filepath
     */
    void setFilePathCanBeModified(bool enable);

    /**
     * @brief Return true if the filepath can be modified
     */
    bool filePathCanBeModified() const;

    // By default CT_AbstractReader don't have a Bounding Box : redefine in children class of geographical files
    //virtual bool hasBoundingBox() {return false;}

    /**
     * @brief Configure the reader.
     *        Call it after the method "setFilePath" and before the method "protectedCreateOutItemDrawableModelList"
     * @return false if the user as canceled the configuration
     */
    virtual bool configure() { return true; }

    /**
     * @brief Redefine this method if you have a special header. By default return a new CT_FileHeader empty.
     */
    virtual CT_FileHeader* createHeaderPrototype() const;

    /**
     * @brief Call this method to save settings of the reader like the filepath.
     * @warning If you redefine this method don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual void saveSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief Call this method to restore settings of the reader like the filepath.
     * @warning If you redefine this method don't forget to call the method of the SuperClass if you redefine it !
     */
    virtual bool restoreSettings(SettingsReaderInterface& reader);

    /**
     * @brief Call it to declare all output models.
     */
    void declareOutputModels(CT_ReaderOutModelStructureManager& manager);

    /**
     * @brief Call it to declare all output models in a group of your result.
     * @param manager : The manager of output models
     * @param hParentGroup : The parent group where to add output models. It can be an input or an output handle
     *                       but must be of type CT_HandleXXXStdGroup
     */
    template<typename HandleGroup>
    void declareOutputModelsInGroup(CT_OutModelStructureManager& manager, HandleGroup& hParentGroup) {
        CT_ReaderOutModelStructureManager rManager = CT_ReaderOutModelStructureManager::createFromHandle(manager, hParentGroup);
        this->declareOutputModels(rManager);
    }

    /**
     * @brief Call it to declare all output models in a group of your result.
     * @param manager : The manager of output models
     * @param hParentGroup : The parent group where to add output models. It can be an input or an output handle
     *                       but must be of type CT_HandleXXXStdGroup
     */
    template<typename HandleGroup>
    void declareOutputModelsInGroupWithHeader(CT_OutModelStructureManager& manager, HandleGroup& hParentGroup, const QString& displayableName = tr("Entête de fichier"))
    {
        CT_ReaderOutModelStructureManager rManager = CT_ReaderOutModelStructureManager::createFromHandle(manager, hParentGroup);
        this->declareOutputModels(rManager);

        CT_FileHeader* header = createHeaderPrototype();

        if(header != nullptr)
            rManager.addItem(hParentGroup, m_hOutFileHeader, displayableName, "", "", header);
    }

    /**
      * @brief Return all readable formats
      */
    const QList<FileFormat>& readableFormats() const;

    /**
     * @brief Returns true if the extension is present in readable formats
     * @param extension : must be the extension of the file WITHOUT the comma "."
     */
    bool isExtensionPresentInReadableFormat(const QString& extension) const;

    /**
     * @brief Return true if it was a read error
     */
    bool isReadError() const;

    /**
     * @brief Return true if the read is stopped
     */
    bool isStopped() const;

    /**
     * @brief Returns a tooltip to show in GUI. None by default.
     */
    QString toolTip() const;

    /**
     * @brief Return the error message. You can set them by a call to the method setErrorMessage()
     */
    QString errorMessage() const;

    /**
     * @brief Clear the error message
     */
    void clearErrorMessage();

    /**
     * @brief Return a new instance of the reader
     */
    virtual CT_AbstractReader* createInstance() const = 0;

    /**
     * @brief Returns a copy with all parameters set
     */
    virtual CT_AbstractReader* copyFull() const = 0;

public slots:

    /**
     * @brief Read the header of the file and return it
     * @return nullptr if it was no header or if it can not be readed. Check isReadError() to know if it was an error and get it with method errorMessage()
     */
    CT_FileHeader* readHeader();

    /**
     * @brief Read the file and the header
     * @return Return false if it was a problem when read the file, check the error with method isReadError() and get it with method errorMessage()
     */
    bool readFile(CT_StandardItemGroup* group);

    /**
     * @brief Cancel the read process
     */
    void cancel();

public:
    CT_HandleOutSingularItem<CT_FileHeader> m_hOutFileHeader;

protected:

    class AbstractHandleManager
    {
    public:
        virtual ~AbstractHandleManager() {}

        virtual void* handle() const = 0;
    };

    template<typename HandleType>
    class HandleManager : public AbstractHandleManager
    {
    public:
        HandleManager(HandleType* handle) : mHandle(handle) {}
        ~HandleManager() final { delete mHandle; }

        void* handle() const final { return mHandle; }

        HandleType* mHandle;
    };

    template<typename HandleType>
    void registerHandlePtr(const QString& uniqueKey, HandleType* handle) {
        m_allHandles.insert(uniqueKey, new HandleManager<HandleType>(handle));
    }

    template<typename HandleType>
    HandleType* registeredHandlePtr(const QString& uniqueKey) const {
        return static_cast<HandleType*>(m_allHandles.value(uniqueKey)->handle());
    }

    void clearHandles();

    /**
     * @brief Add a new readable format
     */
    void addNewReadableFormat(const FileFormat &format);

    /**
     * @brief Set the tooltip to show in gui
     */
    void setToolTip(const QString &t);

    /**
     * @brief Must returns true if the file is correct (verify the header or other things). Called when the filepath is set but
     *        before it will be recorded and after the file has been opened. By
     *        default do nothing an returns true
     */
    virtual bool preVerifyFile(const QString& filepath, QFile& fileOpenReadOnly) const;

    /**
     * @brief Must returns true if the file is correct (verify the header or other things). Called when the filepath is set but
     *        before it will be recorded and after the file has been opened and closed. By
     *        default do nothing an returns true
     */
    virtual bool postVerifyFile(const QString& filepath);

    /**
     * @brief Redefine it to declare all output models.
     */
    virtual void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) = 0;

    /**
      * \brief Set the progression when you read the file
      */
    void setProgress(int progress);

    /**
      * \brief Set a error message
      */
    void setErrorMessage(const QString &err);

    /**
     * @brief Inherit this method to read and get header of the filepath passed in parameter
     * @param filepath : the path to the file
     * @param error : output error
     * @return nullptr if it was an error
     */
    virtual CT_FileHeader* internalReadHeader(const QString& filepath, QString& error) const;

    /**
     * @brief Inherit this method to read the file. You must add item or group in the rootGroup passed
     *        in parameter.
     */
    virtual bool internalReadFile(CT_StandardItemGroup* group) = 0;

    /**
     * @brief Inherit this method to read multiple file at once.
     *        You must add item or group in the rootGroup passed in parameter.
     */
    virtual bool internalReadMultiFile(CT_StandardItemGroup* group);

private:
    QList<FileFormat>                                       m_formats;
    QString                                                 m_tooltip;
    QString                                                 m_errorMess;
    int                                                     m_progress;
    bool                                                    m_error;
    bool                                                    m_stop;
    QString                                                 m_filePath;
    bool                                                    m_filepathCanBeModified;
    int                                                     m_subMenuLevel;
    QHash<QString, AbstractHandleManager*>                  m_allHandles;
    QList<FileBuffer>                                       m_fileBufferList;

signals:
    /**
     * @brief Emitted when the reader start to read the file
     */
    void started();

    /**
     * @brief Emitted when the progress value has changed
     */
    void progressChanged(int progress);

    /**
     * @brief Emitted when the reader has finished to read the file
     */
    void finished();

    /**
     * @brief Emitted when the filepath has been modified
     */
    void filePathModified();
};

#endif // CT_ABSTRACTREADER_H
