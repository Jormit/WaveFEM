#include <iostream>

#include <map>
#include <Eigen/Eigenvalues> 

#include <Spectra/SymGEigsShiftSolver.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>

#include "fem.h"
#include "constants.h"

std::pair<Eigen::VectorXd, Eigen::MatrixXd> fem::solve_eigenproblem(const Eigen::MatrixXd& S, const Eigen::MatrixXd& T, double min)
{
	Eigen::GeneralizedEigenSolver<Eigen::MatrixXd> ges;
	ges.compute(S, T);
	
	Eigen::MatrixXd eVecs = ges.eigenvectors().real();
	Eigen::VectorXd eVals = ges.eigenvalues().real();

	std::map<double, Eigen::VectorXd> eigen_map;
	int i = 0;
	for (auto x : eVals)
	{
		if (x > min)
		{
			eigen_map[x] = eVecs.col(i);
		}
		i++;
	}

	Eigen::MatrixXd eVecs_sorted(eVecs.rows(), eigen_map.size());
	Eigen::VectorXd eVals_sorted(eigen_map.size());

	i = 0;
	for (auto const& [key, val] : eigen_map)
	{
		auto reversed_index = eigen_map.size() - i - 1;
		eVecs_sorted.col(reversed_index) = val;
		eVals_sorted(reversed_index) = key;
		i++;
	}

	return { eVals_sorted, eVecs_sorted };
}

std::pair<Eigen::VectorXd, Eigen::MatrixXd> fem::solve_eigenproblem_iram(const Eigen::SparseMatrix<double>& S, const Eigen::SparseMatrix<double>& T, double guess, int num)
{
	using OpType = Spectra::SymShiftInvert<double, Eigen::Sparse, Eigen::Sparse>;
	using BOpType = Spectra::SparseSymMatProd<double>;
	OpType op(S, T);
	BOpType Bop(T);

	Spectra::SymGEigsShiftSolver<OpType, BOpType, Spectra::GEigsMode::ShiftInvert> geigs(op, Bop, num, num * 2 + 1, guess);

	geigs.init();
	auto nconv = geigs.compute(Spectra::SortRule::LargestAlge);

	Eigen::VectorXd evalues;
	Eigen::MatrixXd evecs;
	if (geigs.info() == Spectra::CompInfo::Successful)
	{
		evalues = geigs.eigenvalues();
		evecs = geigs.eigenvectors();
	}

	return { evalues.reverse(), evecs.rowwise().reverse() };
}

