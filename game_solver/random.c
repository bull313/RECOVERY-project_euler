#include "random.h"

uint32 gen_random_number(uint32 range) {
    double scaled_rand;
    double range_scaled_rand;
    uint32 random_number;
    uint32 result;

    srand(time(NULL));
    random_number = rand();
    scaled_rand = random_number / (double) RAND_MAX;
    range_scaled_rand = scaled_rand * range;
    result = (uint32) range_scaled_rand;

    return result;
}
