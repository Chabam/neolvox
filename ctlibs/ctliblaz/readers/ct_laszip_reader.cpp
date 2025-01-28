#include "ct_laszip_reader.h"
#include "ct_log/ct_logmanager.h"


CT_LASZIP_Reader::CT_LASZIP_Reader(QString filepath)
{
    _filePath = filepath;
    _createOK = false;
    _openOK = false;
    _pointPointerOK = false;
}

bool CT_LASZIP_Reader::openFile()
{
    // create the reader
    if (laszip_create(&_laszip_reader))
    {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : impossible d'accéder au fichier %1").arg(_filePath));
        return false;
    }
    _createOK = true;


    // open the reader
    laszip_BOOL is_compressed = 0;
    if (laszip_open_reader(_laszip_reader, _filePath.toStdString().c_str(), &is_compressed))
    {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : impossible d'ouvrir le fichier %1").arg(_filePath));
        return false;
    }
    _openOK = true;

    return true;
}

bool CT_LASZIP_Reader:: initHeaderPointer(laszip_header_struct** header)
{
    if (!_openOK) {return false;}

    // get a pointer to the header of the reader that was just populated
    if (laszip_get_header_pointer(_laszip_reader, header))
    {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : header non valide pour le fichier %1").arg(_filePath));
        return false;
    }

    return true;
}

bool CT_LASZIP_Reader::initPointPointer()
{
    _pointPointerOK = true;

    if (laszip_get_point_pointer(_laszip_reader, &_point))
    {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : Impossible de lire les points dans le fichier  le fichier %1").arg(_filePath));
        _pointPointerOK = false;
    }

    return _pointPointerOK;
}

bool CT_LASZIP_Reader::readPoint()
{
    if (!_pointPointerOK) {return false;}

    if (laszip_read_point(_laszip_reader))
    {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : Impossible de lire le point (laszip_read_point)"));
        return false;
    }

    return true;
}


bool CT_LASZIP_Reader::seek(laszip_I64 seekPos)
{
    if (!_pointPointerOK) {return false;}

    laszip_seek_point(_laszip_reader, seekPos);

    return true;
}




bool CT_LASZIP_Reader::closeFile()
{
    bool ok = true;

    // close the reader
    if (_openOK)
    {
        if (laszip_close_reader(_laszip_reader))
        {
            PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : impossible de fermer le fichier %1").arg(_filePath));
            ok = false;
        }
    }

    // destroy the reader
    if (_createOK)
    {
        if (laszip_clean(_laszip_reader))
        {
            PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : impossible de clean le fichier %1").arg(_filePath));
            ok = false;
        }

        if (laszip_destroy(_laszip_reader))
        {
            PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_LASZIP_Reader : impossible de libérer l'accès au fichier %1").arg(_filePath));
            ok = false;
        }

    }

    return ok;
}

