#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest.h>
#include "../src/sp_tester.hpp"


/*
TEST_CASE("rotate_module" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");
  
  tester.generate_initial_pair();
   
  std::vector<sp::Node> old_modules = tester.get_modules();

  for (auto m : old_modules) {
    std::cout << "old module " << m.id << " has width " << m.width << " and height " << m.height << '\n'; 
  }
  
  std::cout << '\n';

  tester.rotate_module();

  std::vector<sp::Node> new_modules = tester.get_modules();

 
  for (auto m : new_modules) {
    std::cout << "new module " << m.id << " has width " << m.width << " and height " << m.height << '\n'; 
  }

  for (size_t i = 0; i < new_modules.size(); ++i) {
    if (new_modules[i].width != old_modules[i].width) {
      REQUIRE(new_modules[i].width == old_modules[i].height);
      REQUIRE(new_modules[i].height == old_modules[i].width);
    }

    if (new_modules[i].height != old_modules[i].height) {
      REQUIRE(new_modules[i].width == old_modules[i].height);
      REQUIRE(new_modules[i].height == old_modules[i].width);
    }
  }
}
*/
/*
TEST_CASE("swap_nodes_in_one_sequence" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");
  
  tester.generate_initial_pair();
   
  std::vector<sp::Node> old_modules = tester.get_modules();

  std::list<std::vector<int>> old_sequences = tester.get_sequences();

  SUBCASE("nodes_in_positive_sequence") {
    // swap two nodes in positve sequence
    tester.swap_two_nodes_one_sequence(old_sequences.front());

    std::vector<sp::Node> new_modules = tester.get_modules();

    std::list<std::vector<int>> new_sequences = tester.get_sequences();

    for (size_t i = 0; i < new_modules.size(); ++i) {
      REQUIRE(new_modules[i].id == old_modules[i].id);
      REQUIRE(new_modules[i].llx == old_modules[i].llx);
      REQUIRE(new_modules[i].lly == old_modules[i].lly);
      REQUIRE(new_modules[i].width == old_modules[i].width);
      REQUIRE(new_modules[i].height == old_modules[i].height);
    }

    size_t idx1 = new_sequences.front().size();
    size_t idx2 = new_sequences.front().size();

    for (size_t i = 0; i < new_sequences.front().size(); ++i) {
      REQUIRE(new_sequences.back()[i] == old_sequences.back()[i]);

      if (new_sequences.front()[i] != old_sequences.front()[i]) {
        if (idx1 == new_sequences.front().size()) {
          idx1 = i; 
        }
        else {
          idx2 = i;
        }
      }
      else {
        REQUIRE(new_sequences.front()[i] == old_sequences.front()[i]);
      }
    }

    REQUIRE(new_sequences.front()[idx1] == old_sequences.front()[idx2]);
    REQUIRE(new_sequences.front()[idx2] == old_sequences.front()[idx1]);
  }

  SUBCASE("nodes_in_negative_sequence") {
    // swap two nodes in negatve sequence
    tester.swap_two_nodes_one_sequence(old_sequences.back());

    std::vector<sp::Node> new_modules = tester.get_modules();

    std::list<std::vector<int>> new_sequences = tester.get_sequences();

    for (size_t i = 0; i < new_modules.size(); ++i) {
      REQUIRE(new_modules[i].id == old_modules[i].id);
      REQUIRE(new_modules[i].llx == old_modules[i].llx);
      REQUIRE(new_modules[i].lly == old_modules[i].lly);
      REQUIRE(new_modules[i].width == old_modules[i].width);
      REQUIRE(new_modules[i].height == old_modules[i].height);
    }

    size_t idx1 = new_sequences.back().size();
    size_t idx2 = new_sequences.back().size();

    for (size_t i = 0; i < new_sequences.back().size(); ++i) {
      REQUIRE(new_sequences.front()[i] == old_sequences.front()[i]);

      if (new_sequences.back()[i] != old_sequences.back()[i]) {
        if (idx1 == new_sequences.back().size()) {
          idx1 = i; 
        }
        else {
          idx2 = i;
        }
      }
      else {
        REQUIRE(new_sequences.back()[i] == old_sequences.back()[i]);
      }
    }

    REQUIRE(new_sequences.back()[idx1] == old_sequences.back()[idx2]);
    REQUIRE(new_sequences.back()[idx2] == old_sequences.back()[idx1]);
  }
}
*/


