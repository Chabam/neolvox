#include "ct_parseris.h"

#include <QMap>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define QT_SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#define QT_SKIP_EMPTY_PARTS QString::SkipEmptyParts
#endif


QString CT_ParseRIS::parseRIS(QString ris)
{    
    QMultiMap<QString, QString> fields;
    QStringList lines = ris.split("\n", QT_SKIP_EMPTY_PARTS);

    for (int i = 0 ; i < lines.size() ; i++)
    {
        QString line = lines.at(i);

        QString fieldValue = line.mid(0,2);
        int cpt = 2;
        while (line.mid(cpt,1) == " " || line.mid(cpt,1) == "-")
        {
            ++cpt;
        }

        QString value = line.mid(cpt);

        QString code = fieldValue.toUpper();
        if (code == "AU") {code = "A1";}
        if (code.size() == 2)
        {
            fields.insert(code, value);
        }
    }

    QString str;

    if (fields.contains("A1") || fields.contains("PY"))
    {
        str.append("<b>");
    }


    QList<QString> authors = fields.values("A1");

    for (int i = authors.size() - 1 ; i >= 0 ; i--)
    {

        QString author = authors.at(i);
        author.replace(", ", " ");
        author.replace(",", " ");
        str.append(author);
        if (i > 0)
        {
            str.append(", ");
        }
    }

    if (str.size() > 0)
    {
        str.append(". ");
    }

    if (fields.contains("PY"))
    {
        QList<QString> vals = fields.values("PY");;
        str.append(vals.first());
        str.append(". ");
    }else if (fields.contains("Y1"))
    {
        QList<QString> vals = fields.values("Y1");;
        str.append(vals.first());
        str.append(". ");
    }

    if (str.size() > 0)
    {
        str.append("</b>");
    }

    if (fields.contains("TI"))
    {
        str.append("<em>");
        QList<QString> vals = fields.values("TI");;
        str.append(vals.first());
        str.append("</em>. ");
    } else if (fields.contains("T1"))
    {
        str.append("<em>");
        QList<QString> vals = fields.values("T1");;
        str.append(vals.first());
        str.append("</em>. ");
    }

    if (fields.contains("PB"))
    {
        QList<QString> vals = fields.values("PB");;
        str.append(vals.first());
        str.append(". ");
    }

    if (fields.contains("JO"))
    {
        QList<QString> vals = fields.values("JO");;
        str.append(vals.first());
        str.append(". ");
    }

    if (fields.contains("UR"))
    {
        str.append("<a href=\"");
        QList<QString> vals = fields.values("UR");;
        str.append(vals.first());
        str.append("\">");
        str.append(vals.first());
        str.append("</a>. ");
    }

    return str;
}

