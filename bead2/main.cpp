
#include "partition.hpp"
#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <thread>

struct bool_condition : condition {
  bool flag;
  bool eval(const record &rec) const { return rec.flag == flag; }
  bool_condition(bool f) : flag(f) {}
};

struct int_condition : condition {
  int value;
  int comparision;
  bool eval(const record &rec) const {
    if (comparision == 1)
      return rec.value < value;

    else if (comparision == 0)
      return rec.value == value;

    else
      return rec.value > value;
  }
  int_condition(int val, int comp) : value(val), comparision(comp) {}
};

struct string_condition : condition {
  std::shared_ptr<std::string> data;
  bool eval(const record &rec) const {
    return rec.data == data ||
           (rec.data != nullptr && data != nullptr && *rec.data == *data);
  }
  string_condition(std::string str) {
    if (str == "NULL")
      data = nullptr;
    else
      data = std::make_shared<std::string>(str);
  }
};

query_condition read_query_condition(std::ifstream &in,
                                     int number_of_conditions) {

  std::list<std::shared_ptr<condition>> out;

  int type;
  for (size_t i = 0; i < number_of_conditions; i++) {
    in >> type;
    if (type == 1) {
      bool flag;
      in >> flag;
      bool_condition cond = {flag};
      out.push_back(std::make_shared<bool_condition>(cond));

    } else if (type == 2) {
      int comp, val;
      in >> val >> comp;
      int_condition cond = {val, comp};
      out.push_back(std::make_shared<int_condition>(cond));

    } else {
      std::string data;
      in >> data;
      string_condition cond = {data};
      out.push_back(std::make_shared<string_condition>(cond));
    }
  }

  return query_condition(out);
}

void write_output(Pipe<query> &pipe) {
  std::ofstream outfile;
  outfile.open("output.txt");
  auto curr = pipe.pop();
  while (curr != nullptr) {
    if (curr->hits.size() > 0) {
      for (auto hit : curr->hits) {
        outfile << hit << std::endl;
      }
    } else
      outfile << "{could not find records}" << std::endl;

    curr = pipe.pop();
  }
  outfile.close();
}

int main(int argc, char const *argv[]) {
  std::ifstream input;
  input.open("input.txt");
  int partition_count;
  input >> partition_count;
  std::vector<Pipe<query>> pipes(partition_count + 1);
  std::vector<std::thread> partitions;
  for (int i = 0; i < partition_count; i++) {
    partitions.push_back(std::thread(partition, i + 1, std::ref(pipes[i]),
                                     std::ref(pipes[i + 1])));
  }
  std::thread output_thread =
      std::thread(write_output, std::ref(pipes[pipes.size() - 1]));

  int number_of_queries;
  input >> number_of_queries;
  for (size_t i = 0; i < number_of_queries; i++) {
    int number_of_conditions;
    input >> number_of_conditions;
    query_condition conditions =
        read_query_condition(input, number_of_conditions);

    pipes[0].push(std::make_shared<query_condition>(conditions));
  }
  input.close();
  pipes[0].push(nullptr);

  std::for_each(partitions.begin(), partitions.end(),
                [](std::thread &th) { th.join(); });

  output_thread.join();

  return 0;
}
