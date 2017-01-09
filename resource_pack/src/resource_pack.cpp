#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cstring>

using namespace std;

const size_t ID_MAXSIZE = 32;

struct fileData {
	char res_id[ID_MAXSIZE];
	char filename[FILENAME_MAX];
	vector<char> data;
};

void trim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
}

bool parseResource(const char *filename, vector<fileData> &files) {
	ifstream ifs(filename);

	string resource_file_dir = filename;
	size_t slash_pos = resource_file_dir.find_last_of("/\\");
	resource_file_dir = resource_file_dir.substr(0, slash_pos);

	if (!resource_file_dir.empty()) {
		resource_file_dir.append("/");
	}

	if (ifs.fail()) {
		cerr << "Could not open input file \"" << filename << "\"\n";
		return false;
	}

	string line;
	fileData file;

	int line_num = 0;
	bool escape;

	while (getline(ifs, line)) {
		trim(line);
		
		++line_num;

		if (line.empty()) continue;
		if (line.at(0) == '#') continue;

		int state = 0;
		// 0 Reading ID
		// 1 Space between ID and filename
		// 2 Reading filename
		// 3 Over filename

		memset(file.res_id, 0, sizeof(file.res_id));
		memset(file.filename, 0, sizeof(file.filename));

		char *i = file.res_id;
		
		escape = false;

		for (char c : line) {
			switch (state) {
			case 0:
				if (isspace(c)) {
					++state;
				} else {
					*i++ = c;
					break;
				}
				// fall through
			case 1:
				if (isspace(c)) {
					break;
				} else {
					i = file.filename;

					if (!resource_file_dir.empty()) {
						strncpy(i, resource_file_dir.c_str(), FILENAME_MAX);
						i += resource_file_dir.size();
					}

					++state;
					if (c == '\"') {
						break;
					}
				}
				// fall through
			case 2:
				if (!escape) {
					if (c == '\\') {
						escape = true;
						break;
					} else if (c == '\"') {
						++state;
						break;
					}
				} else {
					escape = false;
				}
				*i++ = c;
				break;
			case 3:
				file.filename[0] = '\0';
				break;
			}
		}

		if (file.res_id[0] == '\0' || file.filename[0] == '\0') {
			cerr << "Syntax error at line #" << line_num << ":\n" << line << "\n";
			return false;
		}

		files.push_back(file);
	}

	return true;
}

bool openResources(vector<fileData> &files) {
	for (fileData &file : files) {
		ifstream ifs(file.filename, ios::binary | ios::ate);
		if (ifs.fail()) {
			cerr << "Could not open file \"" << file.filename << "\"\n";
			return false;
		}

		size_t size = (size_t)ifs.tellg();
		file.data.resize(size);
		ifs.seekg(ios::beg);

		if (!ifs.read(file.data.data(), size)) {
			cerr << "Could not open file \"" << file.filename << "\"\n";
			return false;
		}
	}

	return true;
}

void writeInt(ofstream &ofs, const int num) {
	char str[4];
	str[0] = (num & 0xff000000) >> (8 * 3);
	str[1] = (num & 0x00ff0000) >> (8 * 2);
	str[2] = (num & 0x0000ff00) >> (8 * 1);
	str[3] = (num & 0x000000ff) >> (8 * 0);

	ofs.write(str, 4);
}

bool saveResources(const char *filename, vector<fileData> &files) {
	ofstream ofs(filename, ios::binary);

	if (ofs.fail()) return false;

	const int tableSize = ID_MAXSIZE + sizeof(int) * 2;
	int ptr = tableSize * files.size() + sizeof(int) * 2;

	writeInt(ofs, 0x255435f4);

	writeInt(ofs, files.size());

	for (fileData &file : files) {
		ofs.write(file.res_id, ID_MAXSIZE);
		writeInt(ofs, file.data.size());
		writeInt(ofs, ptr);

		if (ofs.fail()) return false;

		ptr += file.data.size();
	}

	for (fileData &file : files) {
		ofs.write(file.data.data(), file.data.size());

		if (ofs.fail()) return false;
	}

	return true;
}

void changeExtension(char *filename, const char *ext) {
	char *c = filename + strlen(filename) - 1;
	for (; c > filename; --c) {
		if (*c == '.') {
			break;
		} else {
			*c = '\0';
		}
	}
	
	for (const char *e = ext; *e != '\0'; ++e) {
		++c;
		*c = *e;
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " input [output]\n";
		return -1;
	}

	const char *input = argv[1];
	char output[FILENAME_MAX];

	if (argc >= 3) {
		strcpy(output, argv[2]);
	} else {
		strcpy(output, input);
		changeExtension(output, "dat");
	}

	vector<fileData> files;

	if (!parseResource(input, files)) {
		cerr << "Could not parse input file\n";
		return -2;
	}

	if (!openResources(files)) {
		cerr << "Could not open files\n";
		return -3;
	}

	if (!saveResources(output, files)) {
		cerr << "Could not save resource file\n";
		return -4;
	}
	
	cout << "Completed operation. Saved resource file at \"" << output << "\"\n";
	return 0;
}