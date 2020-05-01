#pragma once

// Miscellaneous functions that don't belong anywhere else

#include "ICPPoint.h"
#include "Point.h"
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int MyRandRange(int x); // Range from [0, x-1], used with random_shuffle()

template <class T>
void reverseable_shuffle_forward(std::vector<T>& p, std::vector<unsigned int>& table)
{
    if (p.size() != table.size()) {
        throw std::runtime_error("reverseable_shuffle: size mismatch");
    }

    std::vector<T> tmp;

    for (unsigned int i = 0; i < table.size(); i++) {
        unsigned int pos = table[i];

        std::swap(p[i], p[pos]);
    }
}

template <class T>
void reverseable_shuffle_backward(std::vector<T>& p, std::vector<unsigned int>& table)
{
    if (p.size() != table.size()) {
        throw std::runtime_error("reverseable_shuffle: size mismatch");
    }

    std::vector<T> tmp;

    for (unsigned int i = 0; i < table.size(); i++) {
        unsigned int pos = table[table.size() - i - 1];

        std::swap(p[table.size() - 1 - i], p[pos]);
    }
}

