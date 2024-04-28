#pragma once
#include <utility>
#include <string>

struct arg_parser
{
	arg_parser(int argc, char* argv[]);

	std::string data_path;
	std::string config_filename;
	std::string raw_config_filename;

};