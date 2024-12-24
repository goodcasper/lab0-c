#ifndef DUDECT_CONSTANT_H
#define DUDECT_CONSTANT_H

#include <stdbool.h>
#include <stdint.h>

/* Number of measurements per test */
#define N_MEASURES 150

/* Allow random number range from 0 to 65535 */
#define CHUNK_SIZE 2

#define DROP_SIZE 20

#define DUT_FUNCS  \
    _(insert_head) \
    _(insert_tail) \
    _(remove_head) \
    _(remove_tail)

#define DUT(x) DUT_##x

enum {
#define _(x) DUT(x),
    DUT_FUNCS
#undef _
};
/*  1.Using DUT_FUNCS list,along with the macro _and DUT(X),
    to automatically generate enum member.
    2.Finally,the generated enum includes identifiers starting with DUT_
    ,used to represent various function operations,such as DUT_insert_head

*/
void init_dut();
void prepare_inputs(uint8_t *input_data, uint8_t *classes);
bool measure(int64_t *before_ticks,
             int64_t *after_ticks,
             uint8_t *input_data,
             int mode);

#endif
