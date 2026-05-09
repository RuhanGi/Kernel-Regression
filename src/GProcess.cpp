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


Matrix GProcess::getPredictiveState(Dataset &d, const Matrix &Cinv, const Row& a) {
    size_t Nval = d.valX.size();
    size_t Ntrain = Cinv.size();
    Matrix state(Nval, Row(3));

    for (size_t i = 0; i < Nval; i++) {
        Row k(Ntrain);
        for (size_t j = 0; j < Ntrain; j++)
            k[j] = kernel.calc(d.valX[i], d.X[j]);

        double mean = dot(k, a);
        double self_cov = kernel.calc(d.valX[i], d.valX[i]) + 1.0/kernel.beta;
        double var = self_cov - dot(k, Cinv * k);
        
        state[i][0] = d.valY[i][0];
        state[i][1] = mean;
        state[i][2] = std::sqrt(std::max(0.0, var));
    }
    return state;
}


void GProcess::writeToCSV(const std::string& filename, const Matrix& initial, const Matrix& optimized) {
    std::ofstream file(filename);
    file << "TrueValue,InitMean,InitStd,OptMean,OptStd\n";
    for (size_t i = 0; i < initial.size(); i++) {
        file << initial[i][0] << "," 
             << initial[i][1] << "," << initial[i][2] << ","
             << optimized[i][1] << "," << optimized[i][2] << "\n";
    }
    file.close();
}


void GProcess::fit(Dataset &d) {
    size_t N = d.X.size();
    Matrix initialMetrics, optimizedMetrics;
    double initialR = -1e300;
    double bestR = -1e300;
    int sinceBest = 0;

    for (int epoch = 0; epoch < MAX_EPOCHS; epoch++) {
        Matrix C(N, Row(N));
        for (size_t i = 0; i < N; i++)
            for (size_t j = 0; j < N; j++)
                C[i][j] = kernel.calc(d.X[i], d.X[j]) + (i == j) / kernel.beta;

        Matrix Cinv = invert(C);
        Matrix a = Cinv * d.Y;
        Row aVec = transpose(a)[0];
        Matrix state = getPredictiveState(d, Cinv, aVec);

        kernel.update(d.X, Cinv, aVec);
        double currentR = rSqr(d.valY, transpose({transpose(state)[1]}));
        if (epoch == 0)
        {
            initialR = currentR;
            initialMetrics = state;
            optimizedMetrics = state;
        }

        std::cout   << "\rEpoch: " << epoch 
            << "\t| R^2: " << currentR
            << "\t" << std::flush;
        if (currentR > bestR + TOLERANCE) {
            bestR = currentR;
            sinceBest = 0;
            optimizedMetrics = state;
        } else
            sinceBest++;

        if (sinceBest >= 15)
            break; 
    }

    writeToCSV("results.csv", initialMetrics, optimizedMetrics);
    std::cout << "\n" "Initial R^2: " << initialR << "\n";
    std::cout << "Best R^2: " << bestR << "\n";
}
