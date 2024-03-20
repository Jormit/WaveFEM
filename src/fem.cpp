#include <iostream>
#include <Spectra/SymGEigsShiftSolver.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>

#include "fem.h"
#include "constants.h"

std::pair<Eigen::VectorXd, Eigen::MatrixXd> fem::solve_eigenproblem(const Eigen::SparseMatrix<double>& S, const Eigen::SparseMatrix<double>& T, double guess, int num)
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

