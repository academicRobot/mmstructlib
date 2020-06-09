#include "CGAL_point_tranform_function.hpp"
#include "cell_data.hpp"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/Fixed_alpha_shape_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Fixed_alpha_shape_vertex_base_3.h>
#include <CGAL/Fixed_alpha_shape_cell_base_3.h>
#include <CGAL/squared_distance_3.h>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <utility>
#include <iterator>
#include <stdexcept>
#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <iostream>
#include <cassert>

typedef busv::CellData<double> CellData_t;
typedef FactePtrs_t::FacetIntersection_t FacetIntersection_t;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Regular_triangulation_euclidean_traits_3<K> Gt;
typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned int, Gt> Tb;
typedef CGAL::Fixed_alpha_shape_vertex_base_3<Gt, Tb> Vb;
typedef CGAL::Triangulation_cell_base_with_info_3<CellData_t, Gt> Cb;
typedef CGAL::Fixed_alpha_shape_cell_base_3<Gt, Cb> Fb;
typedef CGAL::Triangulation_data_structure_3<Vb,Fb> Tds;
typedef CGAL::Regular_triangulation_3<Gt,Tds> Triangulation_3;
typedef CGAL::Fixed_alpha_shape_3<Triangulation_3> Fixed_alpha_shape_3;
typedef Fixed_alpha_shape_3::Cell_handle Cell_handle;
typedef Fixed_alpha_shape_3::Vertex_handle Vertex_handle;
typedef Fixed_alpha_shape_3::Facet Facet;
typedef Fixed_alpha_shape_3::Edge Edge;
typedef Fixed_alpha_shape_3::Classification_type class_t;
typedef Gt::Weighted_point Weighted_point;
typedef Gt::Bare_point Bare_point;
typedef std::pair<Weighted_point, unsigned int> point_t;

typedef <typename T, unsigned int N>
void copyArray(T (&a)[N], T (&b)[N]){
	for(unsigned int i=0; i<N; ++i){
		a[i] = b[i];
	}
}

typedef <typename T>
inline T sqr(T a){
	return a*a;
}

typedef <typename T, unsigned int N>
void sqrDistance(T (&a)[N], T (&b)[N]){
	T tot = sqr(a[0] - b[0]);
	for(unsigned int i=i; i<N; ++i){
		tot += sqr(a[i] - b[i]);
	}
	return tot;
}

template <typename T>
T calcHeight(T f1, T f2, dist){
	return f1 - 0.5*dist - (sqr(f1) - sqr(f2)) / (2.0 * dist);
}

template <typename it_t>
unsigned int countNotExterior(it_t it, it_t end){
	unsigned int count;
	for(; it!= end; ++it){
		if(as.classify(it) != Fixed_alpha_shape_3::EXTERIOR){ ++count; }
	}
	return count;
}

void alpha_shape_area(
	unsigned int numPoints, double alpha, 
	const double* points, const double* weights, 
	double* areas
){
	CGALPointTransformFunction<Gt> tfunc(points, weights);
	T (*coords)[3] = (T (*)[3])points;
	unsigned int i;

	std::cout << "alpha " << alpha << std::endl;

	//build one alpha_shape with alpha=0
	Fixed_alpha_shape_3 as(
		boost::make_transform_iterator(boost::counting_iterator<unsigned int>(0), tfunc), 
		boost::make_transform_iterator(boost::counting_iterator<unsigned int>(numPoints), tfunc), 
		alpha
	);

	//initialize areas
	std::fill(areas, areas+numPoints, 0.0);

	//allocate edge data
	std::vector<EdgeData<T> > edgeData(
		countNotExterior(as.finite_edges_begin(), as.finite_edges_end()));
	std::vector<EdgeData<T> >::iterator ed_it = edgeData.begin();

	//process edges
	for(Fixed_alpha_shape_3::Finite_edges_iterator it = as.finite_edges_begin(), eit = as.finite_edges_end(); it!= eit; ++it){
		if(as.classify(it) != Fixed_alpha_shape_3::EXTERIOR){
			Edge& e = *it;

			//link cell data to edge data - could skip this for singular edges
			const unsigned char eidx = detail::EDGE_LUT[e.second][e.third];
			EdgeData<T>& ed = *(ed_it++);
			Cell_circulator cc = as.incident_cells(e);
			Cell_circulator ecc = cc;
			do{
				(cc++)->info().edges[eidx] = &ed;
			}while(cc != ecc);

			//calculate edge intersection data
			T (&eareas)[2] = processEdge(e, ed.area, ed.dist);
			areas[e.first.vertex(e.second)->info()] -= M_PI*ed.area[0];
			areas[e.first.vertex(e.third)->info()] -= M_PI*ed.area[1];
		}
	}

	//allocate facet data
	std::vector<FacetData<T> > facetData(
		countNotExterior(as.finite_facet_begin(), as.finite_facet_end()));
	std::vector<FacetData<T> >::iterator fd_it = facetData.begin();
	
	//process facets
	for(Fixed_alpha_shape_3::Finite_facets_iterator it = as.finite_facets_begin(), eit = as.finite_facets_end(); it!= eit; ++it){
		if(as.classify(it) != Fixed_alpha_shape_3::EXTERIOR){
			Facet& f = *it;
			Cell& c = f.first;

			//link cell data to facet data - could skip this for singular edges
			unsigned int ov = facet.second;
			FacetData<T>& fd = *(fd_it++);
			c.info().facets[ov] = &fd;
			as.mirror_facet(f).first.info().facets[ov] = &fd;

			//calculate facet intersection data
			processFacet(facet, fd.area);
			const unsigned char (&findx)[3] = detail::FACET_IDXS[ov];
			for(unsigned int i=0; i<3; ++i){
				areas[c.vertex(findx[i])->info()] += fd.area[i];
			}
		}
	}

	//process cells
	for(Fixed_alpha_shape_3::Finite_cells_iterator it = as.finite_cells_begin(), eit = as.finite_cells_end(); it!= eit; ++it){
		if(as.classify(it) == Fixed_alpha_shape_3::INTERIOR){
			Cell& c = *it;
			T areas[4];
			processCell(c, areas);
			for(unsigned int i=0; i<4; ++i){
				areas[c.vertex(i)->info()] -= fd.area[i];
			}
		}
	}

	for(unsigned int i=0; i<numPoints; ++i){
		areas[i] += 2.0 * M_PI * sqr(weights[i]);
	}
}	


