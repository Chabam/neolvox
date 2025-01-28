#include "ct_abstractreader.h"

CT_AbstractReader::CT_AbstractReader(int subMenuLevel) :
    m_progress(0),
    m_error(false),
    m_stop(false),
    m_filepathCanBeModified(true),
    m_subMenuLevel(subMenuLevel)
{
}

CT_AbstractReader::CT_AbstractReader(const CT_AbstractReader& other) : QObject( nullptr )
{
    m_formats = other.m_formats;
    m_tooltip = other.m_tooltip;
    m_errorMess = other.m_errorMess;

    // copy models of items
    /*for(const CT_OutStdSingularItemModel* model : other.m_outItemsModel)
        m_outItemsModel.append(static_cast<CT_OutStdSingularItemModel*>(model->copy()));

    // copy models of groups
    for(const CT_OutStdGroupModel* model : other.m_outGroupsModel)
        m_outGroupsModel.append(static_cast<CT_OutStdGroupModel*>(model->copy()));*/

    // m_outItems not copied
    // m_outGroups not copied

    m_progress = other.m_progress;
    m_error = other.m_error;
    m_stop = other.m_stop;
    m_filePath = other.m_filePath;
    m_filepathCanBeModified = other.m_filepathCanBeModified;
    m_subMenuLevel = other.m_subMenuLevel;
}

CT_AbstractReader::~CT_AbstractReader()
{
    clearHandles();
}

QString CT_AbstractReader::displayableName() const
{
    return uniqueName();
}

QString CT_AbstractReader::uniqueName() const
{
    // must not be translatable
    return metaObject()->className();
}

void CT_AbstractReader::setSubMenuLevel(int level)
{
    m_subMenuLevel = level;
}

int CT_AbstractReader::subMenuLevel() const
{
    return m_subMenuLevel;
}

bool CT_AbstractReader::setFilePath(const QString& filepath)
{
    if(!filePathCanBeModified())
        return false;

    QStringList allSuffixes;
    for(const FileFormat& fileFormat : readableFormats()) {
        const QList<QString>& suffixes = fileFormat.suffixes();
        allSuffixes.append(suffixes);
    }

    if(!allSuffixes.contains("*") && !allSuffixes.contains(QFileInfo(filepath).suffix().toLower()))
        return false;

    // Verify that the file exist and can be opened
    if (QFile::exists(filepath))
    {
        QFile file(filepath);

        if(file.open(QIODevice::ReadOnly))
        {
            const bool ok = preVerifyFile(filepath, file);

            file.close();

            if(!ok)
                return false;

            if(!postVerifyFile(filepath))
                return false;

            m_filePath = filepath;

            emit filePathModified();

            return true;
        }
    }

    return false;
}

QString CT_AbstractReader::filepath() const
{
    return m_filePath;
}

