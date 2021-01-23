#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest.h>
#include "../src/sp_tester.hpp"



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


TEST_CASE("swap_nodes_in_proposed_positive_sequence" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");
  
  tester.generate_initial_pair();
   
  std::vector<sp::Node> old_modules = tester.get_modules();

  std::list<std::vector<size_t>> old_sequences = tester.get_sequences();

  tester.swap_two_nodes_positive_sequence();

  std::list<std::vector<size_t>> new_sequences = tester.get_sequences();

  std::list<std::vector<size_t>>::iterator it = old_sequences.begin();

  std::vector<size_t> positive_sequence_curr_old = *it;
  std::vector<size_t> negative_sequence_curr_old = *(++it);
  std::vector<size_t> positive_sequence_prop_old = *(++it);
  std::vector<size_t> negative_sequence_prop_old = *(++it);
  std::vector<size_t> positive_sequence_best_old = *(++it);
  std::vector<size_t> negative_sequence_best_old = *(++it);
 
  it = new_sequences.begin(); 
  std::vector<size_t> positive_sequence_curr_new = *it;
  std::vector<size_t> negative_sequence_curr_new = *(++it);
  std::vector<size_t> positive_sequence_prop_new = *(++it);
  std::vector<size_t> negative_sequence_prop_new = *(++it);
  std::vector<size_t> positive_sequence_best_new = *(++it);
  std::vector<size_t> negative_sequence_best_new = *(++it);

  size_t num_modules = positive_sequence_curr_old.size();

  size_t count = 0;
  
  int idx1 = -1, idx2 = -1;

  for (size_t i = 0; i < num_modules; ++i) {
    REQUIRE(positive_sequence_curr_old[i] == positive_sequence_curr_new[i]);
    REQUIRE(negative_sequence_curr_old[i] == negative_sequence_curr_new[i]);
    REQUIRE(negative_sequence_prop_old[i] == negative_sequence_prop_new[i]);
    REQUIRE(positive_sequence_best_old[i] == positive_sequence_best_new[i]);
    REQUIRE(negative_sequence_best_old[i] == negative_sequence_best_new[i]);

    if (positive_sequence_prop_old[i] != positive_sequence_prop_new[i]) {
      ++count;  
      if (idx1 == -1) {
        idx1 = i;
      }
      else {
        idx2 = i;
      }
    }
  }

  REQUIRE(count == 2);

  REQUIRE(positive_sequence_prop_old[idx1] == positive_sequence_prop_new[idx2]);
  REQUIRE(positive_sequence_prop_old[idx2] == positive_sequence_prop_new[idx1]);
  
  REQUIRE(positive_sequence_curr_old.size() == 6);
  REQUIRE(positive_sequence_prop_old.size() == 6);
  REQUIRE(positive_sequence_best_old.size() == 6);
  REQUIRE(positive_sequence_curr_new.size() == 6);
  REQUIRE(positive_sequence_prop_new.size() == 6);
  REQUIRE(positive_sequence_best_new.size() == 6);
  REQUIRE(negative_sequence_curr_old.size() == 6);
  REQUIRE(negative_sequence_prop_old.size() == 6);
  REQUIRE(negative_sequence_best_old.size() == 6);
  REQUIRE(negative_sequence_curr_new.size() == 6);
  REQUIRE(negative_sequence_prop_new.size() == 6);
  REQUIRE(negative_sequence_best_new.size() == 6);
}


