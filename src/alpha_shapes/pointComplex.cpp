#include "alpha_shapes.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

typedef float f_t;

int main(int argc, char* argv[]){
	if(argc != 3){
		std::cout << "usage: " << argv[0] << " <alpha> <coord/radii file>" << std::endl;
		return 1;
	}
	f_t alpha = boost::lexical_cast<f_t>(argv[1]);
	const char* infilename = argv[2];

	std::vector<f_t> points, radii;
	f_t x, y, z, r;
	std::ifstream fs;
	fs.open(infilename, std::ifstream::in);
	while(fs >> x >> y >> z >> r){
		points.push_back(x);
		points.push_back(y);
		points.push_back(z);
		radii.push_back(r);
	}
	size_t numPoints = radii.size();
	std::cout << "number of points: " << numPoints << std::endl;

	unsigned int numEdges, numTriangles, numTetrahedra;
	unsigned int* edges, * triangles, * tetrahedra, * t;
	busv::alpha_shapes<f_t, unsigned int>(
		numPoints, alpha, 
		&(points[0]), &(radii[0]), 
		&numEdges, &edges,
		&numTriangles, &triangles,
		&numTetrahedra, &tetrahedra
	);
	
	std::cout << "tetrahedra" << std::endl;
	t = tetrahedra;
	for(unsigned int i = 0; i<numTetrahedra; ++i){
		std::cout << "\t" << t[0] << " " << t[1] << " " << t[2] << " " << t[3] << std::endl;
		t += 4;
	}
	std::cout << std::endl;

	std::cout << "triangles" << std::endl;
	t = triangles;
	for(unsigned int i = 0; i<numTriangles; ++i){
		std::cout << "\t" << t[0] << " " << t[1] << " " << t[2] << std::endl;
		t += 3;
	}
	std::cout << std::endl;

	std::cout << "edges" << std::endl;
	t = edges;
	for(unsigned int i = 0; i<numEdges; ++i){
		std::cout << "\t" << t[0] << " " << t[1] << std::endl;
		t += 2;
	}
	std::cout << std::endl;

	return 0;
}

