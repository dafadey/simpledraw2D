#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		return -1;
	}
	std::string prefix;
	if (argc > 2)
		prefix = argv[2];

	std::string name = argv[1];
	std::ifstream inf(name.c_str(), std::ios_base::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(inf), {});
	inf.close();

	std::string varname;
	for (int i = 0; i < name.size(); i++) {
		char c = name.c_str()[i];
		if (c == '.')
			c = '_';
		varname += c;
		if (c == '\\' || c == '/')
			varname.clear();
	}
	varname = prefix + varname;
	std::ios_base::fmtflags f(std::cout.flags());
	std::cout << "const char " << varname << "[] = {";
	for (int i = 0; i < buffer.size(); i++) {
		if (i % 16 == 0)
			std::cout << '\n';
		std::cout << " 0x" << std::setw(2) << std::setfill('0') << std::hex << (int) buffer[i] << ',';
    }
	std::cout << "\n};\n";
	std::cout.flags(f);
	std::cout << "const unsigned int " << varname << "_len = " << buffer.size() << ";\n";
	return 0;
}