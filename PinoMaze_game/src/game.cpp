#include "game.h"

#include "resources.h"
#include "globals.h"

#include <glm/gtc/matrix_transform.hpp>

#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>

game::game(maze *m) : m(m), world(m), hole(m), teleport(m), logic(m) {}

bool game::init() {
	if (!loadMaterials()) return false;

	if (!skybox.init()) return false;
	if (!world.init()) return false;
	if (!hole.init()) return false;
	if (!teleport.init()) return false;
	if (!particle.init()) return false;
	if (!hud.init()) return false;

	if (!logic.init()) return false;

    m_proj = glm::perspective(glm::radians(90.f), (float)windowWidth / (float)windowHeight, 0.1f, skyboxSize * 2.f);

	sun.direction = glm::vec3(0.43555f, 0.5f, -0.25391f);

    return true;
}

void game::handleEvent(SDL_Event &e) {
	logic.handleEvent(e);
}

void game::tick() {
    logic.tick();

	m_view = logic.getCamera().viewMatrix();
	m_marble = logic.marbleMatrix();

	world.tick(this);
	hole.tick(this);
	particle.tick(this);
}

void game::render() {
	glClear(GL_DEPTH_BUFFER_BIT);

    skybox.render(this);
	world.render(this);
	teleport.render(this);

	hole.bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT);
	skybox.render(this);
	world.renderRefraction(this);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	hole.render(this);
	particle.render(this);

	hud.render();
}

light game::viewLight() {
	light l = sun;
	l.direction = glm::vec3(m_view * glm::vec4(sun.direction, 0.0));
	return l;
}

static void trim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
}

bool game::loadMaterials() {
	string txt = loadStringFromResource("IDM_MATERIALS");
	if (txt.empty()) {
		cerr << "Could not load materials\n";
		return false;
	}

	istringstream iss(txt);

	string line;
	int line_num = 0;

	map<string, shared_ptr<texture>> tex;

	while (getline(iss, line)) {
		const char *c_line = line.c_str();
		
		trim(line);

		++line_num;

		if (line.empty()) continue;
		if (line.at(0) == '#') continue;

		istringstream line_iss(line);

		string token;
		line_iss >> token;
		if (token == "texture") {
			string tex_name, tex_id;
			line_iss >> tex_name >> tex_id; // texture name
			if (line_iss.fail()) {
				cerr << "Syntax error at line #" << line_num << "\n" << line << "\n";
				return false;
			}
			tex[tex_name] = make_shared<texture>(loadImageFromResource(tex_id.c_str()));
		} else if (token == "material") {
			string mat_name;
			line_iss >> mat_name;
			material &m = mat[mat_name];

			string token;
			while (!line_iss.eof()) {
				line_iss >> token;
				if (token == "texture") {
					string tex_name;
					line_iss >> tex_name;
					if (line_iss.fail()) {
						cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected texture name\n";
						return false;
					}
					auto it = tex.find(tex_name);
					if (it != tex.end()) {
						m.tex = it->second;
					} else {
						cerr << "Could not find texture " << tex_name << ", skipping\n";
					}
				} else if (token == "normals") {
					string tex_name;
					line_iss >> tex_name;
					if (line_iss.fail()) {
						cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected texture name\n";
						return false;
					}
					auto it = tex.find(tex_name);
					if (it != tex.end()) {
						m.normals = it->second;
					} else {
						cerr << "Could not find texture " << tex_name << ", skipping\n";
					}
				} else if (token == "ambient") {
					int col;
					line_iss >> hex >> col;
					if (line_iss.fail()) {
						cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.ambient = col;
				} else if (token == "diffuse") {
					int col;
					line_iss >> hex >> col;
					if (line_iss.fail()) {
						cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.diffuse = col;
				} else if (token == "specular") {
					int col;
					line_iss >> hex >> col;
					if (line_iss.fail()) {
						cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.specular = col;
				} else if (token == "emissive") {
					int col;
					line_iss >> hex >> col;
					if (line_iss.fail()) {
						cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected hex color\n";
						return false;
					}
					m.emissive = col;
				} else if (token == "shininess") {
					float s;
					line_iss >> dec >> s;
					if (line_iss.fail()) {
						cerr << "Syntax error at line #" << line_num << "\n" << line << "Expected float\n";
						return false;
					}
					m.shininess = s;
				} else {
					cerr << "Sintax error at line #" << line_num << ": unexpected token \"" << token << "\"\n";
					return false;
				}
			}
		} else {
			cerr << "Sintax error at line #" << line_num << ": unexpected token \"" << token << "\"\n";
			return false;
		}
	}

	return true;
}

const material &game::getMaterial(const char *name) {
	auto it = mat.find(name);
	if (it != mat.end()) {
		return it->second;
	} else {
		return material::MAT_DEFAULT;
	}
}