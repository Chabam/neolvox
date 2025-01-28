#ifndef CT_STANDARDREADERSEPARATOR_H
#define CT_STANDARDREADERSEPARATOR_H

#include "ctlibplugin_global.h"

#include "ct_reader/abstract/ct_abstractreader.h"

class CTLIBPLUGIN_EXPORT CT_StandardReaderSeparator
{
public:
    CT_StandardReaderSeparator(const QString& title = "");
    virtual ~CT_StandardReaderSeparator();

    bool addReader(CT_AbstractReader* reader);

    QString title() const;

    QList<CT_AbstractReader*> readers() const;

private:
    QList<CT_AbstractReader*>   m_readers;
    QString                     m_title;

};

#endif // CT_STANDARDREADERSEPARATOR_H
