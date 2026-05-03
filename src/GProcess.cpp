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


void GProcess::fit(Dataset &d)
{
    size_t N = d.X.size();

    Matrix C(N, Row(N));
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            C[i][j] = kernel.calc(d.X[i], d.X[j]) + (i == j) / kernel.beta;

    size_t Nval = d.valX.size();
    Matrix k(Nval, Row(N));
    for (size_t i = 0; i < Nval; i++)
        for (size_t j = 0; j < N; j++)
            k[i][j] = kernel.calc(d.valX[i], d.X[j]);

    Matrix Cinv = invert(C); // N x N
    Matrix a = Cinv * d.Y; // N x 1

    Matrix means = k * a; // Nval x 1
    Row vars(Nval);
    for (size_t i = 0; i < Nval; i++)
        vars[i] = kernel.calc(d.valX[i], d.valX[i]);//TODO - dot(k[i], Cinv * k[i]);

    std::cout << rSqr(means, d.valY);

    // TODO STart learning/updating weights
    // TODO improve efficiency
    // TODO experiment and report
}
