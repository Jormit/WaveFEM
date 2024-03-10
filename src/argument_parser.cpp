#include <iostream>
#include <algorithm>

#include "argument_parser.h"

arg_parser::arg_parser(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Not config file provided" << std::endl;
	}

	std::string filename(argv[1]);
	filename.erase(std::remove(filename.begin(), filename.end(), '\"'), filename.end());

	config_filename = filename.substr(filename.find_last_of("/\\") + 1);
	data_path = filename.substr(0, filename.find_last_of("/\\") + 1);
}
