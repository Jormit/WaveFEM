#include <format>
#include <fstream>

#include "result_writer.h"

std::string result_writer::complex_number(std::complex<double> num)
{
	if (num.imag() >= 0)
	{
		return std::format("{}+{}j", num.real(), num.imag());
	}
	return std::format("{}{}j", num.real(), num.imag());
}

void result_writer::write_2d_field(std::string filename, Eigen::MatrixX2d points, Eigen::MatrixX2cd fields)
{
	std::ofstream ofs(filename);
	for (int i = 0; i < points.rows(); i++)
	{
		ofs << std::format("{} {} {} {}\n", points.row(i)(0), points.row(i)(1),
			complex_number(fields.row(i)(0)), complex_number(fields.row(i)(1)));
	}
	ofs.close();
}

void result_writer::write_3d_field(std::string filename, Eigen::MatrixX3d points, Eigen::MatrixX3cd fields)
{
	std::ofstream ofs(filename);
	for (int i = 0; i < points.rows(); i++)
	{
		ofs << std::format("{} {} {} {} {} {}\n", points.row(i)(0), points.row(i)(1), points.row(i)(2),
			complex_number(fields.row(i)(0)), complex_number(fields.row(i)(1)), complex_number(fields.row(i)(2)));
	}
	ofs.close();
}
