#ifndef _OPERATION_H
#define _OPERATION_H

#include "arrayobject.h"
#include "ethercat.h"


#define NSEC_PER_SEC 1000000000


void add_timespec(struct timespec *ts, int64 addtime);
void ec_sync(int64 reftime, int64 cycletime , int64 *offsettime);
void rt_csp(PyArrayObject_coordinates *points);


#endif /* _OPERATION_H */