bool CT_AbstractReader::setMultipleFilePath(const QList<FileBuffer>& fileBufferList)
{
    // Check that the list is not empty
    if(fileBufferList.length() == 0)
        return false;

    // List all possible suffix
    QStringList allSuffixes;
    for(const FileFormat& fileFormat : readableFormats()) {
        const QList<QString>& suffixes = fileFormat.suffixes();
        allSuffixes.append(suffixes);
    }

    // Now check every file path from the list
    for(auto &fileBuffer: fileBufferList)
    {
        if(!allSuffixes.contains("*") && !allSuffixes.contains(QFileInfo(fileBuffer.filename).suffix().toLower()))
            return false;

        // Verify that the file exist and can be opened
        if (QFile::exists(fileBuffer.filename))
        {
            QFile file(fileBuffer.filename);

            if(file.open(QIODevice::ReadOnly))
            {
                const bool ok = preVerifyFile(fileBuffer.filename, file);

                file.close();

                if(!ok)
                    return false;

                if(!postVerifyFile(fileBuffer.filename))
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }

    // If all path are ok, we can save the buffer list.
    m_fileBufferList = fileBufferList;

    // Associate the filepath to the first reference file (nPoints = 0) in the list
    for(auto &fileBuffer: fileBufferList)
    {
        if(fileBuffer.nPoints == 0)
        {
            m_filePath = fileBuffer.filename;
            break;
        }
    }

    // Check that the filepath is not empty
    if(m_filePath.isEmpty())
        m_filePath = m_fileBufferList[0].filename;

    return true;
}

const QList<CT_AbstractReader::FileBuffer>& CT_AbstractReader::multipleFilepath() const
{
    return m_fileBufferList;
}

void CT_AbstractReader::setFilePathCanBeModified(bool enable)
{
    m_filepathCanBeModified = enable;
}

bool CT_AbstractReader::filePathCanBeModified() const
{
    return m_filepathCanBeModified;
}

CT_FileHeader* CT_AbstractReader::createHeaderPrototype() const
{
    return new CT_FileHeader();
}

void CT_AbstractReader::saveSettings(SettingsWriterInterface &writer) const
{
    writer.addParameterPath(this, "FilePath", filepath());
}

bool CT_AbstractReader::restoreSettings(SettingsReaderInterface &reader)
{
    QString path;

    if(reader.parameterPath(this, "FilePath", path))
        return setFilePath(path);

    return true;
}

void CT_AbstractReader::declareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    clearHandles();

    internalDeclareOutputModels(manager);
}

const QList<FileFormat>& CT_AbstractReader::readableFormats() const
{
    return m_formats;
}

bool CT_AbstractReader::isExtensionPresentInReadableFormat(const QString &extension) const
{
    for(const FileFormat& ff : m_formats) {
        if(ff.suffixes().contains(extension))
            return true;
    }

    return false;
}

bool CT_AbstractReader::isReadError() const
{
    return m_error;
}

bool CT_AbstractReader::isStopped() const
{
    return m_stop;
}

QString CT_AbstractReader::toolTip() const
{
    return m_tooltip;
}

QString CT_AbstractReader::errorMessage() const
{
    return m_errorMess;
}

void CT_AbstractReader::clearErrorMessage()
{
    m_errorMess = "";
    m_error = false;
}

CT_FileHeader* CT_AbstractReader::readHeader()
{
    QString error;

    CT_FileHeader* header = internalReadHeader(filepath(), error);

    if((header == nullptr) && !error.isEmpty())
        setErrorMessage(error);

    return header;
}

bool CT_AbstractReader::readFile(CT_StandardItemGroup* group)
{
    Q_ASSERT(group != nullptr);
    if (group == nullptr) {qDebug() << "CT_AbstractReader::readFile" << ", " <<  "group == nullptr"; return false;}

    emit started();

    clearErrorMessage();
    setProgress(0);

    m_error = false;
    m_stop = false;

    if(m_fileBufferList.length() == 0)
        m_error = internalReadFile(group);
    else
        m_error = internalReadMultiFile(group);

    if(m_hOutFileHeader.isValid()) {
        CT_FileHeader* header = readHeader();

        header->setReader(this->copyFull(), true);

        Q_ASSERT(header != nullptr);
        if (header == nullptr) {qDebug() << "CT_AbstractReader::readFile" << ", " <<  "header == nullptr"; return false;}

        group->addSingularItem(m_hOutFileHeader, header);
    }

    emit finished();

    return m_error;
}

void CT_AbstractReader::cancel()
{
    m_stop = true;
}

void CT_AbstractReader::clearHandles()
{
    qDeleteAll(m_allHandles.begin(), m_allHandles.end());
    m_allHandles.clear();
}

void CT_AbstractReader::addNewReadableFormat(const FileFormat &format)
{
    m_formats.append(format);
}

void CT_AbstractReader::setToolTip(const QString &t)
{
    m_tooltip = t;
}

bool CT_AbstractReader::preVerifyFile(const QString&, QFile&) const
{
    return true;
}

bool CT_AbstractReader::postVerifyFile(const QString&)
{
    return true;
}

void CT_AbstractReader::setProgress(int progress)
{
    if(progress != m_progress)
    {
        m_progress = progress;

        emit progressChanged(m_progress);
    }
}

void CT_AbstractReader::setErrorMessage(const QString &err)
{
    m_errorMess = err;

    if(!m_errorMess.isEmpty())
        m_error = true;
}

CT_FileHeader* CT_AbstractReader::internalReadHeader(const QString& filepath, QString&) const
{
    CT_FileHeader* f = new CT_FileHeader();
    f->setFilePath(filepath);

    return f;
}

bool CT_AbstractReader::internalReadMultiFile(CT_StandardItemGroup* group)
{
    Q_UNUSED(group);
    return true;
}
