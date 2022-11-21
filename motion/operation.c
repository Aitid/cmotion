#include <stdio.h>

#include "operation.h"
#include "ethercat.h"
#include "arrayobject.h"
#include "wrap_master.h"


int64 toff, gl_delta;


/* add ns to timespec */
void add_timespec(struct timespec *ts, int64 addtime) {
   int64 sec, nsec;

   nsec = addtime % NSEC_PER_SEC;
   sec = (addtime - nsec) / NSEC_PER_SEC;
   ts->tv_sec += sec;
   ts->tv_nsec += nsec;
   if ( ts->tv_nsec >= NSEC_PER_SEC )
   {
      nsec = ts->tv_nsec % NSEC_PER_SEC;
      ts->tv_sec += (ts->tv_nsec - nsec) / NSEC_PER_SEC;
      ts->tv_nsec = nsec;
   }
}


/* PI calculation to get linux time synced to DC time */
void ec_sync(int64 reftime, int64 cycletime , int64 *offsettime) {
   static int64 integral = 0;
   int64 delta;
   /* set linux sync point 50us later than DC sync, just as example */
   delta = (reftime - 50000) % cycletime;
   if(delta> (cycletime / 2)) { delta= delta - cycletime; }
   if(delta>0){ integral++; }
   if(delta<0){ integral--; }
   /*
    * offsettime = -(delta * A) - (integral * B)
    * где A и B коэфициенты, которые зависят от железа 
    */
   *offsettime = -(delta / 700) - (integral / 20);
   printf("%ld, %ld", delta, *offsettime); 
   gl_delta = delta;
}


void rt_csp(PyArrayObject_coordinates *points) {

    outPDO *out;
    struct timespec ts, tleft;
    toff = 0;
    int64 cycletime;

    cycletime = 22000000; /* cycletime in ns */

    clock_gettime(CLOCK_MONOTONIC, &ts);
    for (int i = 0; i < points->shape[1]; i++){

        /* calculate next cycle start */
        add_timespec(&ts, cycletime + toff);
        /* wait to cycle start */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tleft);

        for (int j = 0; j < points->shape[0]; j++){
            out = (outPDO*) ec_slave[j + 1].outputs;
            out->p_target_position = points->axis[j][i];
        }

        if (ec_slave[0].hasdc){
           /* calulate toff to get linux time and DC synced */
           ec_sync(ec_DCtime, cycletime, &toff);
        }

        ec_send_processdata();
        ec_receive_processdata(EC_TIMEOUTRET);  
        printf("\n");
    }
    PyArrayObject_Free(points);
}
