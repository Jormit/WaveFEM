#include "fem.h"
#include <iostream>

#include <Spectra/SymGEigsShiftSolver.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>

void fem::solve_eigenproblem(Eigen::SparseMatrix<double> S, Eigen::SparseMatrix<double> T)
{
    using OpType = Spectra::SymShiftInvert<double, Eigen::Sparse, Eigen::Sparse>;
    using BOpType = Spectra::SparseSymMatProd<double>;
    OpType op(S, T);
    BOpType Bop(T);

    Spectra::SymGEigsShiftSolver<OpType, BOpType, Spectra::GEigsMode::ShiftInvert> geigs(op, Bop, 3, 6, 3.0);

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

/*
#include <Eigen/Eigenvalues>

void fem::solve_eigenproblem(Eigen::MatrixXd S, Eigen::MatrixXd T)
{
    Eigen::GeneralizedEigenSolver<Eigen::MatrixXd> ges;
    ges.compute(S, T);
    std::cout << "The (complex) numerators of the generalzied eigenvalues are: " << ges.alphas().transpose() << std::endl;
    std::cout << "The (real) denominatore of the generalzied eigenvalues are: " << ges.betas().transpose() << std::endl;
    std::cout << "The (complex) generalzied eigenvalues are (alphas./beta): " << ges.eigenvalues().transpose() << std::endl;
}
*/

