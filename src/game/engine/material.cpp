#include "material.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>

std::map<std::string, material> material::mat;
std::map<std::string, texture> material::texs;

inline void trim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
}

bool material::loadMaterials(const std::string &source) {
	if (source.empty()) {
		std::cerr << "Could not load materials\n";
		return false;
	}

	std::istringstream iss(source);

	std::string line;
	int line_num = 0;

	while (getline(iss, line)) {
		trim(line);

		++line_num;

		if (line.empty()) continue;
		if (line.at(0) == '#') continue;

		std::istringstream line_iss(line);

		std::string token;
		line_iss >> token;
		if (token == "texture") {
			std::string tex_name, tex_id, option;
			line_iss >> tex_name >> tex_id; // texture name
			if (line_iss.fail()) {
				std::cerr << "Syntax error at line #" << line_num << "\n" << line << "\n";
				return false;
			}
			texture new_tex(loadImageFromResource(tex_id.c_str()));
			while (! line_iss.eof()) {
				line_iss >> option;
				if (option == "linear") {
					new_tex.setFilter(GL_LINEAR);
				} else if (option == "nearest") {
					new_tex.setFilter(GL_NEAREST);
				} else if (option == "clamp") {
					new_tex.setWrapParam(GL_CLAMP_TO_EDGE);
				} else if (option == "repeat") {
					new_tex.setWrapParam(GL_REPEAT);
				} else {
					std::cerr << "Sintax error at line #" << line_num << ": unexpected option " << option << std::endl;
					return false;
				}
			}
			texs.emplace(tex_name, std::move(new_tex));
		} else if (token == "material") {
			std::string mat_name;
			line_iss >> mat_name;
			material &m = mat[mat_name];

			std::string token;
			while (!line_iss.eof()) {
				line_iss >> token;
				if (token == "texture") {
					std::string tex_name;
					line_iss >> tex_name;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected texture name\n";
						return false;
					}
					m.tex = tex_name;
				} else if (token == "normals") {
					std::string tex_name;
					line_iss >> tex_name;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected texture name\n";
						return false;
					}
					m.normals = tex_name;
				} else if (token == "ambient") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.ambient = col;
				} else if (token == "diffuse") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.diffuse = col;
				} else if (token == "specular") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.specular = col;
				} else if (token == "emissive") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.emissive = col;
				} else if (token == "shininess") {
					float s;
					line_iss >> std::dec >> s;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected float\n";
						return false;
					}
					m.shininess = s;
				} else {
					std::cerr << "Sintax error at line #" << line_num << ": unexpected token \"" << token << "\"\n";
					return false;
				}
			}
		} else {
			std::cerr << "Sintax error at line #" << line_num << ": unexpected token \"" << token << "\"\n";
			return false;
		}
	}

	return true;
}

const texture &material::getTexture(const std::string &name) {
	static const texture TEX_DEFAULT;
	auto it = texs.find(name);
	if (it != texs.end()) {
		return it->second;
	} else {
		return TEX_DEFAULT;
	}
}

const material &material::get(const std::string &name) {
	static const material MAT_DEFAULT;
	auto it = mat.find(name);
	if (it != mat.end()) {
		return it->second;
	} else {
		return MAT_DEFAULT;
	}
}