#pragma once
#include <iostream>
#include <cstdlib>

using namespace std;

class MatrixException {
public:
  MatrixException(): message("Matrix Exception") { }
  MatrixException(string msg): message(msg) {}
  string getMessage() const { return message; }
protected:
  string message;
};

class MismatchedMatrixException : public MatrixException {
public:
  MismatchedMatrixException(): MatrixException("Mismatched Matrix Exception") {}
  MismatchedMatrixException(string msg): MatrixException(msg) {}
};

class Matrix {
private:
  static int nAlloc;
  static int nFree;
  int dy;
  int dx;
  int **array;
  void alloc(int cy, int cx) throw(MatrixException);
public:
  int get_nAlloc() const;
  int get_nFree() const;
  int get_dy() const;
  int get_dx() const;
  int** get_array() const;
  Matrix() throw(MatrixException);
  Matrix(int cy, int cx) throw(MatrixException);
  Matrix(const Matrix *obj) throw(MatrixException);
  Matrix(const Matrix &obj) throw(MatrixException);
  Matrix(int *arr, int col, int row) throw(MatrixException);
  ~Matrix();
  Matrix *clip(int top, int left, int bottom, int right) throw(MatrixException);
  void paste(const Matrix *obj, int top, int left) throw(MatrixException);
  Matrix *add(const Matrix *obj) throw(MismatchedMatrixException);
  int sum();
  void mulc(int coef);
  Matrix *int2bool();
  bool anyGreaterThan(int val);
  void print();
  friend ostream& operator<<(ostream& out, const Matrix& obj);
  Matrix& operator=(const Matrix& obj);
};
