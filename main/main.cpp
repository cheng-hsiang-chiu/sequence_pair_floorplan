#include <iostream>
#include "../src/sp.hpp"



int main(int argc, char* argv[]) {

  if (argc < 3) {
    std::cerr << "usage: ./sp input output\n";
    std::exit(EXIT_FAILURE);
  }

  std::string input_file = argv[1];
  std::string output_file = argv[2];

  sp::SequencePair obj;

  obj.open(input_file);

  obj.optimize();

  obj.dump_json(output_file);

  return 0;
}
