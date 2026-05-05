/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kernel.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: RuhanGi <mohammedruhan.goltay@kaust.edu    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 14:59:16 by RuhanGi           #+#    #+#             */
/*   Updated: 2026/05/03 14:59:16 by RuhanGi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Kernel.hpp"


Kernel::Kernel(const KernelType k) : type(k)
{
    theta.push_back(1.0);
    if (type == KernelType::COMPLEX)
    {
        theta.push_back(1.0);
        theta.push_back(0.1);
        theta.push_back(0.1);
    }

    beta = 1.0;
}


double calcDistance(const Row& xn, const Row& xm)
{
    double sum = 0.0;
    for (size_t i = 0; i < xn.size(); i++)
        sum += std::pow(xn[i]-xm[i], 2);
    return sum;
}


double Kernel::calc(const Row& xn, const Row& xm)
{
    double dist = calcDistance(xn, xm);
    if (type == KernelType::RBF)
        return std::exp(dist / -2 / theta[0]);
    else if (type == KernelType::COMPLEX)
        return (theta[0] * std::exp(theta[1] * dist / -2)
         + theta[2] + theta[3] * dot(xn, xm));
    return 0;
}


void Kernel::deriveComplex(const Matrix& X, const Matrix& Cinv, const Row& a)
{
    size_t N = X.size();

    Matrix dCd0(N, Row(N));
    Matrix dCd1(N, Row(N));
    Matrix dCd2(N, Row(N, 1.0));
    Matrix dCd3(N, Row(N));
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
        {
            double dist = calcDistance(X[i], X[j]);
            dCd0[i][j] = std::exp(theta[1] * dist / -2);
            dCd1[i][j] = theta[0] * dCd0[i][j] * dist / -2;
            dCd3[i][j] = dot(X[i], X[j]);
        }
    theta[0] += LR * (-0.5 * trace(Cinv * dCd0) + 0.5 * dot(a, dCd0 * a));
    theta[1] += LR * (-0.5 * trace(Cinv * dCd1) + 0.5 * dot(a, dCd1 * a));
    theta[2] += LR * (-0.5 * trace(Cinv * dCd2) + 0.5 * dot(a, dCd2 * a));
    theta[3] += LR * (-0.5 * trace(Cinv * dCd3) + 0.5 * dot(a, dCd3 * a));

    for (size_t i = 0; i < 4; i++)
        theta[i] = std::min(std::max(theta[i], TOLERANCE), 1e4);
}


void Kernel::update(const Matrix& X, const Matrix& Cinv, const Row& a)
{
    size_t N = X.size();
    double aSqr = dot(a, a);

    beta += LR * (0.5 * trace(Cinv)/(beta*beta) - 0.5 * aSqr/(beta*beta));
    beta = std::min(std::max(beta, TOLERANCE), 1e4);
    if (type == KernelType::RBF)
    {
        Matrix dCdT(N, Row(N));
        for (size_t i = 0; i < N; i++)
            for (size_t j = 0; j < N; j++)
            {
                double dist = calcDistance(X[i], X[j]);
                double k = std::exp(dist / (-2 * theta[0]));
                dCdT[i][j] = k * (dist / (2 * theta[0] * theta[0]));
            }
        theta[0] += LR * (-0.5 * trace(Cinv * dCdT) + 0.5 * dot(a, dCdT * a));
        theta[0] = std::max(theta[0], TOLERANCE);
    }
    else if (type == KernelType::COMPLEX)
        deriveComplex(X, Cinv, a);
}

void Kernel::print()
{
    std::cout << "Beta = " << beta;
    for (double t : theta)
        std::cout << " " << t;
    std::cout << "\n";
}