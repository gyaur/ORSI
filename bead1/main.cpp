#include "matrix.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <string>
#include <thread>

struct Task {
  Matrix A, B, C;
  int threshold;
};

Task readInput(std::string fname) {
  int size, threshold;
  std::ifstream f;
  f.open(fname);
  f >> size >> threshold;
  std::vector<Matrix> matrices = {Matrix(size, size), Matrix(size, size),
                                  Matrix(size, size)};
  int temp;
  for (Matrix &m : matrices) {
    for (int i = 0; i < size * size; i++) {
      f >> temp;
      m.setElement(i / size, i % size, temp);
    }
  }
  f.close();

  return Task{matrices[0], matrices[1], matrices[2], threshold};
}

void writeOutput(std::string fname, std::string mes) {
  std::ofstream f;
  f.open(fname);
  f << mes + "\n";
  f.close();
}

Matrix getRandomVector(int size) {
  Matrix out = Matrix(size, 1);
  for (int i = 0; i < size; i++) {
    out.setElement(i, 0, rand() % 2);
  }
  return out;
}

bool frievalds(const Matrix &A, const Matrix &B, const Matrix &C) {
  Matrix alpha = getRandomVector(A.getHeight());
  Matrix beta = A * (B * alpha);
  Matrix gamma = C * alpha;
  return beta == gamma;
}

int main(int argc, char const *argv[]) {
  Task a = readInput("input.txt");
  std::vector<std::future<bool>> futures;
  for (int i = 0; i < a.threshold; i++) {
    futures.push_back(std::async(std::launch::async, frievalds, std::ref(a.A),
                                 std::ref(a.B), std::ref(a.C)));
  }
  bool res = std::all_of(futures.begin(), futures.end(),
                         [](std::future<bool> &b) { return b.get() == true; });
  writeOutput("output.txt", res ? "accepted" : "rejected");
}
