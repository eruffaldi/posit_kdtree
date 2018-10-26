/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2011-2016 Jose Luis Blanco (joseluisblancoc@gmail.com).
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/

#include <nanoflann.hpp>
#include "nanoflann_utils.hpp"
#include "nanoflann_castclone.hpp"
#include "posit.h"
#include "posit12.hpp"
#include "posit10.hpp"
#include "posit8.hpp"
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "args.hxx"
using namespace std;
using namespace nanoflann;


float radius=0.1;
bool nolist = false;

template <class T>
void saveTreeObj(T & tree, std::ostream & onf, std::string pre)
{
	if((tree->child1 == NULL) && (tree->child2 == NULL))
	{
		onf << pre << " leaf " << tree->node_type.lr.right-tree->node_type.lr.left << " nodes from " << tree->node_type.lr.left << std::endl;
	}
	else
	{
		onf << pre << " feat:" << tree->node_type.sub.divfeat << " range:" << (float)tree->node_type.sub.divlow << " " << (float)tree->node_type.sub.divhigh << std::endl;
		saveTreeObj(tree->child1,onf,pre+"1");	
		saveTreeObj(tree->child2,onf,pre+"2");			
	}
}

template <class T>
void saveTree(T & tree, const char * f)
{
	std::ofstream onf(f);
	saveTreeObj(tree.root_node,onf,"");
}


template <typename src_t,typename dst_t, typename tmp_t = src_t>
void copyPointCloud(PointCloud<dst_t> &y, const PointCloud<src_t> & x, tmp_t f = tmp_t())
{
	if((void*)&y == (void*)&x)
		return;

	y.pts.resize(x.pts.size());
	for(size_t i = 0; i < y.pts.size(); i++)
	{
		typename PointCloud<dst_t>::Point & pd = y.pts[i];
		const typename PointCloud<src_t>::Point & ps = x.pts[i];
		pd.p.x = (dst_t)(tmp_t)ps.p.x;
		pd.p.y = (dst_t)(tmp_t)ps.p.y;
		pd.p.z = (dst_t)(tmp_t)ps.p.z;
	}
}

// construct a kd-tree index:
template <class num_t>
using baseindex =  KDTreeSingleIndexAdaptor<
	L2_Adaptor<num_t, PointCloud<num_t> > ,
	PointCloud<num_t>,
	3 /* dim */
	>;
using my_kd_tree_t_float = baseindex<float>;

