/****************************************************************************

 Copyright (C) 2010-2012 Jules Morel, Pondichéry, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Jules Morel

 This file is part of Computree version 2.0.

 Computree is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Computree is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Computree.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#include "normalsestimator.h"

#ifdef USE_PCL

//DFS visitor, got help from http://stackoverflow.com/questions/14126/how-to-create-a-c-boost-undirected-graph-and-traverse-it-in-depth-first-search
class MSTVisitor : public boost::default_dfs_visitor{
public:
    MSTVisitor(std::vector<std::string> vNames, pcl::PointCloud<pcl::PointNormal>* pCloud):vertNames(vNames),cloud(pCloud){}
    template < typename Edge, typename Graph >
    void tree_edge(Edge e, const Graph& g) const {
        VertexIndexMap vMap = get(boost::vertex_index,g);

        pcl::PointNormal pi = cloud->at(source(e,g));
        pcl::PointNormal pj = cloud->at(target(e,g));
        float prodScal = pi.normal_x*pj.normal_x+pi.normal_y*pj.normal_y+pi.normal_z*pj.normal_z;
        if(prodScal<0.)
        {
            cloud->at(target(e,g)).normal[0] = - cloud->at(target(e,g)).normal_x;
            cloud->at(target(e,g)).normal[1] = - cloud->at(target(e,g)).normal_y;
            cloud->at(target(e,g)).normal[2] = - cloud->at(target(e,g)).normal_z;
        }
    }
private:
    std::vector<std::string> vertNames;
    mutable pcl::PointCloud<pcl::PointNormal>* cloud;
};

normalsEstimator::normalsEstimator(pcl::PointCloud<pcl::PointXYZ> pPointCloud, const IEstimatorObserverAndController *obs):pointCloud(pPointCloud), m_observer((IEstimatorObserverAndController*)obs)
{}

pcl::PointCloud<pcl::PointNormal> normalsEstimator::getNormals(int nbNeighborsNormals)
{
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_smoothed (pointCloud.makeShared());
    pcl::PointCloud<pcl::PointNormal> cloud_smoothed_normals;

    setProgress(0);

    //std::cout<<"    Normals : estimation"<<std::endl;
    pcl::NormalEstimation<pcl::PointXYZ,pcl::Normal>  ne;
    pcl::search::KdTree<pcl::PointXYZ>::Ptr treeNE (new pcl::search::KdTree<pcl::PointXYZ>);
    ne.setInputCloud (cloud_smoothed);
    ne.setSearchMethod(treeNE);
    ne.setKSearch (nbNeighborsNormals);
    pcl::PointCloud<pcl::Normal>::Ptr cloud_normals (new pcl::PointCloud<pcl::Normal>());
    ne.compute (*cloud_normals);

    setProgress(16);

    if(mustStop())
        return cloud_smoothed_normals;

    //std::cout<<"    Normals : smoothing ... "<<std::endl;
    pcl::concatenateFields (*cloud_smoothed, *cloud_normals, cloud_smoothed_normals);

    setProgress(32);

    if(mustStop())
        return cloud_smoothed_normals;

    pcl::search::KdTree<pcl::PointNormal> treePtsNormals;
    treePtsNormals.setInputCloud(cloud_smoothed_normals.makeShared());

    // build a K-nearest neighbors graph
    pcl::KdTreeFLANN<pcl::PointNormal> kdtree;
    kdtree.setInputCloud (cloud_smoothed_normals.makeShared());

    GraphPoisson cloud_graph;

    size_t cloud_smoothed_normals_size = cloud_smoothed_normals.size();

    for (size_t point_i = 0; (point_i < cloud_smoothed_normals_size) && !mustStop(); ++point_i)
    {
        std::vector<int> k_indices (nbNeighborsNormals);
        std::vector<float> k_distances (nbNeighborsNormals);
        kdtree.nearestKSearch (static_cast<int> (point_i), nbNeighborsNormals, k_indices, k_distances);

        for (int k_i = 0; k_i < static_cast<int> (k_indices.size ()); ++k_i){
            pcl::PointNormal pi = cloud_smoothed_normals.at(point_i);
            pcl::PointNormal pj = cloud_smoothed_normals.at(k_indices[k_i]);
            float w = 1 - fabs(pi.normal_x*pj.normal_x+pi.normal_y*pj.normal_y+pi.normal_z*pj.normal_z);
            add_edge (point_i, k_indices[k_i], Weight (w), cloud_graph);
        }


        setProgress(32+((point_i*16)/cloud_smoothed_normals_size));
    }


    if(mustStop())
        return cloud_smoothed_normals;

    //const size_t E = num_edges (cloud_graph),V = num_vertices (cloud_graph);
    //std::cout<<"    Normals : The graph has "<<V<<" vertices and "<<E<<" edges"<<std::endl;

    std::vector <EdgePoisson> spanning_tree;
    kruskal_minimum_spanning_tree(cloud_graph, std::back_inserter(spanning_tree));

    setProgress(64);
    //std::cout<<"    Normals : size of most spanning tree : "<<spanning_tree.size()<<std::endl;

    GraphPoisson MST;

    //create a list of original names for the MST graph.
    std::vector<std::string> mstNames(num_vertices(cloud_graph));
    //Index map for verts in MST, all graphs use an indepenent index system.
    VertexIndexMap mstIndexMap = get(boost::vertex_index, MST);

    size_t spanning_tree_size = spanning_tree.size();

    for(size_t i = 0; (i < spanning_tree_size) && !mustStop(); ++i){

        EdgePoisson e = spanning_tree.at(i);
        Vertex v1 = source(e,cloud_graph);
        Vertex v2 = target(e,cloud_graph);

        //insert the edge to the MST graph
        // Both graphs will share the vertices in verts list.
        std::pair<EdgePoisson,bool> myPair = boost::add_edge(v1,v2,MST);

        //get the vertex index in the MST and set the name to that of original graph
        // mstNames will be used by the visitor
        mstNames.at(mstIndexMap[v1]) = v1;
        mstNames.at(mstIndexMap[v2]) = v2;

        setProgress(64+((i*16)/spanning_tree_size));
    }

    if(mustStop())
        return cloud_smoothed_normals;

    MSTVisitor vis(mstNames,&cloud_smoothed_normals);
    //std::cout << "    Normals : Depth first search on most spanning tree" << std::endl;
    boost::depth_first_search(MST, boost::visitor(vis).root_vertex(0));

    setProgress(100);

    return cloud_smoothed_normals;
}

void normalsEstimator::setProgress(int p)
{
    if(m_observer != nullptr)
        m_observer->setEstimationProgress(p);
}

bool normalsEstimator::mustStop() const
{
    if(m_observer != nullptr)
        return m_observer->mustStopEstimation();

    return false;
}

#endif
