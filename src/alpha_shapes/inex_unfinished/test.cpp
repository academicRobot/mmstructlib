#include "alpha_shapes.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>

int main(int argc, char* argv[]){
	if(argc != 2){
		std::cout << "usage: " << argv[0] << " <alpha>" << std::endl;
		return 1;
	}
	double alpha = boost::lexical_cast<double>(argv[1]);

	unsigned int num_edges, num_triangles, num_tetrahedra;
	double points[] = {
		.2,.2,.2,
		1,0,0,
		0,1,0,
		0,0,1,
		.7,.7,.7
	};
	double weights[] = {
		1, 1, 1, 1, 1
	};
	unsigned int* edges;
	unsigned int* triangles;
	unsigned int* tetrahedra;

	alpha_shapes(
		5, alpha, points, weights,
		&num_edges, &edges, 
		&num_triangles, &triangles, 
		&num_tetrahedra, &tetrahedra
	);
	
	std::cout << "Tetra " << num_tetrahedra << std::endl;
	for(unsigned int i = 0; i < num_tetrahedra*4; i+=4){
		for(unsigned int j = 0; j < 4; ++j){
			std::cout << "\t" << tetrahedra[i+j];
		}
		std::cout << std::endl;
	}

	std::cout << "Tri " << num_triangles << std::endl;
	for(unsigned int i = 0; i < num_triangles*3; i+=3){
		for(unsigned int j = 0; j < 3; ++j){
			std::cout << "\t" << triangles[i+j];
		}
		std::cout << std::endl;
	}

	std::cout << "Edge " << num_edges << std::endl;
	for(unsigned int i = 0; i < num_edges*2; i+=2){
		for(unsigned int j = 0; j < 2; ++j){
			std::cout << "\t" << edges[i+j];
		}
		std::cout << std::endl;
	}

	return 0;
}

