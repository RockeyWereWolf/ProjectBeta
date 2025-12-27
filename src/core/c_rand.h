#ifndef C_RAND_H
#define C_RAND_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  uint64_t seed;
} c_rand_t;
void cr_set_seed(c_rand_t *r, int64_t s);
double cr_next_double(c_rand_t *r);
#ifdef __cplusplus
}
#endif
#endif