/*
TEST_CASE("swap_nodes_in_two_sequences" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");
  
  tester.generate_initial_pair();
   
  std::vector<sp::Node> old_modules = tester.get_modules();

  std::list<std::vector<int>> old_sequences = tester.get_sequences();

  tester.swap_two_nodes_two_sequences(old_sequences.front(), 
                                     old_sequences.back());

  std::vector<sp::Node> new_modules = tester.get_modules();

  std::list<std::vector<int>> new_sequences = tester.get_sequences();

  // new and old modules should be the same configurations
  for (size_t i = 0; i < new_modules.size(); ++i) {
    REQUIRE(new_modules[i].id == old_modules[i].id);
    REQUIRE(new_modules[i].llx == old_modules[i].llx);
    REQUIRE(new_modules[i].lly == old_modules[i].lly);
    REQUIRE(new_modules[i].width == old_modules[i].width);
    REQUIRE(new_modules[i].height == old_modules[i].height);
  }

  // test the positive sequence
  size_t idx1 = new_sequences.front().size();
  size_t idx2 = new_sequences.front().size();

  for (size_t i = 0; i < new_sequences.front().size(); ++i) {
    if (new_sequences.front()[i] != old_sequences.front()[i]) {
      if (idx1 == new_sequences.front().size()) {
        idx1 = i; 
      }
      else {
        idx2 = i;
      }
    }

    else {
      REQUIRE(new_sequences.front()[i] == old_sequences.front()[i]);
    }
  }

  REQUIRE(new_sequences.front()[idx1] == old_sequences.front()[idx2]);
  REQUIRE(new_sequences.front()[idx2] == old_sequences.front()[idx1]);
  
  
  // test the negative sequence
  idx1 = new_sequences.back().size();
  idx2 = new_sequences.back().size();

  for (size_t i = 0; i < new_sequences.back().size(); ++i) {
    if (new_sequences.back()[i] != old_sequences.back()[i]) {
      if (idx1 == new_sequences.back().size()) {
        idx1 = i; 
      }
      else {
        idx2 = i;
      }
    }

    else {
      REQUIRE(new_sequences.back()[i] == old_sequences.back()[i]);
    }
  }

  REQUIRE(new_sequences.back()[idx1] == old_sequences.back()[idx2]);
  REQUIRE(new_sequences.back()[idx2] == old_sequences.back()[idx1]);
}
*/


/*
TEST_CASE("generate_adjacency_list" * doctest::timeout(300)) {

  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");

  std::vector<int> positive_sequence{1, 2, 4, 5, 3, 0};
  std::vector<int> negative_sequence{3, 2, 0, 1, 4, 5};

  std::vector<std::vector<size_t>> adjacency_list;

  SUBCASE("is_horizontal") {
    tester.generate_adjacency_list(
      positive_sequence, negative_sequence, true);

    adjacency_list = tester.get_adjacency_list();

    for (size_t i = 0; i < adjacency_list.size(); ++i) {
      for (size_t j = 0; j < adjacency_list[i].size(); ++j) {
        std::cout << "adjacency_list[" << i << "][" << j << "] = " 
                  << adjacency_list[i][j] << '\n';
      }
    }

    REQUIRE(adjacency_list[0].size() == 0);
    REQUIRE(adjacency_list[1].size() == 2);
    REQUIRE(adjacency_list[2].size() == 3);
    REQUIRE(adjacency_list[3].size() == 1);
    REQUIRE(adjacency_list[4].size() == 1);
    REQUIRE(adjacency_list[5].size() == 0);

    REQUIRE(adjacency_list[1][0] == 4);
    REQUIRE(adjacency_list[1][1] == 5);
    
    REQUIRE(adjacency_list[2][0] == 0);
    REQUIRE(adjacency_list[2][1] == 4);
    REQUIRE(adjacency_list[2][2] == 5);

    REQUIRE(adjacency_list[3][0] == 0);
    
    REQUIRE(adjacency_list[4][0] == 5);
  }

  SUBCASE("is_vertical") {
    tester.generate_adjacency_list(
      positive_sequence, negative_sequence, false);

    adjacency_list = tester.get_adjacency_list();

    REQUIRE(adjacency_list[0].size() == 3);
    REQUIRE(adjacency_list[1].size() == 0);
    REQUIRE(adjacency_list[2].size() == 1);
    REQUIRE(adjacency_list[3].size() == 4);
    REQUIRE(adjacency_list[4].size() == 0);
    REQUIRE(adjacency_list[5].size() == 0);

    REQUIRE(adjacency_list[0][0] == 1);
    REQUIRE(adjacency_list[0][1] == 4);
    REQUIRE(adjacency_list[0][2] == 5);
    
    REQUIRE(adjacency_list[2][0] == 1);

    REQUIRE(adjacency_list[3][0] == 1);
    REQUIRE(adjacency_list[3][1] == 2);
    REQUIRE(adjacency_list[3][2] == 4);
    REQUIRE(adjacency_list[3][3] == 5);
  }
}
*/

