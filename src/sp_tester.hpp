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

    void swap_two_nodes_positive_sequence();
    
    void swap_two_nodes_negative_sequence();

    std::list<std::vector<size_t>> get_sequences();
    
    void swap_two_nodes_two_sequences();

    std::vector<size_t> get_topology_order(const bool is_horizontal);

    std::vector<Node> get_longest_path(const bool is_horizontal);

    std::vector<std::vector<size_t>>  generate_adjacency_list(
      const std::vector<size_t>& positive_sequence, 
      const std::vector<size_t>& negative_sequence, 
      const bool is_horizontal);

    std::vector<size_t> get_in_degree(const bool is_horizontal);
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


// swap two modules in proposed positive sequence 
void SequencePairTester::swap_two_nodes_positive_sequence() {
  sp._swap_two_nodes_positive_sequence();
}


// swap two modules in proposed negative sequence
void SequencePairTester::swap_two_nodes_negative_sequence() {
  sp._swap_two_nodes_negative_sequence();
}


// swap two modules in both proposed sequences
void SequencePairTester::swap_two_nodes_two_sequences() {
  sp._swap_two_nodes_two_sequences();
}

// get positive and negative sequences
std::list<std::vector<size_t>> SequencePairTester::get_sequences() {
  
  std::list<std::vector<size_t>> sequences;

  sequences.emplace_back(sp._positive_sequence_curr);
  sequences.emplace_back(sp._negative_sequence_curr);
  sequences.emplace_back(sp._positive_sequence_prop);
  sequences.emplace_back(sp._negative_sequence_prop);
  sequences.emplace_back(sp._positive_sequence_best);
  sequences.emplace_back(sp._negative_sequence_best);
  
  return sequences;
}


// generate adjacency list
std::vector<std::vector<size_t>> SequencePairTester::generate_adjacency_list(
  const std::vector<size_t>& positive_sequence, 
  const std::vector<size_t>& negative_sequence, 
  const bool is_horizontal) {
  /*
  sp._adjacency_list_horizontal[0] = std::vector<size_t>{};
  sp._adjacency_list_horizontal[1] = std::vector<size_t>{4, 5};
  sp._adjacency_list_horizontal[2] = std::vector<size_t>{0, 4, 5};
  sp._adjacency_list_horizontal[3] = std::vector<size_t>{0};
  sp._adjacency_list_horizontal[4] = std::vector<size_t>{5};
  sp._adjacency_list_horizontal[5] = std::vector<size_t>{};

  sp._adjacency_list_vertical[0] = std::vector<size_t>{1, 4, 5};
  sp._adjacency_list_vertical[1] = std::vector<size_t>{};
  sp._adjacency_list_vertical[2] = std::vector<size_t>{1};
  sp._adjacency_list_vertical[3] = std::vector<size_t>{1, 2, 4, 5};
  sp._adjacency_list_vertical[4] = std::vector<size_t>{};
  sp._adjacency_list_vertical[5] = std::vector<size_t>{};
  */
   
  sp._generate_adjacency_list(
    positive_sequence, negative_sequence, is_horizontal);

  if (is_horizontal) {
    return sp._adjacency_list_horizontal;
  }
  else {
    return sp._adjacency_list_vertical;
  }

}


// get topology order
std::vector<size_t> SequencePairTester::get_topology_order(
  const bool is_horizontal) {

  sp._in_degree_horizontal = std::vector<size_t>{2, 0, 0, 0, 2, 3};
  sp._in_degree_vertical = std::vector<size_t>{0, 3, 1, 0, 2, 2};

  sp._adjacency_list_horizontal[0] = std::vector<size_t>{};
  sp._adjacency_list_horizontal[1] = std::vector<size_t>{4, 5};
  sp._adjacency_list_horizontal[2] = std::vector<size_t>{0, 4, 5};
  sp._adjacency_list_horizontal[3] = std::vector<size_t>{0};
  sp._adjacency_list_horizontal[4] = std::vector<size_t>{5};
  sp._adjacency_list_horizontal[5] = std::vector<size_t>{};

  sp._adjacency_list_vertical[0] = std::vector<size_t>{1, 4, 5};
  sp._adjacency_list_vertical[1] = std::vector<size_t>{};
  sp._adjacency_list_vertical[2] = std::vector<size_t>{1};
  sp._adjacency_list_vertical[3] = std::vector<size_t>{1, 2, 4, 5};
  sp._adjacency_list_vertical[4] = std::vector<size_t>{};
  sp._adjacency_list_vertical[5] = std::vector<size_t>{};

  sp._get_topology_order(is_horizontal);

  return sp._topology_order;
}



// get longest path
std::vector<Node> SequencePairTester::get_longest_path(
  const bool is_horizontal) {

  sp._in_degree_horizontal = std::vector<size_t>{2, 0, 0, 0, 2, 3};
  sp._in_degree_vertical = std::vector<size_t>{0, 3, 1, 0, 2, 2};

  sp._adjacency_list_horizontal[0] = std::vector<size_t>{};
  sp._adjacency_list_horizontal[1] = std::vector<size_t>{4, 5};
  sp._adjacency_list_horizontal[2] = std::vector<size_t>{0, 4, 5};
  sp._adjacency_list_horizontal[3] = std::vector<size_t>{0};
  sp._adjacency_list_horizontal[4] = std::vector<size_t>{5};
  sp._adjacency_list_horizontal[5] = std::vector<size_t>{};

  sp._adjacency_list_vertical[0] = std::vector<size_t>{1, 4, 5};
  sp._adjacency_list_vertical[1] = std::vector<size_t>{};
  sp._adjacency_list_vertical[2] = std::vector<size_t>{1};
  sp._adjacency_list_vertical[3] = std::vector<size_t>{1, 2, 4, 5};
  sp._adjacency_list_vertical[4] = std::vector<size_t>{};
  sp._adjacency_list_vertical[5] = std::vector<size_t>{};

  sp._get_topology_order(is_horizontal);

  sp._get_longest_path(is_horizontal);
 
  return sp._modules;
}


std::vector<size_t> SequencePairTester::get_in_degree(
  const bool is_horizontal) {

  if (is_horizontal) {
    return sp._in_degree_horizontal;
  }
  else {
    return sp._in_degree_vertical;
  }
}


}
