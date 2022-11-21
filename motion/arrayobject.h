#ifndef _ARRAYOBJECT_H
#define _ARRAYOBJECT_H

#define MAXDIMS 10

typedef struct {
    int *axis[MAXDIMS];
    /* кол-во строк и колонок */
    int shape[2];
    /* текущий индекс */
    int index;

} PyArrayObject_coordinates;


void create_array(PyArrayObject_coordinates *py_arr, int row, int col);
int add_column_array(PyArrayObject_coordinates *py_arr, int point[], int dim);
void print_array(PyArrayObject_coordinates *py_arr);
void PyArrayObject_Free(PyArrayObject_coordinates *py_arr);
void PyArrayObject_Free(PyArrayObject_coordinates *py_arr);

#endif /* _ARRAYOBJECT_H */