/*
TEST_CASE("get_topology_order" * doctest::timeout(300)) {

  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");

  std::vector<int> positive_sequence{1, 2, 4, 5, 3, 0};
  std::vector<int> negative_sequence{3, 2, 0, 1, 4, 5};

  std::vector<int> topology_order;
  
  SUBCASE("is_horizontal") {
    tester.generate_adjacency_list(
      positive_sequence, negative_sequence, true);

    topology_order = tester.get_topology_order();
  
    size_t idx0, idx1, idx2, idx3, idx4, idx5; 
    for (size_t i = 0; i < topology_order.size(); ++i) {
      if      (topology_order[i] == 0) { idx0 = i; }    
      else if (topology_order[i] == 1) { idx1 = i; }    
      else if (topology_order[i] == 2) { idx2 = i; }    
      else if (topology_order[i] == 3) { idx3 = i; }    
      else if (topology_order[i] == 4) { idx4 = i; }    
      else                             { idx5 = i; }    
    }
     
    REQUIRE(topology_order.size() == 6);
    
    REQUIRE(idx1 < idx4);
    REQUIRE(idx1 < idx5);
    REQUIRE(idx2 < idx4);
    REQUIRE(idx2 < idx5);
    REQUIRE(idx2 < idx0);
    REQUIRE(idx3 < idx0);
    REQUIRE(idx4 < idx5);
  }

  SUBCASE("is_vertical") {

    tester.generate_adjacency_list(
      positive_sequence, negative_sequence, false);

    topology_order = tester.get_topology_order();
    
    size_t idx0, idx1, idx2, idx3, idx4, idx5; 
    for (size_t i = 0; i < topology_order.size(); ++i) {
      if      (topology_order[i] == 0) { idx0 = i; }    
      else if (topology_order[i] == 1) { idx1 = i; }    
      else if (topology_order[i] == 2) { idx2 = i; }    
      else if (topology_order[i] == 3) { idx3 = i; }    
      else if (topology_order[i] == 4) { idx4 = i; }    
      else                             { idx5 = i; }    
    }

    REQUIRE(topology_order.size() == 6);

    REQUIRE(idx3 < idx1);
    REQUIRE(idx3 < idx5);
    REQUIRE(idx3 < idx4);
    REQUIRE(idx3 < idx2);
    REQUIRE(idx2 < idx1);
    REQUIRE(idx0 < idx4);
    REQUIRE(idx0 < idx5);
    REQUIRE(idx0 < idx1);
  }
}
*/
/*
TEST_CASE("get_longest_path" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;
  
  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");

  tester.generate_initial_pair();

  std::vector<int> longest_path;
  std::vector<int> topology_order;

  std::vector<int> positive_sequence{1, 2, 4, 5, 3, 0};
  std::vector<int> negative_sequence{3, 2, 0, 1, 4, 5};
 
  SUBCASE("is_horizontal") {
    tester.generate_adjacency_list(
      positive_sequence, negative_sequence, true);

    topology_order = tester.get_topology_order();

    longest_path = tester.get_longest_path(true);
    
    std::cout << "horizontal longest path\n";
    for (auto l : longest_path)  std::cout << l << ' ';
    std::cout << '\n';
    REQUIRE(longest_path.size() == 6);

    REQUIRE(longest_path[0] == 696);
    REQUIRE(longest_path[1] == 0);
    REQUIRE(longest_path[2] == 0);
    REQUIRE(longest_path[3] == 0);
    REQUIRE(longest_path[4] == 332);
    REQUIRE(longest_path[5] == 643);
  }

  SUBCASE("is_vertical") {
    tester.generate_adjacency_list(
      positive_sequence, negative_sequence, false);

    topology_order = tester.get_topology_order();

    longest_path = tester.get_longest_path(false);
    
    std::cout << "vertical longest path\n";
    for (auto l : longest_path)  std::cout << l << ' ';
    REQUIRE(longest_path.size() == 6);

    REQUIRE(longest_path[0] == 0);
    REQUIRE(longest_path[1] == 426);
    REQUIRE(longest_path[2] == 172);
    REQUIRE(longest_path[3] == 0);
    REQUIRE(longest_path[4] == 172);
    REQUIRE(longest_path[5] == 172);
  }
}
*/

