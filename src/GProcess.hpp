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
        Kernel &kernel;

    public:
        GProcess(Kernel &k);
    
        Matrix getPredictiveState(Dataset &d, const Matrix &Cinv, const Row &a);
        Matrix formatPlotData(Dataset &d, const Matrix& state);
        GPResults fit(Dataset &d);
        void writeToCSV(const std::string& filename, const Matrix& initial, const Matrix& optimized);
};
