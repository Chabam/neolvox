#include "ct_tools/attributes/ct_attributestocloudworkert.h"

#include "ct_attributes/abstract/ct_abstractattributesscalar.h"
#include "ct_attributes/ct_attributescolor.h"
#include "ct_attributes/ct_attributesnormal.h"

#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"

template<typename TypeAttribute>
void CT_AttributesToCloudWorkerT<TypeAttribute>::apply()
{
    m_cancel = false;
    setProgress(0);

    if(!m_attributes.isEmpty())
    {
        CT_AbstractColorCloud *cc = nullptr;
        CT_AbstractNormalCloud *nn = nullptr;

        if(m_colors.data() != nullptr)
            cc = m_colors->abstractColorCloud();

        if(m_normals.data() != nullptr)
            nn = m_normals->abstractNormalCloud();

        size_t totalSize = 0;

        QListIterator<const TypeAttribute*> it(m_attributes);

        while(it.hasNext())
            totalSize += it.next()->attributesSize();

        size_t currentSize = 0;

        it.toFront();
        while(it.hasNext()
              && !m_cancel)
        {
            const TypeAttribute *pa = it.next();
            const CT_AbstractCloudIndex *pci = pa->abstractCloudIndex();
            size_t s = pa->attributesSize();
            size_t indexP;

            if(s > 0)
            {
                bool used = false;

                if(cc != nullptr)
                {
                    const CT_AbstractAttributesScalar *pas = dynamic_cast<const CT_AbstractAttributesScalar*>(pa);

                    if(pas != nullptr)
                    {
                        used = true;

                        double min = pas->dMin();
                        double max = pas->dMax();
                        double range = max-min;

                        if(range == 0)
                            range = 1;

                        for(size_t i=0; i<s && !m_cancel; ++i)
                        {
                            pci->indexAt(i, indexP);
                            CT_Color &color = cc->colorAt(indexP);

                            color[0] = ((pas->dValueAt(i)-min)*255)/range;
                            color[1] = color[0];
                            color[2] = color[0];

                            ++currentSize;

                            setProgress(int((currentSize*100)/totalSize));
                        }
                    }
                    else
                    {
                        const CT_AttributesColor *pac = dynamic_cast<const CT_AttributesColor*>(pa);

                        if(pac != nullptr)
                        {
                            used = true;

                            for(size_t i=0; i<s && !m_cancel; ++i)
                            {
                                pci->indexAt(i, indexP);
                                CT_Color &color1 = cc->colorAt(indexP);
                                const CT_Color &color2 = pac->constColorAt(i);

                                color1 = color2;
                                ++currentSize;

                                setProgress(int((currentSize*100)/totalSize));
                            }
                        }
                    }
                }

                if(nn != nullptr)
                {
                    const CT_AttributesNormal *pan = dynamic_cast<const CT_AttributesNormal*>(pa);

                    if(pan != nullptr)
                    {
                        used = true;

                        for(size_t i=0; i<s && !m_cancel; ++i)
                        {
                            pci->indexAt(i, indexP);
                            CT_Normal &normal1 = nn->normalAt(indexP);
                            const CT_Normal &normal2 = pan->constNormalAt(i);

                            normal1 = normal2;

                            ++currentSize;

                            setProgress(int((currentSize*100)/totalSize));
                        }
                    }
                }

                if(!used)
                {
                    currentSize += s;
                    setProgress(int((currentSize*100)/totalSize));
                }
            }
        }
    }

    setProgress(100);
    emit finished();
}
