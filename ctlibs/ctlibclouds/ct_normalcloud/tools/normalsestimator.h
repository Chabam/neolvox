#ifndef NORMALSESTIMATOR_H
#define NORMALSESTIMATOR_H

#include "ctlibclouds_global.h"

#ifdef USE_PCL
#include "iestimatorobserverandcontroller.h"

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

#include <pcl/common/common.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/boost.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/property_map/property_map.hpp>

typedef boost::property<boost::edge_weight_t, float> Weight;
typedef boost::property<boost::vertex_name_t, int> Index;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Index, Weight> GraphPoisson;

typedef boost::graph_traits<GraphPoisson>::vertex_descriptor Vertex;
typedef boost::graph_traits<GraphPoisson>::edge_descriptor EdgePoisson;
typedef boost::property_map< GraphPoisson, boost::vertex_index_t>::type VertexIndexMap;
typedef boost::property_map< GraphPoisson, boost::edge_weight_t>::type WeightMap;

class CTLIBCLOUDS_EXPORT normalsEstimator
{
public:
    normalsEstimator(pcl::PointCloud<pcl::PointXYZ> pPointCloud, const IEstimatorObserverAndController *obs = nullptr);
    pcl::PointCloud<pcl::PointNormal> getNormals(int nbNeighborsNormals);

private:
    pcl::PointCloud<pcl::PointXYZ>  pointCloud;
    IEstimatorObserverAndController *m_observer;

    void setProgress(int p);
    bool mustStop() const;
};

#endif

#endif // NORMALSESTIMATOR_H
