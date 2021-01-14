#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest.h>
#include "../src/sp_tester.hpp"



TEST_CASE("rotate_module" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("/home/chiu/sequence_pair/circuits/circuit2.txt");
  
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


TEST_CASE("swap_nodes_in_one_sequence" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("/home/chiu/sequence_pair/circuits/circuit2.txt");
  
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


TEST_CASE("swap_nodes_in_two_sequences" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("/home/chiu/sequence_pair/circuits/circuit2.txt");
  
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


TEST_CASE("generate_dag" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("/home/chiu/sequence_pair/circuits/circuit2.txt");
  
  tester.generate_initial_pair();
   
  std::vector<sp::Node> old_modules = tester.get_modules();

  std::list<std::vector<int>> old_sequences = tester.get_sequences();

  SUBCASE("is_horizontal") {
    graph::DAG dag =  
      tester.generate_dag(old_sequences.front(), old_sequences.back(), true); 
    
    std::vector<int> topological_order = dag.get_topology_order();
     
    size_t idx0, idx1, idx2, idx3, idx4, idx5; 
    for (size_t i = 0; i < topological_order.size(); ++i) {
      if      (topological_order[i] == 0) { idx0 = i; }    
      else if (topological_order[i] == 1) { idx1 = i; }    
      else if (topological_order[i] == 2) { idx2 = i; }    
      else if (topological_order[i] == 3) { idx3 = i; }    
      else if (topological_order[i] == 4) { idx4 = i; }    
      else                                { idx5 = i; }    
    }
     
    REQUIRE(idx1 < idx4);
    REQUIRE(idx1 < idx5);
    REQUIRE(idx2 < idx4);
    REQUIRE(idx2 < idx5);
    REQUIRE(idx2 < idx0);
    REQUIRE(idx3 < idx0);
    REQUIRE(idx4 < idx5);
  }

  SUBCASE("is_not_horizontal") {
    graph::DAG dag =  
      tester.generate_dag(old_sequences.front(), old_sequences.back(), false); 
    
    std::vector<int> topological_order = dag.get_topology_order();
     
    size_t idx0, idx1, idx2, idx3, idx4, idx5; 
    for (size_t i = 0; i < topological_order.size(); ++i) {
      if      (topological_order[i] == 0) { idx0 = i; }    
      else if (topological_order[i] == 1) { idx1 = i; }    
      else if (topological_order[i] == 2) { idx2 = i; }    
      else if (topological_order[i] == 3) { idx3 = i; }    
      else if (topological_order[i] == 4) { idx4 = i; }    
      else                                { idx5 = i; }    
    }

    REQUIRE(idx3 < idx1);
    REQUIRE(idx3 < idx5);
    REQUIRE(idx3 < idx4);
    REQUIRE(idx3 < idx2);
    REQUIRE(idx2 < idx1);
    REQUIRE(idx0 < idx4);
    REQUIRE(idx0 < idx5);
    REQUIRE(idx0 < idx1);
    
  }
  /*
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
  */
}
