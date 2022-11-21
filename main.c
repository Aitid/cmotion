#include <math.h>
#include <stdio.h>

#include "wrap_master.h"
#include "arrayobject.h"
#include "operation.h"


PyArrayObject_coordinates generated_pulse(){

    PyArrayObject_coordinates points;
    create_array(&points, 2, 1001);
    int p[] = {0, 0};
    // int p[] = {0};
    add_column_array(&points, p, 2);

    for (int i = 0; i < 1000; i++){
        // int p[] = {(int) (3000.0 * sin(i) + 1000), (int) (1000.0 * sin(i) + 1000)};
        int p[] = {i * 1000, i * 1000};
        // int p[] = {i * 1000};
        add_column_array(&points, p, 2);
    }

    return points;
}


int main(int argc, char *argv[]) {
    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

    if (argc == 2)
    {
        printf("ifname: %s \n", argv[1]);

        setup(argv[1]);

        PyArrayObject_coordinates p1;  //, p2;
        p1 = generated_pulse();
        // p2 = generated_pulse();

        enable_csp_mode();
        rt_csp(&p1);
        // rt_csp(&p2);
        complete();
        
    }
    else
    {
        printf("Usage: mycsp ifname \n");
    }
    printf("End program\n");
    return (0);
}
