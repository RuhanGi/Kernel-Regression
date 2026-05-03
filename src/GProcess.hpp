/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GProcess.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: RuhanGi <mohammedruhan.goltay@kaust.edu    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 17:02:03 by RuhanGi           #+#    #+#             */
/*   Updated: 2026/05/03 17:02:03 by RuhanGi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Types.hpp"

class GProcess {
    private:
        Kernel &k;

    public:
        GProcess(Kernel &k);
        void fit(Dataset &d);
};
