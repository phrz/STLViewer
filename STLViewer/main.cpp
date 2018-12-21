//
//  main.cpp
//  STLViewer
//
//  Created by Paul Herz on 2018-06-11.
//  Copyright © 2018 Paul Herz. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Shader.h"
#include "cinder/TriMesh.h"
#include "cinder/CameraUi.h"
#include "cinder/Text.h"

#include "STLParser.hpp"
#include "MyShader.hpp"

using namespace ci;
using namespace ci::app;

class STLViewerApp: public App {
private:
	vec3 initialCameraPosition = vec3(60, -36, 110);
	
	CameraPersp camera;
	CameraUi cameraUi;
	TextBox textBox;
	
	gl::TextureRef makePositionTextTexture();
	TriMeshRef makeMeshFromSTLFile(STLFile const& stl);
	gl::GlslProgRef flatShader, shader;
	gl::BatchRef textBatch, stlBatch;//, planeBatch;
	
	TriMeshRef stlMesh;
//	Timer timer;
public:
	void setup() override;
	void draw() override;
};

TriMeshRef STLViewerApp::makeMeshFromSTLFile(STLFile const& stl) {
	auto format = TriMesh::Format().positions().normals();
	TriMeshRef mesh = TriMesh::create(format);
	
	for (auto triangle : stl.triangles) {
		mesh->appendPositions(triangle.points, 3);
		auto v = static_cast<uint32_t>(mesh->getNumVertices());
		mesh->appendTriangle(v - 3, v - 2, v - 1);
	}
	mesh->recalculateNormals(false); // smooth = false
	
	return mesh;
}

void STLViewerApp::setup() {
	optional<STLFile> stl = STLParser::openSTL("/Users/phrz/Desktop/stress.stl");
	if(!stl) {
		console() << "Could not load STL.\n";
		exit(1);
	}
	
	// get shader based on distance from light source
//	this->shader = MyShader::makeShader(vec3());
	
	// Build mesh from loaded STL data
	// (unwrap optional)
	this->stlMesh = makeMeshFromSTLFile(stl.get());
	
	// Camera setup
	// initial position, looking at [origin]
	this->camera.lookAt(this->initialCameraPosition, vec3());
	// allow mouse manipulation of camera
	this->cameraUi = CameraUi(&camera, STLViewerApp::getWindow());
	
	// Textbox setup
	textBox.setColor(Color::white());
	textBox.font(Font("Helvetica Bold", 36));
	
	// turn on z-buffering
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	// setup shading
	this->flatShader = gl::getStockShader(
		gl::ShaderDef().color()
	);
	
	this->shader = gl::getStockShader(gl::ShaderDef().lambert());
	
	// setup static batches
	stlBatch = gl::Batch::create(*stlMesh, shader);
	
//	auto plane = geom::Plane().origin(vec3()).normal(vec3(0,0,1)).size(vec2(100,100));
//	planeBatch = gl::Batch::create(plane, flatShader);
	
//	timer.start();
}

void STLViewerApp::draw() {
	gl::clear();
	gl::ScopedFaceCulling scopedCulling(true, GL_BACK);
	
	// setup for moving light source
//	double s = timer.getSeconds();
//	float sl = 13.f;
//	vec3 lightLocation {
//		sl * float(sin(s)),
//		sl * float(cos(s)),
//		sl
//	};
//	gl::BatchRef sphereBatch = gl::Batch::create(
//		geom::Sphere().center(lightLocation).radius(1.f),
//		flatShader
//	);
//	this->shader->uniform("lightLocation", lightLocation);
	
	// 3D part
	// render based on the camera position
	// ========================================
	gl::setMatrices(this->camera);
	
//	gl::color(1.f, 0.f, 0.f);
//	sphereBatch->draw();
	
//	gl::color(0.f, .1f, .3f);
//	planeBatch->draw();
	
	gl::color(Color::white());
	stlBatch->draw();
	
	// 2D part
	// ========================================
	gl::setMatricesWindow(getWindowSize());
	
	// draw text box
	auto tx = this->makePositionTextTexture();
	Rectf drawRect(
		0, // x0
		0, // y0
		tx->getWidth() / 2, // x1
		tx->getHeight() / 2 // y1
	);
	gl::draw(tx, drawRect);

}

gl::TextureRef STLViewerApp::makePositionTextTexture() {
	// get the position as a string
	// and set the textbox to it
	std::stringstream ss;
	vec3 eye = this->camera.getEyePoint();
	
	ss
	<< std::setiosflags(std::ios::fixed)
	<< std::setprecision(1)
	<< "X" << eye.x << " Y" << eye.y << " Z" << eye.z;
	
	this->textBox.setText(ss.str());
	
	// Render the textbox to a surface and draw it
	Surface textSurface = this->textBox.render();
	return gl::Texture::create(textSurface);
}

CINDER_APP(STLViewerApp, RendererGl)
