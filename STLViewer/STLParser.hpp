//
//  STLParser.hpp
//  STLViewer
//
//  Created by Paul Herz on 2018-06-11.
//  Copyright © 2018 Paul Herz. All rights reserved.
//

#ifndef STLParser_hpp
#define STLParser_hpp

#include <fstream>
#include <iostream>
#include <string>
#include <initializer_list>

#include<boost/optional.hpp>
#include<boost/none_t.hpp>
#include<boost/none.hpp>

template<typename T>
using optional = boost::optional<T>;
using nullopt_t = boost::none_t;
#define nullopt boost::none

#include "STLFile.hpp"

class STLParser {
private:
	template<typename T>
	static T readBinary(std::ifstream& ifs);
	
	static bool expectLiteral(std::ifstream& ifs, std::string literal);
	static bool expectLiterals(std::ifstream& ifs, std::initializer_list<std::string> literals);
	static bool readVec3String(std::ifstream& ifs, float& x, float& y, float& z);
	static optional<std::string> peekWord(std::ifstream& ifs);
	
	static optional<STLFile> parseBinarySTL(std::ifstream& file);
	static optional<STLFile> parseASCIISTL(std::ifstream& file);
	
public:
	static optional<STLFile> openSTL(std::string fileName);
};

#endif /* STLParser_hpp */
