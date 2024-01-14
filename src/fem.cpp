#include "fem.h"

#include "../include/Spectra/SymGEigsShiftSolver.h"
#include "../include/Spectra/MatOp/SymShiftInvert.h"
#include "../include/Spectra/MatOp/SparseSymMatProd.h"

#include <iostream>

/*
* void fem::solve_eigenproblem(Eigen::SparseMatrix<double> S, Eigen::SparseMatrix<double> T)
{
	using OpType = Spectra::SparseSymMatProd<double>;
	using BOpType = Spectra::SparseSymMatProd<double>;
	OpType op(S);
	BOpType Bop(T);

	Spectra::SymGEigsShiftSolver<OpType, BOpType, Spectra::GEigsMode::ShiftInvert> geigs(op, Bop, 3, 6, 0.0);

	geigs.init();
	int nconv = geigs.compute(Spectra::SortRule::LargestMagn);

    // Retrieve results
    Eigen::VectorXd evalues;
    Eigen::MatrixXd evecs;
    if (geigs.info() == Spectra::CompInfo::Successful)
    {
        evalues = geigs.eigenvalues();
        evecs = geigs.eigenvectors();
    }

    std::cout << "Number of converged generalized eigenvalues: " << nconv << std::endl;
    std::cout << "Generalized eigenvalues found:\n" << evalues << std::endl;
    std::cout << "Generalized eigenvectors found:\n" << evecs.topRows(10) << std::endl;

}
*/