template <typename num_t>
void kdtree_demo(const PointCloud<num_t> & cloud, const char * targetsave, const char * iot, my_kd_tree_t_float *ff)
{

	using my_kd_tree_t = baseindex<num_t>;


	my_kd_tree_t   index(3 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
    typename my_kd_tree_t::BoundingBox bb;
    auto beginA = std::chrono::high_resolution_clock::now();
    index.computeBoundingBox(bb);
    auto endA = std::chrono::high_resolution_clock::now();

    auto begin = std::chrono::high_resolution_clock::now();
    if(ff)
    {
    	castcopyindex(ff,&index);
    }
    else
		index.buildIndex();
    auto end = std::chrono::high_resolution_clock::now();
    if(iot != 0)
    {
		/*	FILE *f = fopen(iot, "wb");
		if (!f) throw std::runtime_error("Error writing index file!");
		index.saveIndex(f);
		fclose(f);    	
		*/
		saveTree(index,iot);
    }
	std::cout << "item size:" << sizeof(num_t) << std::endl;
	std::cout << "bbox time(us):" << std::chrono::duration_cast<std::chrono::microseconds>(endA - beginA).count() << std::endl;
	std::cout << "building time(us):" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
	std::cout << "index memory:" << index.usedMemory(index) << std::endl;
	std::cout << "points: " << index.m_size << " dim:" <<  index.dim << " indice:" << index.vind.size() << " m_leaf_max_size:" << index.m_leaf_max_size << std::endl;
	std::cout << "EPS inside " << (float)(static_cast<num_t>(0.00001)) << " and " << (float)(num_t(1) - num_t(0.00001)) << std::endl;
	//std::cout << "bbox: " << index.root_bbox << std::endl;

#if 0
	// Test resize of dataset and rebuild of index:
	cloud.pts.resize(cloud.pts.size()*0.5);
	index.buildIndex();
#endif

	const num_t query_pt[3] = { num_t(0.5), num_t(0.5), num_t(0.5)};

	// ----------------------------------------------------------------
	// knnSearch():  Perform a search for the N closest points
	// ----------------------------------------------------------------
	{
		size_t num_results = 5;
		std::vector<size_t>   ret_index(num_results);
		std::vector<num_t> out_dist_sqr(num_results);

	    auto begin = std::chrono::high_resolution_clock::now();
		num_results = index.knnSearch(&query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);
	    auto end = std::chrono::high_resolution_clock::now();
		
		// In case of less points in the tree than requested:
		ret_index.resize(num_results);
		out_dist_sqr.resize(num_results);
		std::cout << "knnSearch time(us):" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;

		cout << "knnSearch(): num_results=" << num_results << "\n";
		if(!nolist)
		for (size_t i = 0; i < num_results; i++)
			cout << "idx["<< i << "]=" << ret_index[i] << " dist["<< i << "]=" << (float) out_dist_sqr[i] << endl;
		cout << "\n";
	}

	// ----------------------------------------------------------------
	// radiusSearch():  Perform a search for the N closest points
	// ----------------------------------------------------------------
	{
	    auto begin = std::chrono::high_resolution_clock::now();
		const num_t search_radius = static_cast<num_t>(radius);
		std::vector<std::pair<size_t,num_t> >   ret_matches;

		nanoflann::SearchParams params;
		//params.sorted = false;

		const size_t nMatches = index.radiusSearch(&query_pt[0], search_radius, ret_matches, params);
	    auto end = std::chrono::high_resolution_clock::now();
		std::cout << "radiusSearch time:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;

		cout << "radiusSearch(): radius=" << search_radius << " -> " << nMatches << " matches\n";
		if(!nolist)
		for (size_t i = 0; i < nMatches; i++)
			cout << "idx["<< i << "]=" << ret_matches[i].first << " dist["<< i << "]=" << (float)ret_matches[i].second << endl;
		cout << "\n";
	}

}

int main(int argc, char * argv[])
{
	args::ArgumentParser parser("nanoflann test",                                "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<int> siter(parser, "iter", "Iterations", {'i', "iterations"},3);
    args::ValueFlag<float> sspan(parser, "span", "Span", {'s', "span"},10.0);
    args::ValueFlag<bool> ssave(parser, "save", "Save", {'S', "save"},false);
    args::ValueFlag<bool> sfloat(parser, "usefloat", "usefloat", {'f', "float"},false);
    args::ValueFlag<float> sradius(parser, "radius", "Radius", {'r', "radius"},0.1);
    args::ValueFlag<int> spoints(parser, "points", "Points", {'n', "points"},10000);
    args::ValueFlag<bool> alist(parser, "list", "List Results", {'l', "list"},false);

    try {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help) {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }


	// Randomize Seed
	srand(time(NULL));

	using P =Posit<int16_t,12,2,uint32_t,PositSpec::WithInf> ;

	radius=args::get(sradius);
	nolist = !args::get(alist);

	using p16=Posit<int16_t,8,1,uint32_t,PositSpec::WithInf>;
	PointCloud<float> cloudf;
	PointCloud<double> cloudd;
	PointCloud<P> cloudp;
	PointCloud<posit12> cloudp12;
	PointCloud<posit10> cloudp10;
	PointCloud<posit8> cloudp8;
	PointCloud<p16> cloudp16;
	auto & base = cloudp8;
	generateRandomPointCloud(base, args::get(spoints), posit8(args::get(sspan))); // generate 10000 points over 0...10
	copyPointCloud(cloudd,base,float());
	copyPointCloud(cloudp,base,float());
	copyPointCloud(cloudp12,base,float());
	copyPointCloud(cloudp10,base,float());
	copyPointCloud(cloudf,base,float());
	copyPointCloud(cloudp16,base,float());

	my_kd_tree_t_float * pff = 0;
	if(sfloat)
	{
		pff = new my_kd_tree_t_float(3,  cloudf, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
		pff->buildIndex();
	}

	int N = args::get(siter);
	for(int i = 0; i < N; i++)
	{
		std::cout << "\n-----------------\nDouble Precision\n";
		kdtree_demo<double>(cloudd,"doubleindex","doubleindex.txt",pff);
	}
	for(int i = 0; i < N; i++)
	{
		std::cout << "\n-----------------\nSingle Precision\n";
		kdtree_demo<float>(cloudf,0,"singleindex.txt",pff);
	}
	/*std::cout << "\n-----------------\n" << typeid(P).name() << std::endl;
	kdtree_demo<P>(cloudp);
	std::cout << "\n-----------------\nPosit12 Tabulated\n";
	kdtree_demo<posit12>(cloudp12);
	*/
	for(int i = 0; i < N+1; i++)
	{
		if(i == 0)
		std::cout << "\n-----------------\nPosit10 first\n";
	else
		std::cout << "\n-----------------\nPosit10\n";
	    kdtree_demo<posit10>(cloudp10,0,"p10index.txt",pff);
	}

	for(int i = 0; i < N+1; i++)
	{
		if(i == 0)
		std::cout << "\n-----------------\nPosit8 first\n";
	else
		std::cout << "\n-----------------\nPosit8\n";
	    kdtree_demo<posit8>(cloudp8,0,"p8index.txt",pff);
	}	

	for(int i = 0; i < N+1; i++)
	{
		if(i == 0)
		std::cout << "\n-----------------\nPosit16 first\n";
	else
		std::cout << "\n-----------------\nPosit16\n";
	    kdtree_demo<p16 >(cloudp16,0,"p16index.txt",pff);
	}	

	delete pff;
	return 0;
}


/*
DistanceType epsError(1 + searchParams.eps); 
*/
