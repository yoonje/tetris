#include "Matrix_e.h"

int Matrix::nAlloc = 0;
int Matrix::nFree = 0;

int Matrix::get_nAlloc() const { return nAlloc; }

int Matrix::get_nFree() const { return nFree; }

int Matrix::get_dy() const { return dy; }

int Matrix::get_dx() const { return dx; }

int **Matrix::get_array() const { return array; }

void Matrix::alloc(int cy, int cx) throw(MatrixException) {
  if ((cy < 0) || (cx < 0))
    throw MatrixException("wrong matrix size");
  dy = cy;
  dx = cx;
  array = new int*[dy];
  for (int y = 0; y < dy; y++)
    array[y] = new int[dx];
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = 0;
  nAlloc++;
}

Matrix::Matrix() throw(MatrixException) { alloc(0, 0); }

Matrix::~Matrix() { 
  for (int y = 0; y < dy; y++)
    delete array[y];
  delete array;
  nFree++;
}

Matrix::Matrix(int cy, int cx) throw(MatrixException) {
  alloc(cy, cx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = 0;
}

Matrix::Matrix(const Matrix *obj) throw(MatrixException) {
  alloc(obj->dy, obj->dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj->array[y][x];
}

Matrix::Matrix(const Matrix &obj) throw(MatrixException) {
  alloc(obj.dy, obj.dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj.array[y][x];
}

Matrix::Matrix(int *arr, int col, int row) throw(MatrixException) {
  alloc(col, row);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = arr[y * dx + x];
}

Matrix *Matrix::clip(int top, int left, int bottom, int right) throw(MatrixException) {
  int cy = bottom - top;
  int cx = right - left;
  Matrix *temp = new Matrix(cy, cx);
  for (int y = 0; y < cy; y++) {
    for (int x = 0; x < cx; x++) {
      if ((top + y >= 0) && (left + x >= 0) &&
	  (top + y < dy) && (left + x < dx))
	temp->array[y][x] = array[top + y][left + x];
      else
	throw MatrixException("invalid matrix range");
    }
  }
  return temp;
}

void Matrix::paste(const Matrix *obj, int top, int left) throw(MatrixException) {
  for (int y = 0; y < obj->dy; y++)
    for (int x = 0; x < obj->dx; x++) {
      if ((top + y >= 0) && (left + x >= 0) &&
	  (top + y < dy) && (left + x < dx))
	array[y + top][x + left] = obj->array[y][x];
      else
	throw MatrixException("invalid matrix range");
    }
}

Matrix *Matrix::add(const Matrix *obj) throw(MismatchedMatrixException) {
  if ((dx != obj->dx) || (dy != obj->dy))
    throw MismatchedMatrixException("Mismatched Matrices");
  //throw MatrixException("Wrong Matrices");
  Matrix *temp = new Matrix(dy, dx);
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      temp->array[y][x] = array[y][x] + obj->array[y][x];
  return temp;
}

int Matrix::sum() {
  int total = 0;
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      total += array[y][x];
  return total;
}

void Matrix::mulc(int coef) {
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = coef * array[y][x];
}

Matrix *Matrix::int2bool() {
  Matrix *temp = new Matrix(dy, dx);
  int **t_array = temp->get_array();
  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      t_array[y][x] = (array[y][x] != 0 ? 1 : 0);
  
  return temp;
}

bool Matrix::anyGreaterThan(int val) {
  for (int y = 0; y < dy; y++) {
    for (int x = 0; x < dx; x++) {
      if (array[y][x] > val)
	return true;
    }
  }
  return false;
}

void Matrix::print() {
  cout << "Matrix(" << dy << "," << dx << ")" << endl;
  for (int y = 0; y < dy; y++) {
    for (int x = 0; x < dx; x++)
      cout << array[y][x] << " ";
    cout << endl;
  }
}


ostream& operator<<(ostream& out, const Matrix& obj){
  out << "Matrix(" << obj.dy << "," << obj.dx << ")" << endl;
  for(int y = 0; y < obj.dy; y++){
    for(int x = 0; x < obj.dx; x++)
      out << obj.array[y][x] << " ";
    out << endl;
  }
  out << endl;
  return out;
}

Matrix& Matrix::operator=(const Matrix& obj)
{
  if (this == &obj) return *this;
  if ((dx != obj.dx) || (dy != obj.dy))
    alloc(obj.dy, obj.dx);

  for (int y = 0; y < dy; y++)
    for (int x = 0; x < dx; x++)
      array[y][x] = obj.array[y][x];
  return *this;
}

