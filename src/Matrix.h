#ifndef MATRIX_H
#define MATRIX_H

/*
My own wrapper around GNU Scientific Library
*/

#include "Global.h"
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

extern _Global Global;

class Matrix {
public:
    // Test
    static void PrintMem();
    Matrix();
    Matrix(int row, int col);
    Matrix(const Matrix& b);
    ~Matrix();

    gsl_matrix* GetMatrix();

    void InitVars();
    int GetRow() const; // Returns number of rows
    int GetCol() const; // Returns number of columns
    void Create(int row, int col); // Create the matrix

    inline void Set(int row, int col, double val)
    {
        gsl_matrix_set(m, row, col, val);
        // m->data[row*m->size2 + col] = val;
    }

    inline double Get(int row, int col)
    {
        return gsl_matrix_get(m, row, col);
        // return m->data[row*m->size2 + col];
    }

    void SVD(Matrix& ret_U, Matrix& ret_S,
        Matrix& ret_V); // Singular Value Decomposition
    void SquareTranspose(); //
    void Add(Matrix& b); // Add this matrix to matrix b
    void Copy(Matrix& b); // Copy this matrix to matrix b
    void Print(); // Print the matrix to stdout
    void LoadIdentity(); // Load the identity matrix
    double Determinant();
    void Zero();

    Matrix Transpose() const; // Transpose this matrix
    Matrix Inverse() const;

    Matrix& operator=(const Matrix& b);
    Matrix& operator+=(const Matrix& b);
    Matrix operator+(const Matrix& b) const;
    Matrix operator-(const Matrix& b) const;
    Matrix operator*(const Matrix& b) const;
    Matrix operator*(const double val) const;
    Matrix operator/(const double val) const;
    Matrix& operator/=(const double val);

private:
    gsl_matrix* m;
};

#endif
