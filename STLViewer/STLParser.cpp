//
//  STLParser.cpp
//  STLViewer
//
//  Created by Paul Herz on 2018-06-11.
//  Copyright © 2018 Paul Herz. All rights reserved.
//

#include "STLParser.hpp"

template<typename T>
T STLParser::readBinary(std::ifstream& ifs) {
	T out;
	ifs.read(
		reinterpret_cast<char*>(&out),
		sizeof(T)
	);
	return out;
}

/// expect the next word to be the given literal, and consume it.
bool STLParser::expectLiteral(std::ifstream& ifs, std::string literal) {
	std::string s {};
	ifs >> s;
	if(s == literal) {
		return true;
	} else if(ifs.bad()) {
		std::cerr << "[STLParser::expectLiteral] I/O error." << std::endl;
	} else if(ifs.eof()) {
		std::cerr << "[STLParser::expectLiteral] Unexpected EOF." << std::endl;
	} else {
		std::cerr << "[STLParser::expectLiteral] Expected literal '" << literal << "', got '" << s << "'." << std::endl;
	}
	return false;
}

/// expect a series of words/tokens/literals (ignoring whitespace, using it as a delimiter)
bool STLParser::expectLiterals(std::ifstream& ifs, std::initializer_list<std::string> literals) {
	for(auto literal : literals) {
		if(!expectLiteral(ifs, literal)) {
			return false;
		}
	}
	return true;
}

/// get the next word without consuming the stream
optional<std::string> STLParser::peekWord(std::ifstream& ifs) {
	auto position = ifs.tellg();
	
	if(position == -1) {
		// tellg error state
		std::cerr
		<< "Warning: tellg() returned -1 (error state) in STLParser::peekWord"
		<< std::endl;
		return nullopt;
	}
	
	std::string word;
	ifs >> word;
	ifs.seekg(position);
	
	if(ifs.bad() || ifs.fail()) {
		std::cerr
		<< "Warning: received `bad` or `fail` state resulting from extraction "
		<< "and seeking in STLParser::peekWord" << std::endl;
		return nullopt;
	}
	
	return word;
}

/// expects a string "{x} {y} {z}", where x, y, and z are
/// string-formatted floats that istream can parse.
bool STLParser::readVec3String(std::ifstream& ifs, float& x, float& y, float& z) {
	if(ifs >> x >> y >> z) {
		return true;
	} else if(ifs.bad()) {
		std::cerr << "[STLParser::readVec3String] I/O error." << std::endl;
	} else if(ifs.eof()) {
		std::cerr << "[STLParser::readVec3String] Unexpected EOF." << std::endl;
	} else {
		std::cerr << "[STLParser::readVec3String] Unknown error." << std::endl;
	}
	return false;
}

optional<STLFile> STLParser::parseBinarySTL(std::ifstream& file) {
	// Skipping header (80 bytes)
	file.seekg(80);
	
	auto triangleCount = STLParser::readBinary<std::uint32_t>(file);
	std::cout << triangleCount << " triangles" << std::endl;
	
	// normal: x y z
	// points[1..3]: x y z
	STLFile stlModel;
	stlModel.triangles.reserve(triangleCount);
	
	for(uint32_t i = 0; i < triangleCount; i++) {
		stlModel.triangles.push_back(readBinary<STLTriangle>(file));
		readBinary<uint16_t>(file); // ignore attribute
	}
	
	return stlModel;
}

optional<STLFile> STLParser::parseASCIISTL(std::ifstream& file) {
	std::string s;
	optional<std::string> maybeNextWord;
	
	float x = 0, y = 0, z = 0;
	STLFile stlModel;
	
	// skip "solid" detection, which was already detected in openSTL
	/*
	// header: "solid"
	expectLiteral(file, "solid");
	*/
	
	// ignore name, assume there can be spaces
	file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	
	// facet loop
	while(!file.eof()) {
		STLTriangle triangle;
		
		// instead of 'facet', it could be 'endsolid', so peek ahead to check
		maybeNextWord = peekWord(file);
		if(maybeNextWord && maybeNextWord.get() == "endsolid") {
			break;
		}
		
		// expect 'facet normal {x} {y} {z}'
		if(!expectLiterals(file, {"facet", "normal"}) || !readVec3String(file, x, y, z)) {
			return nullopt;
		}
		triangle.normal = STLTriangle::point_t(x,y,z);
		
		// expect 'outer loop'
		if(!expectLiterals(file, {"outer", "loop"})) {
			return nullopt;
		}
		
		// three lines 'vertex {x} {y} {z}'
		for(int v = 0; v < 3; v++) {
			if(!expectLiteral(file, "vertex") || !readVec3String(file, x, y, z)) {
				return nullopt;
			}
			triangle.points[v] = STLTriangle::point_t(x,y,z);
		}
		
		// expect 'endloop endfacet'
		if(!expectLiterals(file, {"endloop", "endfacet"})) {
			return nullopt;
		}
		
		stlModel.triangles.push_back(triangle);
	}
	
	return stlModel;
}

optional<STLFile> STLParser::openSTL(std::string fileName) {
	// the file can either be ASCII or binary. Begin in binary mode, and
	// check the first five bytes to see if they are "solid" (ASCII).
	std::ifstream file { fileName, std::ios::binary };
	
	size_t constexpr KEYWORD_SIZE = 5; // s o l i d
	char buffer[KEYWORD_SIZE] { 0 };
	file.read(reinterpret_cast<char*>(&buffer), sizeof buffer);
	
	bool isASCII = strncmp(buffer, "solid", sizeof buffer) == 0;
	
	if(isASCII) {
		std::cout << "ASCII mode detected for '" << fileName << "'." << std::endl;
		return parseASCIISTL(file);
	}
	
	std::cout << "Binary mode detected for '" << fileName << "'." << std::endl;
	return parseBinarySTL(file);
}
