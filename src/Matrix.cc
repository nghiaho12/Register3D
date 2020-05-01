#include "Matrix.h"

void Matrix::InitVars() { m = NULL; }

Matrix::Matrix() { InitVars(); }

Matrix::Matrix(int row, int col) { Create(row, col); }

Matrix::~Matrix()
{
    if (m)
        gsl_matrix_free(m);
}

Matrix::Matrix(const Matrix& b)
{
    Create(b.GetRow(), b.GetCol());

    gsl_matrix_memcpy(m, b.m);
}

gsl_matrix* Matrix::GetMatrix() { return m; }

int Matrix::GetRow() const { return m->size1; }

int Matrix::GetCol() const { return m->size2; }

void Matrix::Create(int row, int col)
{
    // All memory allocation occurs here only
    m = gsl_matrix_calloc(row, col);
}

/*
inline void Matrix::Set(int row, int col, double val)
{
        gsl_matrix_set(m, row, col, val);
}

double Matrix::Get(int row, int col)
{
        return gsl_matrix_get(m, row, col);
}
*/

void Matrix::SVD(Matrix& ret_U, Matrix& ret_S, Matrix& ret_V)
{
    gsl_vector* s;

    s = gsl_vector_calloc(m->size2);

    gsl_matrix_memcpy(ret_U.m, m);

    //	gsl_linalg_SV_decomp_jacobi(ret_U.m, ret_V.m, s);

    gsl_matrix* NN = gsl_matrix_calloc(m->size2, m->size2);
    gsl_vector* work = gsl_vector_calloc(m->size2);
    gsl_linalg_SV_decomp_mod(ret_U.m, NN, ret_V.m, s, work);

    // Copy vector to matrix S
    for (unsigned int i = 0; i < s->size; i++)
        gsl_matrix_set(ret_S.m, i, i, gsl_vector_get(s, i));

    gsl_vector_free(s);

    gsl_matrix_free(NN);
    gsl_vector_free(work);
}

Matrix Matrix::Transpose() const
{
    Matrix ret(m->size2, m->size1);

    gsl_matrix_transpose_memcpy(ret.GetMatrix(), m);

    return ret;
}

void Matrix::SquareTranspose() { gsl_matrix_transpose(m); }

Matrix Matrix::operator-(const Matrix& b) const
{
    // Matrix a(GetRow(), GetCol());
    Matrix a(*this);

    // gsl_matrix_memcpy(a.m, m);
    gsl_matrix_sub(a.m, b.m);

    return a;
}

void Matrix::Add(Matrix& b) { gsl_matrix_add(m, b.m); }

void Matrix::Copy(Matrix& b) { gsl_matrix_memcpy(m, b.m); }

Matrix& Matrix::operator=(const Matrix& b)
{
    if (m)
        gsl_matrix_free(m);

    Create(b.GetRow(), b.GetCol());

    gsl_matrix_memcpy(m, b.m);

    return *this;
}

Matrix& Matrix::operator+=(const Matrix& b)
{
    gsl_matrix_add(m, b.m);

    return *this;
}

Matrix Matrix::operator+(const Matrix& b) const
{
    Matrix a = *this;

    gsl_matrix_add(a.m, b.m);

    return a;
}

Matrix Matrix::operator*(const Matrix& b) const
{
    Matrix a(GetRow(), b.GetCol());

    gsl_linalg_matmult(m, b.m, a.m);

    return a;
}

Matrix Matrix::operator*(const double val) const
{
    Matrix a(*this);

    //	gsl_matrix_memcpy(a.m, m);
    gsl_matrix_scale(a.m, val);

    return a;
}

Matrix Matrix::operator/(const double val) const
{
    // Matrix a(GetRow(), GetCol());
    Matrix a(*this);

    // gsl_matrix_memcpy(a.m, m);
    gsl_matrix_scale(a.m, 1.0 / val);

    return a;
}

Matrix& Matrix::operator/=(const double val)
{
    gsl_matrix_scale(m, 1.0 / val);

    return *this;
}

void Matrix::Print()
{
    for (int i = 0; i < GetRow(); i++) {
        for (int j = 0; j < GetCol(); j++)
            printf("%4.3f ", Get(i, j));

        printf("\n");
    }
}

void Matrix::LoadIdentity()
{
    for (int i = 0; i < GetRow(); i++)
        for (int j = 0; j < GetCol(); j++)
            Set(i, j, 0);

    for (int i = 0; i < GetRow(); i++)
        Set(i, i, 1.0);
}

Matrix Matrix::Inverse() const
{
    gsl_matrix_view view = gsl_matrix_view_array(m->data, m->size1, m->size2);

    gsl_matrix* minv = gsl_matrix_alloc(m->size1, m->size2);

    gsl_permutation* p = gsl_permutation_alloc(m->size1);

    int s;

    gsl_linalg_LU_decomp(&view.matrix, p, &s);
    gsl_linalg_LU_invert(&view.matrix, p, minv);

    /* minv is now inverse of m */
    Matrix ret(m->size1, m->size2);

    gsl_matrix_memcpy(ret.GetMatrix(), minv);

    // Free memory
    gsl_permutation_free(p);
    gsl_matrix_free(minv);

    return ret;
}

double Matrix::Determinant()
{
    int s;
    double det;

    gsl_permutation* p = gsl_permutation_alloc(m->size1);
    gsl_matrix* ludecomp = gsl_matrix_calloc(GetRow(), GetRow());

    gsl_matrix_memcpy(ludecomp, m);

    gsl_linalg_LU_decomp(ludecomp, p, &s);

    det = gsl_linalg_LU_det(ludecomp, s);

    // Free memory
    gsl_permutation_free(p);
    gsl_matrix_free(ludecomp);

    return det;
}

void Matrix::Zero()
{
    for (int i = 0; i < GetRow(); i++)
        for (int j = 0; j < GetCol(); j++)
            Set(i, j, 0.0);
}
