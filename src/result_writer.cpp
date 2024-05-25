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

void result_writer::write_structured_2d_field(std::string filename, structured_2d_field_data data)
{
	auto grid = data.grid;
	auto fields = data.field;

	std::ofstream ofs(filename + ".2d");

	ofs << std::format("{} {}\n", grid.start_point[0], grid.start_point[1]);
	ofs << std::format("{} {}\n", grid.step_sizes[0], grid.step_sizes[1]);
	ofs << std::format("{} {}\n", grid.num_steps[0], grid.num_steps[1]);

	for (auto row : fields.rowwise())
	{
		ofs << std::format("{} {}\n", complex_number(row(0)), complex_number(row(1)));
	}
	ofs.close();
}

void result_writer::write_structured_3d_field(std::string filename, structured_3d_field_data data)
{
	auto grid = data.grid;
	auto fields = data.field;

	std::ofstream ofs(filename + ".3d");

	ofs << std::format("{} {} {}\n", grid.start_point[0], grid.start_point[1], grid.start_point[2]);
	ofs << std::format("{} {} {}\n", grid.step_sizes[0], grid.step_sizes[1], grid.step_sizes[2]);
	ofs << std::format("{} {} {}\n", grid.num_steps[0], grid.num_steps[1], grid.num_steps[2]);

	for (auto row : fields.rowwise())
	{
		ofs << std::format("{} {} {}\n", complex_number(row(0)), complex_number(row(1)), complex_number(row(2)));
	}
	ofs.close();
}

void result_writer::write_unstructured_3d_field(std::string filename, unstructured_3d_field_data data)
{
	auto points = data.points;
	auto fields = data.field;

	std::ofstream ofs(filename + ".3du");
	for (size_t i = 0; i < points.rows(); i++)
	{
		ofs << std::format("{} {} {} {} {} {}\n", 
			points(i, 0), points(i, 1), points(i, 2),
			complex_number(fields(i, 0)), complex_number(fields(i, 1)), complex_number(fields(i, 2))
		);
	}
	ofs.close();
}
