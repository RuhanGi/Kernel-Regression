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


double MSE(const Matrix &preds, const Matrix &actual)
{
    double totalLoss = 0.0;
    size_t n = preds.size();
    size_t k = preds[0].size();

    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < k; j++) {
            double error = actual[i][j] - preds[i][j];
            totalLoss += error * error;
        }
    return totalLoss / (n * k);
}


Matrix GProcess::formatPlotData(Dataset &d, const Matrix& state) {
    Matrix out;
    for(size_t i = 0; i < d.valX.size(); i++) {
        Row r;
        r.push_back(d.valX[i][0]); // X1
        r.push_back(d.valX[i].size() > 1 ? d.valX[i][1] : 0.0); // X2 or 0
        r.push_back(state[i][0]); // True Y
        r.push_back(state[i][1]); // Mean
        r.push_back(state[i][2]); // Std
        out.push_back(r);
    }
    return out;
}


GPResults GProcess::fit(Dataset &d) {
    GPResults res;
    res.k = kernel.type;

    size_t N = d.X.size();
    Matrix initialMetrics, optimizedMetrics;
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
        Matrix preds = transpose({transpose(state)[1]});
        double currentR2 = rSqr(d.valY, preds);
        double currentMSE = MSE(d.valY, preds);
        res.history.push_back({currentR2, currentMSE});
        if (epoch == 0)
            res.initialPredictions = formatPlotData(d, state);
        
        kernel.update(d.X, Cinv, aVec);

        std::cout   << YELLOW "\rEpoch: " << epoch 
            << RED "\t| MSE: " << currentMSE
            << GREEN "\t| R^2: " << currentR2
            << "\t" RESET << std::flush;
        if (currentR2 > bestR + TOLERANCE) {
            bestR = currentR2;
            sinceBest = 0;
            res.finalPredictions = formatPlotData(d, state);
        } else
            sinceBest++;

        if (sinceBest >= 15)
            break; 
    }

    std::cout << GREY "\n" "Initial R^2: " << res.history.front().r2 << "\n";
    std::cout << "Best R^2: " << bestR << "\n" RESET;
    return res;
}
