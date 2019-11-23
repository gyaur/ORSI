#include "utils.hpp"
#include <fstream>
#include <iostream>

record read_record(std::ifstream &in) {
  int value;
  bool flag;
  std::string data;
  in >> value >> flag >> data;
  return record(value, flag,
                data == "NULL" ? nullptr : std::make_shared<std::string>(data));
}

std::vector<record> get_records(int id) {
  std::ifstream input;
  input.open("input" + std::to_string(id) + ".dat");
  int number_of_records;
  input >> number_of_records;
  std::vector<record> out;
  for (size_t i = 0; i < number_of_records; i++) {
    out.push_back(read_record(input));
  }
  return out;
}
void process_queries(const std::vector<record> &records, Pipe<query> &inPipe,
                     Pipe<query> &outPipe) {
  query curr = inPipe.pop();
  while (curr != nullptr) {
    for (auto &&record : records) {
      if (eval_all(curr->conditions, record))
        curr->hits.push_back(record);
        // std::cout<<record<<std::endl;
    }
    outPipe.push(curr);
    curr = inPipe.pop();
  }
}

void partition(int id, Pipe<query> &inPipe, Pipe<query> &outPipe) {
  std::vector<record> records = get_records(id);
  process_queries(records, inPipe, outPipe);
  outPipe.push(nullptr);
}
