#ifndef _OPERATION_H
#define _OPERATION_H

#include "arrayobject.h"
#include "ethercat.h"


#define NSEC_PER_SEC 1000000000

int rt_csp(PyArrayObject_coordinates *points, int32 cycletime);  /* cycletime in ns */

#endif /* _OPERATION_H */
