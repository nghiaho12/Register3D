#include "Misc.h"
#include <random> // For better random number, Windows one sucks big time

std::mt19937 eng;

int MyRandRange(int x)
{
    std::uniform_int_distribution<> unif(0, x - 1);

    return unif(eng);
}

/*
// For Windows compiler not supporting tr1/random
int MyRandRange(int x)
{
        const int m = (RAND_MAX<<16) + RAND_MAX;
        int r = (rand()<<16) + rand();

    return (int)(x*(double)r/m);
}
*/
