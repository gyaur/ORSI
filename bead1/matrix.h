#pragma once
#include <vector>

class Matrix {
private:
  std::vector<std::vector<int>> matrix;
  int height, width;

public:
  Matrix(int h, int w);
  Matrix(const std::vector<std::vector<int>> &m);
  Matrix(const Matrix &m);
  int getHeight() const;
  int getWidth() const;
  int getElement(int x, int y) const;
  void setElement(int x, int y, int n);
  void print() const;
  friend Matrix operator*(const Matrix &lhs, const Matrix &rhs);
  friend bool operator==(const Matrix &lhs, const Matrix &rhs);
};
#include "matrix.h"
#include <iostream>

Matrix::Matrix(int h, int w) {
  height = h;
  width = w;

  matrix.resize(width);
  for (auto &vec : matrix) {
    vec.resize(height);
    for (auto &element : vec) {
      element = 0;
    }
  }
}

Matrix::Matrix(const Matrix &m) {
  height = m.height;
  width = m.width;
  matrix = m.matrix;
}

Matrix::Matrix(const std::vector<std::vector<int>> &m) {
  height = m[0].size();
  width = m.size();
  matrix = m;
}

int Matrix::getHeight() const { return height; }
int Matrix::getWidth() const { return width; }
void Matrix::setElement(int x, int y, int n) { matrix[y][x] = n; }
int Matrix::getElement(int x, int y) const { return matrix[y][x]; }

Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
  if (lhs.width == rhs.height) {
    Matrix out = Matrix(lhs.height, rhs.width);

    for (int i = 0; i < lhs.height; i++) {
      for (int j = 0; j < rhs.width; j++) {
        int value = 0;
        for (int k = 0; k < lhs.width; k++) {
          value += lhs.matrix[k][i] * rhs.matrix[j][k];
        }
        out.setElement(i, j, value);
      }
    }
    return out;
  }
  throw std::invalid_argument("Incorrect sizes");
}
bool operator==(const Matrix &lhs, const Matrix &rhs) {
  return rhs.matrix == lhs.matrix;
}

void Matrix::print() const {
  for (int i = 0; i < height; i++) {
    std::cout << "|";
    for (int j = 0; j < width; j++) {
      std::cout << matrix[j][i] << "|";
    }
    std::cout << std::endl;
  }
}
