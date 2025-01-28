#ifndef CT_READER_TRJ_H
#define CT_READER_TRJ_H

#include "ctlibio/ctlibio_global.h"
#include "headers/ct_trjheader.h"
#include "ct_reader/abstract/ct_abstractreader.h"

#include "ct_itemdrawable/ct_scanpath.h"

/**
 * @brief Reader that can load a trj file (*.trj) that represent a trajectory
 */
class CTLIBIO_EXPORT CT_Reader_TRJ : public CT_AbstractReader
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_TRJ(int subMenuLevel = 0);
    CT_Reader_TRJ(const CT_Reader_TRJ& other);

    /**
     * @brief Returns a displayable name of the reader
     */
    QString displayableName() const override;

    /**
     * @brief Redefined to check the validity of the file. Returns false if the file is not compatible.
     */
    bool setFilePath(const QString& filepath) override;

    /**
     * @brief Redefined to create a CT_TRJHeader
     */
    CT_FileHeader* createHeaderPrototype() const override;

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    READER_ALL_COPY_IMP(CT_Reader_TRJ)

protected:
    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;

    CT_FileHeader* internalReadHeader(const QString &filepath, QString &error) const override;

    bool internalReadFile(CT_StandardItemGroup* group) override;

private:

    typedef struct
    {
        char            Recog[8] ;       // TSCANTRJ
        int             Version ;        // File version 20010715
        int             HdrSize ;        // sizeof(TrajHdr)
        int             PosCnt ;         // Number of position records
        int             PosSize ;        // Size of position records
        char            Desc[78] ;       // Description
        unsigned char   SysIdv ;         // System identifier (for lever arms)
        unsigned char   Quality ;        // Quality for whole trajectory (1-5)
        double          BegTime ;        // First time stamp
        double          EndTime ;        // Last time stamp
        int             OrigNbr ;        // Original number (before any splitting)
        int             Number ;         // Line number (in laser points)
        char            VrtVideo[400] ;  // Vertical facing video
        double          VrtBeg ;         // Start time of VrtVideo[]
        double          VrtEnd ;         // End time of VrtVideo[]
        char            FwdVideo[400] ;  // Forward facing video
        double          FwdBeg ;         // Start time of FwdVideo[]
        double          FwdEnd ;         // End time of FwdVideo[]
        char            WaveFile[400] ;  // Waveform data file
        char            Group[16] ;      // Group (session description)
    } TrajHdr ;

    typedef struct
    {
        double          Time ;           // Time stamp
        double          x ;              // Position x
        double          y ;              // Position y
        double          z ;              // Position z
        double          Head ;           // Heading (degrees)
        double          Roll ;           // Roll (degrees)
        double          Pitch ;          // Pitch (degrees)
        unsigned char   QtyXy ;          // Quality for xy, 0=not set
        unsigned char   QtyZ ;           // Quality for z, 0=not set
        unsigned char   QtyH ;           // Quality for headingy, 0=not set
        unsigned char   QtyRp ;          // Quality for roll/pitch, 0=not set
        short           Mark ;           // Run time flag
        short           Flag ;           // Run time flag
    } TrajPos ;


    CT_HandleOutSingularItem<CT_ScanPath>          m_hOutScanPath;

};

#endif // CT_Reader_TRJ_H