TEST_CASE("swap_nodes_in_proposed_negative_sequence" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");  
  //tester.open("../circuits/circuit2.txt");
 
  tester.generate_initial_pair();
   
  std::vector<sp::Node> old_modules = tester.get_modules();

  std::list<std::vector<size_t>> old_sequences = tester.get_sequences();

  tester.swap_two_nodes_negative_sequence();

  std::list<std::vector<size_t>> new_sequences = tester.get_sequences();

  std::list<std::vector<size_t>>::iterator it = old_sequences.begin();

  std::vector<size_t> positive_sequence_curr_old = *it;
  std::vector<size_t> negative_sequence_curr_old = *(++it);
  std::vector<size_t> positive_sequence_prop_old = *(++it);
  std::vector<size_t> negative_sequence_prop_old = *(++it);
  std::vector<size_t> positive_sequence_best_old = *(++it);
  std::vector<size_t> negative_sequence_best_old = *(++it);
 
  it = new_sequences.begin(); 
  std::vector<size_t> positive_sequence_curr_new = *it;
  std::vector<size_t> negative_sequence_curr_new = *(++it);
  std::vector<size_t> positive_sequence_prop_new = *(++it);
  std::vector<size_t> negative_sequence_prop_new = *(++it);
  std::vector<size_t> positive_sequence_best_new = *(++it);
  std::vector<size_t> negative_sequence_best_new = *(++it);

  size_t num_modules = positive_sequence_curr_old.size();

  size_t count = 0;
  
  int idx1 = -1, idx2 = -1;

  for (size_t i = 0; i < num_modules; ++i) {
    REQUIRE(positive_sequence_curr_old[i] == positive_sequence_curr_new[i]);
    REQUIRE(negative_sequence_curr_old[i] == negative_sequence_curr_new[i]);
    REQUIRE(positive_sequence_prop_old[i] == positive_sequence_prop_new[i]);
    REQUIRE(positive_sequence_best_old[i] == positive_sequence_best_new[i]);
    REQUIRE(negative_sequence_best_old[i] == negative_sequence_best_new[i]);

    if (negative_sequence_prop_old[i] != negative_sequence_prop_new[i]) {
      ++count;  
      if (idx1 == -1) {
        idx1 = i;
      }
      else {
        idx2 = i;
      }
    }
  }

  REQUIRE(count == 2);

  REQUIRE(negative_sequence_prop_old[idx1] == negative_sequence_prop_new[idx2]);
  REQUIRE(negative_sequence_prop_old[idx2] == negative_sequence_prop_new[idx1]);
  
  REQUIRE(positive_sequence_curr_old.size() == 6);
  REQUIRE(positive_sequence_prop_old.size() == 6);
  REQUIRE(positive_sequence_best_old.size() == 6);
  REQUIRE(positive_sequence_curr_new.size() == 6);
  REQUIRE(positive_sequence_prop_new.size() == 6);
  REQUIRE(positive_sequence_best_new.size() == 6);
  REQUIRE(negative_sequence_curr_old.size() == 6);
  REQUIRE(negative_sequence_prop_old.size() == 6);
  REQUIRE(negative_sequence_best_old.size() == 6);
  REQUIRE(negative_sequence_curr_new.size() == 6);
  REQUIRE(negative_sequence_prop_new.size() == 6);
  REQUIRE(negative_sequence_best_new.size() == 6);
}


