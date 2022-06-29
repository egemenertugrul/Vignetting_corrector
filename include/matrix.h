#ifndef MATRIX_H
#define MATRIX_H


//template <typename T>
//extern bool transposeMatrix(T *m1, T *m2, int row1, int col1);
//template <typename T>
//extern bool invertMatrix(T *m1, int row1, int col1);
//template <typename T>
//extern bool multiplyMatrices(T *m1, T *m2, T *m3, int row1, int row2, int col1, int col2);


void transposeMatrix(double *m1,double *m2,int m,int n);
void multiplyMatrices(double *m1,double *m2,double *result,int i_1,int j_12,int j_2);
int invertMatrix(double *m1,int n);

#endif
