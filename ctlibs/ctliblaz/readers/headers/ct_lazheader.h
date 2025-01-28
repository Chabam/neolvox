#ifndef CT_LAZHEADER_H
#define CT_LAZHEADER_H

#include "ctliblaz/ctliblaz_global.h"
#include "ctliblas/readers/headers/ct_lasheader.h"

#include "laszip/laszip_api.h"

class CTLIBLAZ_EXPORT CT_LAZHeader : public CT_LASHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_LAZHeader, CT_LASHeader, LAZ Header)

    using SuperClass = CT_LASHeader;

public:
    CT_LAZHeader();
    CT_LAZHeader(const CT_LAZHeader& other) = default;

    /**
     * @brief Read the zipped header
     */
    bool readzip(const QString &filepath, QString &error);

    /**
     * @brief Write the zipped header
     */
    bool writezip(laszip_header &header) const;
};

#endif // CT_LAZHEADER_H
