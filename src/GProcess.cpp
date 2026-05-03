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

GProcess::GProcess(Kernel &k) : k(k)
{

}

void GProcess::fit(Dataset &d)
{
    size_t N = d.X.size();
    Matrix K(N, Row(N));
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            K[i][j] = k.calc(d.X[i], d.X[j]) + (i == j) / k.beta;

}