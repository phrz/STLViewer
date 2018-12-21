//
//  STLFile.hpp
//  STLViewer
//
//  Created by Paul Herz on 2018-06-12.
//  Copyright © 2018 Paul Herz. All rights reserved.
//

#ifndef STLFile_hpp
#define STLFile_hpp

#include <iostream>
#include <vector>

#include "cinder/app/RendererGl.h"

struct STLTriangle {
	using point_t = glm::f32vec3;
	point_t normal;
	point_t points[3];
};

struct STLFile {
	std::vector<STLTriangle> triangles;
};

#endif /* STLFile_hpp */
