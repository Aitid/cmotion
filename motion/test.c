#include <math.h>
#include <stdio.h>

#include "arrayobject.h"
#include "operation.h"
#include "wrap_master.h"


PyArrayObject_coordinates generated_pulse(){

    PyArrayObject_coordinates points;
    create_array(&points, 2, 101);
    int p[] = {0, 0};
    add_column_array(&points, p, 2);

    for (int i = 0; i < 100; i++){
        int p[] = {(int) (3000.0 * sin(i) + 1000), (int) (1000.0 * sin(i) + 1000)};
        add_column_array(&points, p, 2);
    }

    return points;
}


void start(char *ifname){
    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");
    setup(ifname);

    PyArrayObject_coordinates p1, p2;
    p1 = generated_pulse();
    p2 = generated_pulse();

    enable_csp_mode();
    rt_csp(&p1);
    rt_csp(&p2);
    complete();

}
