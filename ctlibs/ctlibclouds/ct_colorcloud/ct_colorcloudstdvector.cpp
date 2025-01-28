#include "ct_colorcloudstdvector.h"

CT_ColorCloudStdVector::CT_ColorCloudStdVector() : CT_StandardCloudStdVectorT<CT_Color>(), CT_AbstractColorCloud()
{
}

CT_ColorCloudStdVector::CT_ColorCloudStdVector(size_t size) : CT_StandardCloudStdVectorT<CT_Color>(size), CT_AbstractColorCloud()
{
}

size_t CT_ColorCloudStdVector::size() const
{
    return CT_StandardCloudStdVectorT<CT_Color>::size();
}

size_t CT_ColorCloudStdVector::memoryUsed() const
{
    return CT_StandardCloudStdVectorT<CT_Color>::memoryUsed();
}

void CT_ColorCloudStdVector::addColor(const CT_Color &color)
{
    addT(color);
}

CT_Color& CT_ColorCloudStdVector::addColor()
{
    return addT();
}

CT_Color& CT_ColorCloudStdVector::colorAt(const size_t &index)
{
    return pTAt(index);
}

const CT_Color& CT_ColorCloudStdVector::constColorAt(const size_t &index) const
{
    return pTAt(index);
}

CT_Color& CT_ColorCloudStdVector::operator[](const size_t &index)
{
    return pTAt(index);
}

const CT_Color& CT_ColorCloudStdVector::operator[](const size_t &index) const
{
    return pTAt(index);
}

CT_AbstractCloud* CT_ColorCloudStdVector::copy() const
{
    size_t s = size();

    CT_ColorCloudStdVector *cloud = new CT_ColorCloudStdVector(s);

    for(size_t i=0; i<s; ++i)
        (*cloud)[i] = pTAt(i);

    return cloud;
}

void CT_ColorCloudStdVector::erase(const size_t &beginIndex, const size_t &sizes)
{
    CT_StandardCloudStdVectorT<CT_Color>::erase(beginIndex, sizes);
}

void CT_ColorCloudStdVector::resize(const size_t &newSize)
{
    CT_StandardCloudStdVectorT<CT_Color>::resize(newSize);
}

void CT_ColorCloudStdVector::reserve(const size_t& newSize)
{
    CT_StandardCloudStdVectorT<CT_Color>::reserve(newSize);
}
