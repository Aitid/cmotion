#include <stdlib.h>
#include <stdio.h>
#include "arrayobject.h"


void create_array(PyArrayObject_coordinates *py_arr, int row, int col){
    /* 
     * 2D матрица, где порядковый номер строки соответствует номеру привода/slave
     * В колонке хрониться кол-во пульсов на привод
     *
     * Например shape: 4 X 6
     *
     *     C0 C1 C2 C3 C4 C5 C6
     * R0 |--|--|--|--|--|--|--|
     * R1 |--|--|--|--|--|--|--|
     * R2 |--|--|--|--|--|--|--|
     * R3 |--|--|--|--|--|--|--|
     * R4 |--|--|--|--|--|--|--|
     * 
     * shape[0] = 4 - кол-во приводов/осей
     * shape[1] = 6 - кол-во TARGETs 
     *
     */

    for (int i = 0; i < row; i++){
        py_arr->axis[i] = malloc(col * sizeof(int));
    }

    py_arr->shape[0] = row;
    py_arr->shape[1] = col;
    py_arr->index = -1;
}


int add_column_array(PyArrayObject_coordinates *py_arr, int point[], int dim){
    /*
     * Добавление колонки в матрицу справо
     *
     * Например:
     *
     *     C0 C1 C2 C3 C4 C5        C6          C0 C1 C2 C3 C4 C5 C6
     * R0 |--|--|--|--|--|--|      |--|     R0 |--|--|--|--|--|--|--|
     * R1 |--|--|--|--|--|--|  +   |--|  =  R1 |--|--|--|--|--|--|--|
     * R2 |--|--|--|--|--|--|      |--|     R2 |--|--|--|--|--|--|--|
     * R3 |--|--|--|--|--|--|      |--|     R3 |--|--|--|--|--|--|--|
     * R4 |--|--|--|--|--|--|      |--|     R4 |--|--|--|--|--|--|--|
     * 
     */

    if (py_arr->shape[0] != dim){
        printf("ValueError: not consistency");
        return 0;
    }

    if (py_arr->shape[1] <= py_arr->index + 1){
        printf("\nIndexError: array index out of range");
        return 0;
    }

    py_arr->index++;

    for (int i = 0; i < py_arr->shape[0]; i++){
        py_arr->axis[i][py_arr->index] = point[i];
    }
    return 1;
}


void print_array(PyArrayObject_coordinates *py_arr){
    printf("\n");
    for (int i = 0; i < py_arr->shape[0]; i++){
        for (int j = 0; j < py_arr->shape[1]; j++){
            printf("%d ", py_arr->axis[i][j]);
        }
        printf("\n");
    }
}

void PyArrayObject_Free(PyArrayObject_coordinates *py_arr) {
    /*
     * удаляем матрицу из память 
     */

    for (int i; i < py_arr->shape[0]; i++)
    {
        free(py_arr->axis[i]);
    }
}
