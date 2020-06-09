#include "alpha_shape_area.hpp"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

#include "weight_data.hpp"
#include "weight_func_obj_min.hpp"
#include "create_alpha_shape.hpp"
#include "alpha_shape_weighted_area.hpp"

typedef float f_t;
typedef size_t i_t;

int main(int argc, char* argv[]){

	feenableexcept(FE_INVALID | FE_OVERFLOW);

	if(argc != 3){
		std::cout << "usage: " << argv[0] << " <window size> <index/coord/radii file>" << std::endl;
		return 1;
	}
	i_t window_size = boost::lexical_cast<i_t>(argv[1]);
	f_t alpha = 0.0;
	const char* infilename = argv[2];

	std::vector<i_t> indices;
	std::vector<f_t> points, radii;
	i_t index;
	f_t x, y, z, r;
	std::ifstream fs;
	fs.open(infilename, std::ifstream::in);
	while(fs >> index >> x >> y >> z >> r){
		indices.push_back(index);
		points.push_back(x);
		points.push_back(y);
		points.push_back(z);
		radii.push_back(r);
	}
	size_t numPoints = radii.size();
	std::cout << "number of points: " << numPoints << std::endl;

	std::vector<f_t> areas(numPoints);

	if(0){
		busv::alpha_shape_area<f_t, i_t>(numPoints, alpha, &(points[0]), &(radii[0]), &(areas[0]));
	}
	else{
		std::vector<busv::weight_data<f_t, i_t> > wd(numPoints);
		for(i_t i = 0; i<numPoints; ++i){
			wd[i].index = indices[i];
		}
		busv::weight_func_obj_min<f_t, i_t> wfo(&(wd[0]), window_size);
		busv::AlphaShapeContainer<f_t, i_t> asc = busv::create_alpha_shape(numPoints, alpha,  &(points[0]), &(radii[0]));
		busv::alpha_shape_weighted_area<f_t, i_t>(asc, wfo, &(areas[0]));
	}

	for(i_t i = 0; i<numPoints; ++i){
		std::cout << i << "\t" << indices[i] << "\t" << areas[i] << std::endl;
	}
	std::cout << std::endl;

	return 0;
}

