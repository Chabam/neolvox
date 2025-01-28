#ifndef INDOCUMENTVIEWINTERFACE_H
#define INDOCUMENTVIEWINTERFACE_H

class DocumentInterface;

/**
 * @brief Represent a view that a document can contains
 */
class InDocumentViewInterface
{
public:
    virtual ~InDocumentViewInterface() {}

    /**
     * @brief Returns the document that contains this view
     */
    virtual DocumentInterface* document() const = 0;
};

#endif // INDOCUMENTVIEWINTERFACE_H
