/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Data.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: RuhanGi <mohammedruhan.goltay@kaust.edu    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 11:26:21 by RuhanGi           #+#    #+#             */
/*   Updated: 2026/04/08 11:26:21 by RuhanGi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Types.hpp"

class Dataset {
private:
    void parse(std::ifstream& file);
    void split();

public:
    Matrix X;
    Matrix Y;
    Matrix valX;
    Matrix valY;
    std::vector<std::string> headers;
    std::string labelName;
    Row means;
    Row stds;

    Dataset(const std::string& file);
    void normalize();
    void printStats();
    void shuffle();
};
