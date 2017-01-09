#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std;

static const int ID_MAXSIZE = 32;

struct resource {
	size_t size;
	size_t ptr;
	char res_id[ID_MAXSIZE];
};

vector<resource> resFiles;
ifstream resourceIfs;

void removeExtension(char *filename) {
	char *c = filename + strlen(filename) - 1;
	for (; c > filename; --c) {
		if (*c == '.') {
			*c = '\0';
			break;
		}
		*c = '\0';
	}
}

unsigned int readInt(ifstream &ifs) {
	char data[4];
	ifs.read(data, 4);
	unsigned int num =
		(data[0] << 24 & 0xff000000) |
		(data[1] << 16 & 0x00ff0000) |
		(data[2] << 8 & 0x0000ff00) |
		(data[3] & 0x000000ff);
	return num;
}

bool openResourceFile(const char *filename) {
	resourceIfs.open(filename, ios::binary);

	if (resourceIfs.fail()) {
		return false;
	}

	if (readInt(resourceIfs) != 0x255435f4) {
		return false;
	}

	resFiles.resize(readInt(resourceIfs));

	for (resource &res : resFiles) {
		resourceIfs.read(res.res_id, ID_MAXSIZE);
		res.size = readInt(resourceIfs);
		res.ptr = readInt(resourceIfs);
	}

	return true;
}

bool saveFiles(const char *output_dir) {
	for (resource &res : resFiles) {
		char output[FILENAME_MAX];
		strncpy(output, output_dir, FILENAME_MAX);
		strncat(output, res.res_id, FILENAME_MAX);
		
		vector<char> data(res.size);
		
		resourceIfs.seekg(res.ptr);
		resourceIfs.read(data.data(), res.size);
		
		ofstream ofs(output, ios::binary);
		ofs.write(data.data(), data.size());
		
		if (ofs.fail()) {
			cerr << "Could not save file \"" << output << "\"\n";
			return false;
		}
	}
	
	return true;
}

void create_dir(const char *dir_name) {
#ifdef _WIN32
	CreateDirectory(dir_name, nullptr);
#else
	struct stat st = {0};
	if (stat(dir_name, &st) == -1) {
		mkdir(dir_name, 0700);
	}
#endif
}
int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " input [output_dir]\n";
		return -1;
	}
	
	const char *input = argv[1];
	char output[FILENAME_MAX];
	memset(output, 0, sizeof(output));
	
	if (argc >= 3) {
		strncpy(output, argv[2], FILENAME_MAX);
	} else {
		strncpy(output, input, FILENAME_MAX);
		removeExtension(output);
	}
	
	char lastChar = output[strlen(output)-1];
	if (lastChar != '\\' && lastChar != '/') {
#ifdef _WIN32
		output[strlen(output)] = '\\';
#else
		output[strlen(output)] = '/';
#endif
	}
	
	if (output[0] == '\\' || output[0] == '/') {
		memset(output, 0, sizeof(output));
	} else {
		create_dir(output);
	}
	
	if (!openResourceFile(input)) {
		cerr << "Could not open resource file \"" << input << "\"\n";
		return -2;
	}
	
	if (!saveFiles(output)) {
		cerr << "Error. Terminating.\n";
		return -3;
	}
	
	cout << "Operation completed succesfully.\n";
	return 0;
}

