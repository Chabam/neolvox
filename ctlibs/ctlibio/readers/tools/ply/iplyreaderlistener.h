#ifndef IPLYREADERLISTENER_H
#define IPLYREADERLISTENER_H

/**
 * @brief Interface for class that want to stop a read or be informed of the progress of the reader
 */
class IPlyReaderListener {
public:
    virtual ~IPlyReaderListener() {}

    /**
     * @brief This method must returns true if the read must be stopped
     */
    virtual bool plyReadMustStop() const = 0;

    /**
     * @brief This method is called when the progress changed
     */
    virtual void plyProgressChanged(int progress) = 0;
};

#endif // IPLYREADERLISTENER_H
