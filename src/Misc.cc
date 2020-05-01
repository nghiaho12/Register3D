#include "Misc.h"
#include <tr1/random> // For better random number, Windows one sucks big time

tr1::mt19937 eng;

int MyRandRange(int x)
{
    tr1::uniform_int<int> unif(0, x - 1);

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
