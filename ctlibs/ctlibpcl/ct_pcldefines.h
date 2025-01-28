#ifndef CT_PCLDEFINES_H
#define CT_PCLDEFINES_H

#ifndef Q_MOC_RUN
#if defined(_WIN32) && defined(_MSC_VER) // Microsoft Visual Studio Compiler
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__) // GNU Compiler (gcc,g++) for Linux, Unix, and MinGW (Windows)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__APPLE__) // Clang Compiler (Apple)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    #define PCL_NO_PRECOMPILE
    #include <pcl/point_types.h>
    #include <pcl/point_cloud.h>
    #include <pcl/octree/octree_search.h>
    #include <pcl/octree/impl/octree_search.hpp>
    #include <pcl/filters/voxel_grid.h>
    #include <pcl/filters/impl/voxel_grid.hpp>
    #include <pcl/filters/statistical_outlier_removal.h>
    #include <pcl/filters/impl/statistical_outlier_removal.hpp>
#if defined(_WIN32) && defined(_MSC_VER)
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__APPLE__)
#pragma GCC diagnostic pop
#endif
#endif

typedef pcl::PointXYZ                                                               CT_PCLPoint;
typedef pcl::PointCloud<CT_PCLPoint>                                                CT_PCLCloud;
typedef std::vector<int>                                                            CT_PCLCloudIndex;

#endif // CT_PCLDEFINES_H
