#include "material.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_map>

#include "../resources.h"

static std::unordered_map<std::string, std::shared_ptr<material>> m_materials;
static std::unordered_map<std::string, std::shared_ptr<texture>> m_textures;

inline void trim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
}

bool loadMaterials(const std::string &source) {
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
			auto new_tex = std::make_shared<texture>(loadImageFromResource(tex_id.c_str()));
			m_textures[tex_name] = new_tex;

			while (! line_iss.eof()) {
				line_iss >> option;
				if (option == "linear") {
					new_tex->setFilter(GL_LINEAR);
				} else if (option == "nearest") {
					new_tex->setFilter(GL_NEAREST);
				} else if (option == "clamp") {
					new_tex->setWrapParam(GL_CLAMP_TO_EDGE);
				} else if (option == "repeat") {
					new_tex->setWrapParam(GL_REPEAT);
				} else {
					std::cerr << "Sintax error at line #" << line_num << ": unexpected option " << option << std::endl;
					return false;
				}
			}
		} else if (token == "material") {
			std::string mat_name;
			line_iss >> mat_name;
			if (line_iss.fail()) {
				std::cerr << "Syntax error at line #" << line_num << "\n" << line << "\n";
				return false;
			}
			auto m = std::make_shared<material>();
			m_materials[mat_name] = m;

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
					m->tex = getTexture(tex_name);
				} else if (token == "normals") {
					std::string tex_name;
					line_iss >> tex_name;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected texture name\n";
						return false;
					}
					m->normals = getTexture(tex_name);
				} else if (token == "specmap") {
					std::string tex_name;
					line_iss >> tex_name;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected texture name\n";
						return false;
					}
					m->specmap = getTexture(tex_name);
				} else if (token == "ambient") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m->ambient = col;
				} else if (token == "diffuse") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m->diffuse = col;
				} else if (token == "specular") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m->specular = col;
				} else if (token == "emissive") {
					int col;
					line_iss >> std::hex >> col;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m->emissive = col;
				} else if (token == "shininess") {
					float s;
					line_iss >> std::dec >> s;
					if (line_iss.fail()) {
						std::cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected float\n";
						return false;
					}
					m->shininess = s;
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

void cleanupMaterials() {
	m_materials.clear();
	m_textures.clear();
}

const std::shared_ptr<texture> getTexture(const std::string &name) {
	auto it = m_textures.find(name);
	if (it != m_textures.end()) {
		return it->second;
	} else {
		std::cout << "Could not find texture id " << name << std::endl;
		return nullptr;
	}
}

const std::shared_ptr<material> getMaterial(const std::string &name) {
	auto it = m_materials.find(name);
	if (it != m_materials.end()) {
		return it->second;
	} else {
		std::cout << "Could not find material id " << name << std::endl;
		return nullptr;
	}
}