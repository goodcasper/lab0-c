#ifndef DUDECT_FIXTURE_H
#define DUDECT_FIXTURE_H
/* As a marker,it is used to inform the program that this header has been
 * processed,to prevent multiple inclusions*/
#include <stdbool.h>
#include "constant.h"

/* Interface to test if function is constant */

#define _(x) bool is_##x##_const(void);
/* In constant.h,DUT_FUNCS is defined,which declares 4 functions here
   such as bool is_remove_head_const(void); */
DUT_FUNCS
#undef _

#endif
