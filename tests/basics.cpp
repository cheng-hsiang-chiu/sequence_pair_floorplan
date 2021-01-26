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


TEST_CASE("get_longest_path" * doctest::timeout(300)) {
  
  sp::SequencePairTester tester;
  
  std::vector<sp::Node> new_modules;
  
  std::vector<sp::Node> old_modules;

  SUBCASE("is_horizontal") {

    tester.open("../../circuits/circuit2.txt");
    //tester.open("../circuits/circuit2.txt");

    tester.generate_initial_pair();

    old_modules = tester.get_modules(); 
    
    new_modules = tester.get_longest_path(true);
    
    REQUIRE(new_modules.size() == 6);
    REQUIRE(old_modules.size() == 6);

    REQUIRE(new_modules[0].llx == 696);
    REQUIRE(new_modules[1].llx == 0);
    REQUIRE(new_modules[2].llx == 0);
    REQUIRE(new_modules[3].llx == 0);
    REQUIRE(new_modules[4].llx == 332);
    REQUIRE(new_modules[5].llx == 643);

    REQUIRE(new_modules[0].lly == old_modules[0].lly);
    REQUIRE(new_modules[1].lly == old_modules[1].lly);
    REQUIRE(new_modules[2].lly == old_modules[2].lly);
    REQUIRE(new_modules[3].lly == old_modules[3].lly);
    REQUIRE(new_modules[4].lly == old_modules[4].lly);
    REQUIRE(new_modules[5].lly == old_modules[5].lly);
  }

  SUBCASE("is_vertical") {
    
    tester.open("../../circuits/circuit2.txt");
    //tester.open("../circuits/circuit2.txt");

    tester.generate_initial_pair();
   
    old_modules = tester.get_modules();
     
    new_modules = tester.get_longest_path(false);
    
    REQUIRE(new_modules.size() == 6);
    REQUIRE(old_modules.size() == 6);
    
    REQUIRE(new_modules[0].lly == 0);
    REQUIRE(new_modules[1].lly == 426);
    REQUIRE(new_modules[2].lly == 172);
    REQUIRE(new_modules[3].lly == 0);
    REQUIRE(new_modules[4].lly == 172);
    REQUIRE(new_modules[5].lly == 172);
    
    REQUIRE(new_modules[0].llx == old_modules[0].lly);
    REQUIRE(new_modules[1].llx == old_modules[1].lly);
    REQUIRE(new_modules[2].llx == old_modules[2].lly);
    REQUIRE(new_modules[3].llx == old_modules[3].lly);
    REQUIRE(new_modules[4].llx == old_modules[4].lly);
    REQUIRE(new_modules[5].llx == old_modules[5].lly);
  }
}



TEST_CASE("generate_adjacency_list" * doctest::timeout(300)) {

  sp::SequencePairTester tester;

  tester.open("../../circuits/circuit2.txt");
  //tester.open("../circuits/circuit2.txt");

  std::vector<size_t> positive_sequence{1, 2, 4, 5, 3, 0};
  std::vector<size_t> negative_sequence{3, 2, 0, 1, 4, 5};

  std::vector<std::vector<size_t>> adjacency_list;
  std::vector<size_t> in_degree;

  SUBCASE("is_horizontal") {
    tester.generate_initial_pair();
    
    adjacency_list = tester.generate_adjacency_list(
      positive_sequence, negative_sequence, true);

    in_degree = tester.get_in_degree(true);

    /*
    for (size_t i = 0; i < adjacency_list.size(); ++i) {
      for (size_t j = 0; j < adjacency_list[i].size(); ++j) {
        std::cout << "adjacency_list[" << i << "][" << j << "] = " 
                  << adjacency_list[i][j] << '\n';
      }
    }
    */

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

    REQUIRE(in_degree.size() == 6);
    REQUIRE(in_degree[0] == 2);
    REQUIRE(in_degree[1] == 0);
    REQUIRE(in_degree[2] == 0);
    REQUIRE(in_degree[3] == 0);
    REQUIRE(in_degree[4] == 2);
    REQUIRE(in_degree[5] == 3);
  }

  SUBCASE("is_vertical") {
    tester.generate_initial_pair();
    
    adjacency_list = tester.generate_adjacency_list(
      positive_sequence, negative_sequence, false);

    in_degree = tester.get_in_degree(false);

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
    
    REQUIRE(in_degree.size() == 6);
    REQUIRE(in_degree[0] == 0);
    REQUIRE(in_degree[1] == 3);
    REQUIRE(in_degree[2] == 1);
    REQUIRE(in_degree[3] == 0);
    REQUIRE(in_degree[4] == 2);
    REQUIRE(in_degree[5] == 2);
  }
}

