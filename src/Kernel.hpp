/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kernel.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: RuhanGi <mohammedruhan.goltay@kaust.edu    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 14:59:13 by RuhanGi           #+#    #+#             */
/*   Updated: 2026/05/03 14:59:13 by RuhanGi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Types.hpp"

class Kernel {
    private:
        Row thetas;

    public:
        const KernelType type;
        double beta;

        Kernel(KernelType k);
        double calc(const Row& xn, const Row& xm);
        double getDerivative(const Row& xn, const Row& xm, int index);
};