TEST_CASE("swap_nodes_in_both_proposed_sequences" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");  
  //tester.open("../circuits/circuit2.txt");
 
  tester.generate_initial_pair();
   
  std::vector<sp::Node> old_modules = tester.get_modules();

  std::list<std::vector<size_t>> old_sequences = tester.get_sequences();

  tester.swap_two_nodes_two_sequences();

  std::list<std::vector<size_t>> new_sequences = tester.get_sequences();

  std::list<std::vector<size_t>>::iterator it = old_sequences.begin();

  std::vector<size_t> positive_sequence_curr_old = *it;
  std::vector<size_t> negative_sequence_curr_old = *(++it);
  std::vector<size_t> positive_sequence_prop_old = *(++it);
  std::vector<size_t> negative_sequence_prop_old = *(++it);
  std::vector<size_t> positive_sequence_best_old = *(++it);
  std::vector<size_t> negative_sequence_best_old = *(++it);
 
  it = new_sequences.begin(); 
  std::vector<size_t> positive_sequence_curr_new = *it;
  std::vector<size_t> negative_sequence_curr_new = *(++it);
  std::vector<size_t> positive_sequence_prop_new = *(++it);
  std::vector<size_t> negative_sequence_prop_new = *(++it);
  std::vector<size_t> positive_sequence_best_new = *(++it);
  std::vector<size_t> negative_sequence_best_new = *(++it);

  size_t num_modules = positive_sequence_curr_old.size();

  size_t pcount = 0, ncount = 0;
  
  int pidx1 = -1, pidx2 = -1, nidx1 = -1, nidx2 = -1;

  for (size_t i = 0; i < num_modules; ++i) {
    REQUIRE(positive_sequence_curr_old[i] == positive_sequence_curr_new[i]);
    REQUIRE(negative_sequence_curr_old[i] == negative_sequence_curr_new[i]);
    REQUIRE(positive_sequence_best_old[i] == positive_sequence_best_new[i]);
    REQUIRE(negative_sequence_best_old[i] == negative_sequence_best_new[i]);

    if (negative_sequence_prop_old[i] != negative_sequence_prop_new[i]) {
      ++ncount;  
      if (nidx1 == -1) {
        nidx1 = i;
      }
      else {
        nidx2 = i;
      }
    }

    if (positive_sequence_prop_old[i] != positive_sequence_prop_new[i]) {
      ++pcount;  
      if (pidx1 == -1) {
        pidx1 = i;
      }
      else {
        pidx2 = i;
      }
    }
  }

  REQUIRE(pcount == 2);
  REQUIRE(ncount == 2);

  REQUIRE(negative_sequence_prop_old[nidx1] == negative_sequence_prop_new[nidx2]);
  REQUIRE(negative_sequence_prop_old[nidx2] == negative_sequence_prop_new[nidx1]);
  
  REQUIRE(positive_sequence_prop_old[pidx1] == positive_sequence_prop_new[pidx2]);
  REQUIRE(positive_sequence_prop_old[pidx2] == positive_sequence_prop_new[pidx1]);
  
  if (positive_sequence_prop_old[pidx1] == negative_sequence_prop_old[nidx1]) {
    REQUIRE(positive_sequence_prop_old[pidx1] == negative_sequence_prop_old[nidx1]);
    REQUIRE(positive_sequence_prop_old[pidx2] == negative_sequence_prop_old[nidx2]);
  }
  else {
    REQUIRE(positive_sequence_prop_old[pidx1] == negative_sequence_prop_old[nidx2]);
    REQUIRE(positive_sequence_prop_old[pidx2] == negative_sequence_prop_old[nidx1]);
  }

  REQUIRE(positive_sequence_curr_old.size() == 6);
  REQUIRE(positive_sequence_prop_old.size() == 6);
  REQUIRE(positive_sequence_best_old.size() == 6);
  REQUIRE(positive_sequence_curr_new.size() == 6);
  REQUIRE(positive_sequence_prop_new.size() == 6);
  REQUIRE(positive_sequence_best_new.size() == 6);
  REQUIRE(negative_sequence_curr_old.size() == 6);
  REQUIRE(negative_sequence_prop_old.size() == 6);
  REQUIRE(negative_sequence_best_old.size() == 6);
  REQUIRE(negative_sequence_curr_new.size() == 6);
  REQUIRE(negative_sequence_prop_new.size() == 6);
  REQUIRE(negative_sequence_best_new.size() == 6);
}



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


TEST_CASE("get_topology_order" * doctest::timeout(300)) {

  sp::SequencePairTester tester;

  std::vector<size_t> topology_order;
  
  SUBCASE("is_horizontal") {

    tester.open("../../circuits/circuit2.txt");
    
    tester.generate_initial_pair();
    
    topology_order = tester.get_topology_order(true);
  
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

    tester.open("../../circuits/circuit2.txt");
    
    tester.generate_initial_pair();
    
    topology_order = tester.get_topology_order(false);
    
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

