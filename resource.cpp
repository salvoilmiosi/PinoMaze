#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

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

	if (ifs.fail()) {
		cerr << "Could not open input file \"" << filename << "\"\n";
		return false;
	}

	string line;
	fileData file;

	int line_num = 0;
	bool escape = false;

	while (getline(ifs, line)) {
		trim(line);

		if (line.empty()) continue;
		if (line.at(0) == '#') continue;

		int state = 0;
		// 0 Reading ID
		// 1 Space between ID and type
		// 2 Reading type
		// 3 Space betwwen type and filename
		// 4 Reading filename
		// 5 Over filename

		memset(file.res_id, 0, sizeof(file.res_id));
		memset(file.filename, 0, sizeof(file.filename));

		char *i = file.res_id;

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
					++state;
				}
				// fall through
			case 2:
				if (isspace(c)) {
					++state;
				} else {
					break;
				}
				// fall through
			case 3:
				if (isspace(c)) {
					break;
				} else {
					i = file.filename;
					++state;
					if (c == '\"') {
						break;
					}
				}
				// fall through
			case 4:
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
			case 5:
				file.filename[0] = '\0';
				break;
			}
		}

		if (file.res_id[0] == '\0' || file.filename[0] == '\0') {
			cerr << "Syntax error at line #" << line_num << ":\n" << line << "\n";
			return false;
		}

		files.push_back(file);
		++line_num;
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

int main(int argc, char **argv) {
	if (argc < 3) {
		cout << "Usage: resource [input] [output]\n";
		return -1;
	}

	const char *input = argv[1];
	const char *output = argv[2];

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