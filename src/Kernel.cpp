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
    thetas.push_back(1.0);
    if (type == KernelType::COMPLEX)
    {
        thetas.push_back(1.0);
        thetas.push_back(0.1);
        thetas.push_back(0.1);
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
        return std::exp(dist / -2 / thetas[0]);
    else if (type == KernelType::COMPLEX)
        return (thetas[0] * std::exp(thetas[1] * dist / -2)
         + thetas[2] + thetas[3] * dot(xn, xm));
    return 0;
}


// double Kernel::getDerivative(const Row& xn, const Row& xm, int index)
// {
    
// }
