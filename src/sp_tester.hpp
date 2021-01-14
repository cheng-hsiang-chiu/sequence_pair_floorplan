#include <list>
#include "sp.hpp"



namespace sp {
 
 
  
class SequencePairTester{
  public:

    SequencePair sp;

    SequencePairTester() = default;

    void open(const std::string input_file);

    void rotate_module();
    
    void generate_initial_pair();

    std::vector<Node> get_modules();

    void swap_two_nodes_one_sequence(std::vector<int>& sequence);

    std::list<std::vector<int>> get_sequences();
    
    void swap_two_nodes_two_sequences(
      std::vector<int>& positive_sequence,
      std::vector<int>& negative_sequence);
    
    graph::DAG generate_dag(
      const std::vector<int> positive_sequence,
      const std::vector<int> negative_sequence,
      const bool is_horizontal) const;
};


// read in the module configurations
void SequencePairTester::open(const std::string input_file) {

  std::cout << "sp tester open\n";
  sp.open(input_file);
}


// generate an initial pair
void SequencePairTester::generate_initial_pair() {

  sp._generate_initial_pair();
  
  sp._positive_sequence[0] = 1;
  sp._positive_sequence[1] = 2;
  sp._positive_sequence[2] = 4;
  sp._positive_sequence[3] = 5;
  sp._positive_sequence[4] = 3;
  sp._positive_sequence[5] = 0;

  sp._negative_sequence[0] = 3;
  sp._negative_sequence[1] = 2;
  sp._negative_sequence[2] = 0;
  sp._negative_sequence[3] = 1;
  sp._negative_sequence[4] = 4;
  sp._negative_sequence[5] = 5;
}


// get the modules
std::vector<Node> SequencePairTester::get_modules() {
  
  return sp._modules;
}


// rotate a moudle
void SequencePairTester::rotate_module() {
 
  sp._rotate_module(); 
}


// swap two modules in one sequence
void SequencePairTester::swap_two_nodes_one_sequence(
  std::vector<int>& sequence) {
 
  sp._swap_two_nodes_one_sequence(sequence); 
}


// get positive and negative sequences
std::list<std::vector<int>> SequencePairTester::get_sequences() {
  
  std::list<std::vector<int>> sequences;
  sequences.emplace_back(sp._positive_sequence);
  sequences.emplace_back(sp._negative_sequence);

  return sequences;
}


// swap two moduels in both sequences
void SequencePairTester::swap_two_nodes_two_sequences(
  std::vector<int>& positive_sequence,
  std::vector<int>& negative_sequence) {

  sp._swap_two_nodes_two_sequences(positive_sequence, negative_sequence);
}


// generate a DAG
graph::DAG SequencePairTester::generate_dag(
  const std::vector<int> positive_sequence,
  const std::vector<int> negative_sequence,
  const bool is_horizontal) const {

  return sp._generate_dag(positive_sequence, negative_sequence, is_horizontal);
}



}
