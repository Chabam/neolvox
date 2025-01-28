#if (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__) // GNU Compiler (gcc,g++) for Linux, Unix, and MinGW (Windows)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include "ct_normalcloudstdvector.h"
#if (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

CT_NormalCloudStdVector::CT_NormalCloudStdVector(size_t size) : CT_StandardCloudStdVectorT<CT_Normal>(size), CT_AbstractNormalCloud()
{
}

size_t CT_NormalCloudStdVector::size() const
{
    return CT_StandardCloudStdVectorT<CT_Normal>::size();
}

size_t CT_NormalCloudStdVector::memoryUsed() const
{
    return CT_StandardCloudStdVectorT<CT_Normal>::memoryUsed();
}

void CT_NormalCloudStdVector::addNormal(const CT_Normal &normal)
{
    addT(normal);
}

CT_Normal& CT_NormalCloudStdVector::addNormal()
{
    return addT();
}

CT_Normal& CT_NormalCloudStdVector::normalAt(const size_t &index)
{
    return pTAt(index);
}

const CT_Normal& CT_NormalCloudStdVector::constNormalAt(const size_t &index) const
{
    return pTAt(index);
}

CT_Normal& CT_NormalCloudStdVector::operator[](const size_t &index)
{
    return pTAt(index);
}

const CT_Normal& CT_NormalCloudStdVector::operator[](const size_t &index) const
{
    return pTAt(index);
}

void CT_NormalCloudStdVector::replaceNormal(const size_t &index, CT_Normal &normal)
{
    replaceT(index, normal);
}

CT_AbstractCloud* CT_NormalCloudStdVector::copy() const
{
    size_t s = size();

    CT_NormalCloudStdVector *cloud = new CT_NormalCloudStdVector(s);

    for(size_t i=0; i<s; ++i)
        (*cloud)[i] = pTAt(i);

    return cloud;
}

void CT_NormalCloudStdVector::erase(const size_t &beginIndex, const size_t &sizes)
{
    CT_StandardCloudStdVectorT<CT_Normal>::erase(beginIndex, sizes);
}

void CT_NormalCloudStdVector::resize(const size_t &newSize)
{
    CT_StandardCloudStdVectorT<CT_Normal>::resize(newSize);
}

void CT_NormalCloudStdVector::reserve(const size_t& newSize)
{
    CT_StandardCloudStdVectorT<CT_Normal>::reserve(newSize);
}
