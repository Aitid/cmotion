#include <math.h>
#include <stdio.h>

#include "ethercat.h"
#include "wrap_master.h"
#include "arrayobject.h"
#include "operation.h"


PyArrayObject_coordinates generated_pulse(){

    PyArrayObject_coordinates points;
    create_array(&points, 3, 1001);
    int p[] = {0, 0, 0};
    // int p[] = {0};
    add_column_array(&points, p, 3);

    for (int i = 0; i < 1000; i++){
        // int p[] = {(int) (3000.0 * sin(i) + 1000), (int) (1000.0 * sin(i) + 1000)};
        // int p[] = {-i * 100, -i * 100, -i * 100};
        int p[] = {0, i * 1000, 0};
        add_column_array(&points, p, 3);
    }

    return points;
}


int main(int argc, char *argv[]) {
    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");
    int32 cycletime;  /* cycletime in ns */

    if (argc == 3) {
        sscanf (argv[2], "%d", &cycletime);

        printf("ifname: %s arg 2: %s \n", argv[1], argv[2]);
        printf("cycletime: %d \n", cycletime);

        setup(argv[1], cycletime);

        PyArrayObject_coordinates p1;
        p1 = generated_pulse();

        enable_csp_mode();
        rt_csp(&p1, cycletime);
        complete();
    }
    else {
        printf("Usage: mycsp ifname cycletime  \n");
    }
    printf("End program\n");
    return (0);
}
