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
    
    if (type == KernelType::RBF)
        theta.push_back(1.0);
    else if (type == KernelType::COMPLEX)
    {
        theta.push_back(1.0);
        theta.push_back(1.0);
        theta.push_back(0.1);
        theta.push_back(0.1);
    }
    else if (type == KernelType::ARD)
        theta.push_back(1.0);

    beta = 1.0;
}


double calcDistance(const Row& xn, const Row& xm)
{
    double sum = 0.0;
    for (size_t i = 0; i < xn.size(); i++)
        sum += std::pow(xn[i]-xm[i], 2);
    return sum;
}


double Kernel::calcARD(const Row& xn, const Row& xm) {
    while (xn.size()+1 > theta.size())
        theta.push_back(1.0);

    double sum = 0.0;
    for (size_t i = 0; i < xn.size(); i++) {
        double l_sqr = std::pow(theta[i + 1], 2);
        sum += std::pow(xn[i] - xm[i], 2) / std::max(l_sqr, 1e-9);
    }
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
    else if (type == KernelType::ARD)
        return theta[0] * std::exp(-0.5 * calcARD(xn, xm));
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


void Kernel::deriveARD(const Matrix& X, const Matrix& Cinv, const Row& a)
{
    size_t N = X.size();
    size_t D = X[0].size();
    Matrix dCdT0(N, Row(N));
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            dCdT0[i][j] = std::exp(-0.5 * calcARD(X[i], X[j]));

    theta[0] += LR * (-0.5 * trace(Cinv * dCdT0) + 0.5 * dot(a, dCdT0 * a));
    for (size_t d = 0; d < D; d++) {
        Matrix dCdTd(N, Row(N));
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < N; j++) {
                double dist_d = std::pow(X[i][d] - X[j][d], 2);
                double k_val = calc(X[i], X[j]);
                dCdTd[i][j] = k_val * (dist_d / std::pow(theta[d + 1], 3));
            }
        }
        theta[d + 1] += LR * (-0.5 * trace(Cinv * dCdTd) + 0.5 * dot(a, dCdTd * a));
    }

    for (double &t : theta)
        t = std::min(std::max(t, TOLERANCE), 1e4);
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
    else if (type == KernelType::ARD)
        deriveARD(X, Cinv, a);
}

void Kernel::print()
{
    std::cout << "Hyperparameters = [Beta=" << beta;
    for (double t : theta)
        std::cout << ", " << t;
    std::cout << "]" << std::flush;
}