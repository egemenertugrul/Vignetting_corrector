#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "matrix.h"

/*Matrix Inverse*/
int invertMatrix(double *m1, int n)
{
    int *is, *js;
    int i, j, k, l, u, v;
    double temp, max_v;
    is = (int *)malloc(n * sizeof(int));
    js = (int *)malloc(n * sizeof(int));
    if(is==NULL||js==NULL)
    {
        printf("out of memory!\n");
        return(0);
    }
    for(k=0;k<n;k++)
    {
        max_v=0.0;
        for(i=k;i<n;i++)
            for (j = k; j < n; j++)
            {
                temp = fabs(m1[i*n + j]);
                if (temp > max_v)
                {
                    max_v = temp; is[k] = i; js[k] = j;
                }
            }
        if(max_v==0.0)
        {
            free(is); free(js);
            printf("invers is not availble!\n");
            return(0);
        }
        if(is[k]!=k)
        {
            for (j = 0; j<n; j++)
            {
                u = k*n + j; v = is[k] * n + j;
                temp = m1[u]; m1[u] = m1[v]; m1[v] = temp;
            }
        }
        
        if(js[k]!=k)
        {
            for (i = 0; i<n; i++) {
                u = i*n + k; v = i*n + js[k];
                temp = m1[u]; m1[u] = m1[v]; m1[v] = temp;
            }
        }
        
        l=k*n+k;
        m1[l]=1.0/m1[l];
        for(j=0;j<n;j++)
            if(j!=k)
            {
                u=k*n+j;
                m1[u]*=m1[l];
            }
        for (i = 0; i < n; i++)
        {
            if (i != k)
                for (j = 0; j < n; j++)
                    if (j != k)
                    {
                        u = i*n + j;
                        m1[u] -= m1[i*n + k] * m1[k*n + j];
                    }
        }
        for(i=0;i<n;i++)
        {
            if (i != k)
            {
                u = i*n + k;
                m1[u] *= -m1[l];
            }
        }
        
    }
    for(k=n-1;k>=0;k--)
    {
        if(js[k]!=k)
        {
            for (j = 0; j<n; j++) {
                u = k*n + j; v = js[k] * n + j;
                temp = m1[u]; m1[u] = m1[v]; m1[v] = temp;
            }
        }
        
        if(is[k]!=k)
        {
            for (i = 0; i<n; i++) {
                u = i*n + k; v = i*n + is[k];
                temp = m1[u]; m1[u] = m1[v]; m1[v] = temp;
            }
        }
        
    }
    free(is);
    free(js);
    return(1);
}

/*Matrix Transpose*/
void transposeMatrix(double *m1,double *m2, int m, int n)
{
    int i, j;
    for(i = 0; i < m; i++)
        for(j = 0; j < n; j++)
            m2[j * m + i] = m1[i * n + j];
    return;
}

/*Matrix  Multiplication*/
void multiplyMatrices(double *m1,double *m2,double *result, int row1, int col1, int col2)
{
    #pragma omp parallel for schedule(static)
    for(int i = 0; i < row1; i++)
    {
        for(int j = 0; j < col2; j++)
        {
            result[i * col2 + j]=0.0;
            
            for(int k = 0; k < col1; k++)
                result[i * col2 + j] += m1[i * col1 + k] * m2[j + k * col2];
        }
    }
    return;
}
