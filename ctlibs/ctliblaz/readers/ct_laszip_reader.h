#ifndef CT_LASZIP_READER_H
#define CT_LASZIP_READER_H

#include "laszip/laszip_api.h"

#include <QObject>

class CT_LASZIP_Reader : public QObject
{
    Q_OBJECT

public:

    CT_LASZIP_Reader(QString filepath);

    virtual ~CT_LASZIP_Reader() {}

    bool openFile();
    bool closeFile();

    bool initHeaderPointer(laszip_header_struct** header);

    bool initPointPointer();
    bool readPoint();
    bool seek(laszip_I64 seekPos);

    laszip_point* point() {return _point;}

private:
    QString             _filePath;
    laszip_POINTER      _laszip_reader;
    laszip_point*       _point;

    bool                _createOK;
    bool                _openOK;
    bool                _pointPointerOK;


};

#endif // CT_LASZIP_READER_H
