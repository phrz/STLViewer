//
//  MyShader.hpp
//  STLViewer
//
//  Created by Paul Herz on 2018-06-14.
//  Copyright © 2018 Paul Herz. All rights reserved.
//

#ifndef MyShader_hpp
#define MyShader_hpp

#include "cinder/gl/gl.h"

class MyShader {
public:
	static cinder::gl::GlslProgRef makeShader(cinder::vec3 lightLocation);
};

#endif /* MyShader_hpp */
