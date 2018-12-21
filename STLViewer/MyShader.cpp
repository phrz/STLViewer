//
//  MyShader.cpp
//  STLViewer
//
//  Created by Paul Herz on 2018-06-14.
//  Copyright © 2018 Paul Herz. All rights reserved.
//

#include "MyShader.hpp"

cinder::gl::GlslProgRef MyShader::makeShader(cinder::vec3 lightLocation) {
	auto program = cinder::gl::GlslProg::create(
		cinder::gl::GlslProg::Format()
		// ============================================
		.vertex(CI_GLSL(150,
			uniform mat4 ciModelViewProjection;
			in vec4 ciPosition;
			in vec4 ciColor;
			out lowp vec4 Color;
			
			uniform vec3 lightLocation;

			void main(void) {
				gl_Position = ciModelViewProjection * ciPosition;
				float lightDist = distance(ciPosition.xyz, lightLocation);
				float lightAmount = smoothstep(0., 1., clamp(
					1. / (lightDist / 10.),
					0., 1.
				));
				
				Color = vec4(
					ciColor.rgb * lightAmount,
					ciColor.a
				);
			}
		))
		// ============================================
		.fragment(CI_GLSL(150,
			uniform mat4 ciModelViewProjection;
			in vec4 Color;
			out vec4 oColor;

			void main(void) {
				oColor = Color;
			}
		))
		// ============================================
	);
	
	program->uniform("lightLocation", lightLocation);
	
	return program;
}
