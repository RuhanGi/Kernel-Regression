/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: RuhanGi <mohammedruhan.goltay@kaust.edu    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 18:39:12 by RuhanGi           #+#    #+#             */
/*   Updated: 2026/04/10 18:39:12 by RuhanGi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GProcess.hpp"

void runAssessment(Dataset &data) {
    std::vector<KernelType> kernels = {KernelType::RBF, KernelType::COMPLEX, KernelType::ARD};
    std::ofstream master("results.csv");

    master << "Kernel,DataType,Epoch,R2,MSE,X1,X2,TrueY,Mean,Std\n"; 
    for (auto type : kernels) {
        Kernel k(type);
        GProcess gp(k);
        GPResults res = gp.fit(data);
        
        std::string kName = (type == KernelType::RBF) ? "RBF" : 
                            (type == KernelType::COMPLEX) ? "Complex" : "ARD";

        for (size_t e = 0; e < res.history.size(); e++)
            master << kName << ",History," << e << "," 
                   << res.history[e].r2 << "," << res.history[e].mse 
                   << ",0,0,0,0,0\n";

        for (const auto& row : res.finalPredictions)
            master << kName << ",Spatial,-1,0,0," 
                   << row[0] << "," << row[1] << ","
                   << row[2] << "," << row[3] << ","
                   << row[4] << "\n";
        
        std::cout << "Finished assessment for Kernel: " << kName << "\n";
    }
    master.close();
}