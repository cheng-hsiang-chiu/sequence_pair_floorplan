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

    //void swap_two_nodes_one_sequence(std::vector<int>& sequence);

    std::list<std::vector<size_t>> get_sequences();
    
    void swap_two_nodes_two_sequences();

    /*    
    graph::DAG generate_dag(
      const std::vector<int> positive_sequence,
      const std::vector<int> negative_sequence,
      const bool is_horizontal) const;
    */
    std::vector<size_t> get_topology_order();

    void generate_adjacency_list(
      const std::vector<size_t>& positive_sequence, 
      const std::vector<size_t>& negative_sequence, 
      const bool is_horizontal);

    std::vector<std::vector<size_t>> get_adjacency_list_horizontal();
    
    std::vector<std::vector<size_t>> get_adjacency_list_vertical();
    
    std::vector<size_t> get_longest_path(const bool is_horizontal);
};


// read in the module configurations
void SequencePairTester::open(const std::string input_file) {

  std::cout << "sp tester open\n";
  sp.open(input_file);
}


// generate an initial pair
void SequencePairTester::generate_initial_pair() {

  sp._generate_initial_pair();
  
  sp._positive_sequence_curr[0] = 1;
  sp._positive_sequence_curr[1] = 2;
  sp._positive_sequence_curr[2] = 4;
  sp._positive_sequence_curr[3] = 5;
  sp._positive_sequence_curr[4] = 3;
  sp._positive_sequence_curr[5] = 0;

  sp._negative_sequence_curr[0] = 3;
  sp._negative_sequence_curr[1] = 2;
  sp._negative_sequence_curr[2] = 0;
  sp._negative_sequence_curr[3] = 1;
  sp._negative_sequence_curr[4] = 4;
  sp._negative_sequence_curr[5] = 5;
}


// get the modules
std::vector<Node> SequencePairTester::get_modules() {
  
  return sp._modules;
}


// rotate a moudle
void SequencePairTester::rotate_module() {
 
  sp._rotate_module(); 
}

/*
// swap two modules in one sequence
void SequencePairTester::swap_two_nodes_one_sequence(
  std::vector<int>& sequence) {
 
  sp._swap_two_nodes_one_sequence(sequence); 
}
*/

// get positive and negative sequences
std::list<std::vector<size_t>> SequencePairTester::get_sequences() {
  
  std::list<std::vector<size_t>> sequences;
  sequences.emplace_back(sp._positive_sequence_curr);
  sequences.emplace_back(sp._negative_sequence_curr);

  return sequences;
}


// swap two moduels in both sequences
void SequencePairTester::swap_two_nodes_two_sequences() {

  sp._swap_two_nodes_two_sequences();
}


/*
// generate a DAG
graph::DAG SequencePairTester::generate_dag(
  const std::vector<int> positive_sequence,
  const std::vector<int> negative_sequence,
  const bool is_horizontal) const {

  return sp._generate_dag(positive_sequence, negative_sequence, is_horizontal);
}
*/


// get adjacency list
std::vector<std::vector<size_t>> 
  SequencePairTester::get_adjacency_list_horizontal() {
  return sp._adjacency_list_horizontal;
}


// get adjacency list negative
std::vector<std::vector<size_t>> 
  SequencePairTester::get_adjacency_list_vertical() {
  return sp._adjacency_list_vertical;
}


// generate adjacency list
void SequencePairTester::generate_adjacency_list(
  const std::vector<size_t>& positive_sequence, 
  const std::vector<size_t>& negative_sequence, 
  const bool is_horizontal) {

  sp._generate_adjacency_list(
    positive_sequence, negative_sequence, is_horizontal);
}


// get topology order
std::vector<size_t> SequencePairTester::get_topology_order() {
  sp._get_topology_order(true);

  return sp._topology_order;
}


/*
// get longest path
std::vector<int> SequencePairTester::get_longest_path(
  const bool is_horizontal) {
  
  sp._get_longest_path(is_horizontal);
  return sp._longest_path;
}
*/


}
