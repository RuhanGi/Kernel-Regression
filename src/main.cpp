/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: RuhanGi <mohammedruhan.goltay@kaust.edu    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 18:35:50 by RuhanGi           #+#    #+#             */
/*   Updated: 2026/01/28 18:35:50 by RuhanGi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GProcess.hpp"

int	main(int argc, char *argv[])
{
	if (argc != 2 || !argv)
		return (std::cerr << RED "Usage: kernel.exe {data}" RESET "\n", 1);

	try
	{
		Dataset data = Dataset(argv[1]);
		data.printStats();

		Kernel k = Kernel(KernelType::COMPLEX);
		// Kernel k = Kernel(KernelType::RBF);
		// Kernel k = Kernel(KernelType::ARD);
		GProcess g = GProcess(k);
		g.fit(data);
	}
	catch (std::exception & e)
	{
		std::cerr << RED "ERROR: " << e.what() << RESET "\n";
		return (1);
	}

	return (0);
}
