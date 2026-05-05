/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GProcess.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: RuhanGi <mohammedruhan.goltay@kaust.edu    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 17:02:06 by RuhanGi           #+#    #+#             */
/*   Updated: 2026/05/03 17:02:06 by RuhanGi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GProcess.hpp"

GProcess::GProcess(Kernel &k) : kernel(k)
{

}


double logAbsDet(Matrix m)
{
    if (m.empty()) return 0.0;
    size_t n = m.size();
    if (!checkRectangle(m, true))
        throw std::invalid_argument("Non-Square Matrix");

    double log_abs = 0.0;
    for (size_t j = 0; j < n; j++)
    {
        size_t pivot = j;
        for (size_t i = j + 1; i < n; i++)
            if (std::abs(m[i][j]) > std::abs(m[pivot][j])) pivot = i;
        std::swap(m[j], m[pivot]);                       // swap rows
        if (std::abs(m[j][j]) < 1e-15) return std::nan(""); // singular

        log_abs += std::log(std::abs(m[j][j]));          // accumulate log|pivot|

        double inv_pivot = 1.0 / m[j][j];                // normalise pivot row
        for (size_t k = 0; k < n; k++) m[j][k] *= inv_pivot;
        for (size_t i = 0; i < n; i++)
            if (i != j)
            {
                double factor = m[i][j];
                for (size_t k = 0; k < n; k++)
                    m[i][k] -= factor * m[j][k];
            }
    }
    return log_abs;
}


void GProcess::fit(Dataset &d)
{
    size_t N = d.X.size();
    size_t Nval = d.valX.size();
    Row rHistory;
    Row likeHistory;

    Matrix C(N, Row(N));
    Matrix k(Nval, Row(N));
    for (int epoch = 0; epoch < MAX_EPOCHS; epoch++)
    {
        kernel.print();
        for (size_t i = 0; i < N; i++)
            for (size_t j = 0; j < N; j++)
                C[i][j] = kernel.calc(d.X[i], d.X[j]) + (i == j) / kernel.beta;

        for (size_t i = 0; i < Nval; i++)
            for (size_t j = 0; j < N; j++)
                k[i][j] = kernel.calc(d.valX[i], d.X[j]);

        Matrix Cinv = invert(C);
        Matrix a = Cinv * d.Y;
        Matrix means = k * a;

        rHistory.push_back(rSqr(means, d.valY));
        kernel.update(d.X, Cinv, transpose(a)[0]);

        double term1 = -0.5 * dot(transpose(d.Y)[0], transpose(a)[0]);
        double term2 = -0.5 * logAbsDet(C);
        double term3 = -(N / 2.0) * std::log(2 * M_PI);
        double like = (term1 + term2 + term3) / N;
        likeHistory.push_back(like);

        // if (epoch > 0 && std::abs(likeHistory[epoch] - likeHistory[epoch-1]) < TOLERANCE)
        //     break;
        if (epoch > 0 && std::abs(rHistory[epoch] - rHistory[epoch-1]) < TOLERANCE)
            break;
        std::cout   << "\rEpoch: " << epoch 
                    << "\t| R^2: " << rHistory.back()
                    << "\t| log(P): " << likeHistory.back()
                    << "\t" << std::flush;
    }

    std::cout << "Initial R^2: " << rHistory.front() << "\n";
    std::cout << "Final R^2: " << rHistory.back() << "\n";
}

// void GProcess::fit(Dataset &d)
// {
//     size_t N = d.X.size();
//     Matrix C(N, Row(N));
//     for (size_t i = 0; i < N; i++)
//         for (size_t j = 0; j < N; j++)
//             C[i][j] = kernel.calc(d.X[i], d.X[j]) + (i == j) / kernel.beta;

//     size_t Nval = d.valX.size();
//     Matrix k(Nval, Row(N));
//     for (size_t i = 0; i < Nval; i++)
//         for (size_t j = 0; j < N; j++)
//             k[i][j] = kernel.calc(d.valX[i], d.X[j]);

//     Matrix Cinv = invert(C); // N x N
//     Matrix a = Cinv * d.Y; // N x 1

//     Matrix means = k * a; // Nval x 1
//     kernel.update(d.X, Cinv, a[0]);

//     Row vars(Nval);
//     for (size_t i = 0; i < Nval; i++)
//         vars[i] = kernel.calc(d.valX[i], d.valX[i]) - dot(k[i], Cinv * k[i]);


//     std::cout << rSqr(means, d.valY);
// }