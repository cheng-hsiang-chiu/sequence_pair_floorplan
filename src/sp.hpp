#include <iostream>
#include <vector>
#include <random>
#include <map>
#include <chrono>
#include <fstream>
#include <queue>
#include <utility>
#include <algorithm>
#include <iterator>
#include <cassert>
//#include "../lib/graph.hpp"


#define SP_FROZEN_TEMPERATURE 0.1
#define SP_MAX_ITERATIONS_PER_TEMPERATURE 1000
#define SP_RANDOM_MOVES 60
#define SP_P 0.99
#define SP_T0 -1


namespace sp {
 
 
struct Node{
  size_t id;
  size_t width;
  size_t height;
  size_t llx;
  size_t lly;  

  Node(size_t n_id, size_t n_width, size_t n_height,
       size_t n_llx, size_t n_lly)
    : id(n_id), width(n_width), height(n_height), llx(n_llx), lly(n_lly) {} 
};
 
  
class SequencePair{
  friend class SequencePairTester;

  public:
    SequencePair() = default;

    void optimize();
        
    void open(const std::string input_file);

    void dump(std::ostream& os) const;
    
    void dump_json(std::string output_file) const;

  private:
    size_t _num_modules = 0;
     
    std::string _input_file;

    std::vector<Node> _modules;

    std::vector<Node> _modules_best;

    std::vector<size_t> _positive_sequence_curr;

    std::vector<size_t> _negative_sequence_curr;

    std::vector<size_t> _positive_sequence_prop;

    std::vector<size_t> _negative_sequence_prop;
    
    std::vector<size_t> _positive_sequence_best;

    std::vector<size_t> _negative_sequence_best;
    
    std::random_device _rd;

    std::mt19937 _gen{_rd()};
    
    size_t _urx = 0;

    size_t _ury = 0;

    std::vector<size_t> _topology_order;

    //graph::DAG _dag;

    std::vector<size_t> _cost;

    //std::vector<graph::Node> _all_nodes;
    
    //std::vector<int> _longest_path;
    
    std::map<size_t, size_t> _mapping;

    std::vector<std::vector<size_t>> _adjacency_list_horizontal;

    std::vector<std::vector<size_t>> _adjacency_list_vertical;
    
    std::vector<size_t> _in_degree_horizontal;

    std::vector<size_t> _in_degree_vertical;
    
    std::vector<size_t> _in_degree;
    
    std::queue<size_t> _queue;

    std::vector<size_t> _seq;
    
    size_t _changed_id1;

    size_t _changed_id2;

    std::vector<bool> _is_exist;
    //std::map<size_t, std::pair<size_t, size_t>> _pn_mapping;


     
    void _generate_initial_pair();
  
    double _calculate_initial_temperature();

    void _simulated_annealing(const double initial_temperature);

    size_t _generate_neighbors();

    size_t _pack(const std::vector<size_t>& positive_sequence,
                 const std::vector<size_t>& negative_sequence,
                 const size_t operation);

    void _pack_helper(const std::vector<size_t>& positive_sequence,
                      const std::vector<size_t>& negative_sequence, 
                      const bool is_horizontal,
                      const size_t operation);

    void _dump_cost(std::string output_file) const;

    void _rotate_module();

    void _swap_two_nodes_positive_sequence();
    
    void _swap_two_nodes_negative_sequence();

    void _swap_two_nodes_two_sequences();
    
    /*
    graph::DAG _generate_dag(
      const std::vector<int>& positive_sequence,
      const std::vector<int>& negative_sequence,
      const bool is_horizontal) const;
    */
    void _generate_adjacency_list(
      const std::vector<size_t>& positive_sequence,
      const std::vector<size_t>& negative_sequence,
      const bool is_horizontal); 
    
    void _get_topology_order(const bool is_horizontal);

    void _get_longest_path(const bool is_horizontal);
};


// read in the module configurations
void SequencePair::open(const std::string input_file) {

  _input_file = input_file;

  std::ifstream infile(input_file);
   
  if (!infile) {
    std::cerr << "File could not be opened!!\n";
    std::exit(EXIT_FAILURE);
  }

  size_t width, height, id, index = 0;
  
  infile >> _num_modules;

  _modules.reserve(_num_modules);

  while (infile >> id >> width >> height) {
    //Node node;
    //node.id = id;
    //node.width = width;
    //node.height = height; 

    //_modules.push_back(node);
    _modules.emplace_back(id, width, height, 0, 0);
    _mapping.emplace(id, index++);
  }

  std::cout << "Modules are successfully opened!!\n";
}


// dump floorplan to console
void SequencePair::dump(std::ostream& os) const {

  for (size_t i = 0; i < _num_modules; ++i) {
    os << _modules[i].id     << ' '
       << _modules[i].llx    << ' '
       << _modules[i].lly    << ' '
       << _modules[i].width  << ' '
       << _modules[i].height << '\n';
  }
}


// dump floorplan to a file with a json extesion
// not yet finished
void SequencePair::dump_json(std::string output_file) const {
 
  _dump_cost(output_file);
   
  if (output_file.rfind(".json") == std::string::npos) {
    output_file.append(".json");
  }
  
  std::ofstream outfile(output_file, std::ios::out);
 
  if (!outfile) {
    std::cerr << "File could not be opened for writing\n";
    std::exit(EXIT_FAILURE); 
  }

  // generate json output 
  outfile << "{\"circuit\":\""    << _input_file << "\""
          << ",\"block_number\":" << _num_modules
          << ",\"llx\":0"
          << ",\"lly\":0"
          << ",\"urx\":"  << _urx
          << ",\"ury\":"  << _ury
          << ",\"area\":" << (_urx * _ury)
          << ",\"coordinates\":"
          << "[";

  for (size_t i = 0; i < _num_modules; ++i) {
    outfile << "{\"idx\":"    << _modules[i].id
            << ",\"llx\":"    << _modules[i].llx
            << ",\"lly\":"    << _modules[i].lly
            << ",\"width\":"  << _modules[i].width
            << ",\"height\":" << _modules[i].height;
    
    if(i == _num_modules-1) {
      outfile << "}";
    }
    else {
      outfile << "},";
    }
  }

  outfile << "]}"; 
}


// dump the cost 
void SequencePair::_dump_cost(const std::string output_file) const {
  
  std::ofstream outfile(output_file+"_cost.txt", std::ios::out);
    
  if (!outfile) {
    std::cerr << "File could not be opened for writing\n";
    std::exit(EXIT_FAILURE); 
  }

  for (auto c : _cost) {
    outfile << c << '\n';
  }
}


// generate an initial pair
void SequencePair::_generate_initial_pair() {

  _adjacency_list_horizontal.resize(_num_modules);
  _adjacency_list_vertical.resize(_num_modules);
  
  for (size_t i = 0; i < _num_modules; ++i) {
    _adjacency_list_horizontal.resize(_num_modules);
    _adjacency_list_vertical.resize(_num_modules);
  }

  _in_degree_horizontal.resize(_num_modules);
  _in_degree_vertical.resize(_num_modules);
  _topology_order.resize(_num_modules);
  _is_exist.resize(_num_modules);

  _positive_sequence_curr.resize(_num_modules);
  _negative_sequence_curr.resize(_num_modules);
  _positive_sequence_prop.resize(_num_modules);
  _negative_sequence_prop.resize(_num_modules);
  _positive_sequence_best.resize(_num_modules);
  _negative_sequence_best.resize(_num_modules);

  for (size_t i = 0; i < _num_modules; ++i) {    
    _positive_sequence_curr.at(i) = _modules[i].id;
    _negative_sequence_curr.at(i) = _modules[i].id;
    //_pn_mapping[i] = std::make_pair(i, i);
  }

  _positive_sequence_prop = _positive_sequence_curr;
  _negative_sequence_prop = _negative_sequence_curr;
  _positive_sequence_best = _positive_sequence_curr;
  _negative_sequence_best = _negative_sequence_curr;
  
  /*
  _positive_sequence[0] = 1;
  _positive_sequence[1] = 2;
  _positive_sequence[2] = 4;
  _positive_sequence[3] = 5;
  _positive_sequence[4] = 3;
  _positive_sequence[5] = 0;

  _negative_sequence[0] = 3;
  _negative_sequence[1] = 2;
  _negative_sequence[2] = 0;
  _negative_sequence[3] = 1;
  _negative_sequence[4] = 4;
  _negative_sequence[5] = 5;
  */ 
}


// generate the optimized floorplan
void SequencePair::optimize() {

  _generate_initial_pair();

  auto tbeg = std::chrono::steady_clock::now();

  double initial_temperature = _calculate_initial_temperature();

  auto tend = std::chrono::steady_clock::now();

  std::cout << "initial temperature = " << initial_temperature << '\n';
  
  std::cout << "calculate initial temperature completed: " 
            << std::chrono::duration_cast<std::chrono::microseconds>(tend-tbeg).count()
            << " microseconds\n";  
            
  tbeg = std::chrono::steady_clock::now();
  
  _simulated_annealing(initial_temperature);

  tend = std::chrono::steady_clock::now();

  std::cout << "simulated annealing completed: " 
            << std::chrono::duration_cast<std::chrono::microseconds>(tend-tbeg).count()
            << " microseconds\n";  

  _modules = _modules_best;

  tbeg = std::chrono::steady_clock::now();
  
  size_t area = _pack(_positive_sequence_curr, _negative_sequence_curr, 1);
  
  tend = std::chrono::steady_clock::now();
  
  std::cout << "pack modules completed: " 
            << std::chrono::duration_cast<std::chrono::microseconds>(tend-tbeg).count()
            << " microseconds" << std::endl;  
}


// calculate initial temperature
double SequencePair::_calculate_initial_temperature() {
  
  size_t num_moves = 0;
  double total_area_change = 0.0;
  double delta_area, avg_area_change, init_temperature;
  
  //std::vector<int> positive_sequence_curr = _positive_sequence;
  //std::vector<int> negative_sequence_curr = _negative_sequence;
  //std::vector<int> positive_sequence_prop = _positive_sequence;
  //std::vector<int> negative_sequence_prop = _negative_sequence;

  /*
  for (auto p : positive_sequence_curr)  std::cout << p << ' ';
  std::cout << '\n';
  for (auto n : negative_sequence_curr)  std::cout << n << ' ';
  std::cout << '\n';
  */
  size_t area_curr = _pack(_positive_sequence_curr, _negative_sequence_curr, 1);
  size_t area_prop;

  while (num_moves < SP_RANDOM_MOVES) {
    //std::cout << "before generate neighbors\n";
    //for (auto p : positive_sequence_prop)  std::cout << p << ' ';
    //std::cout << '\n';
    //for (auto n : negative_sequence_prop)  std::cout << n << ' ';
    //std::cout << '\n';
    size_t operation = _generate_neighbors();
    //std::cout << "after generate neighbors with operation = " << operation << "\n";
    //for (auto p : positive_sequence_prop)  std::cout << p << ' ';
    //std::cout << '\n';
    //for (auto n : negative_sequence_prop)  std::cout << n << ' ';
    //std::cout << '\n';
    
    area_prop = _pack(_positive_sequence_prop, _negative_sequence_prop, operation);

    delta_area = area_prop > area_curr ? 
                 (area_prop - area_curr) : 
                 (area_curr - area_prop); 

    total_area_change += delta_area;
    
    ++num_moves;
    
    //positive_sequence_curr = positive_sequence_prop;
    
    //negative_sequence_curr = negative_sequence_prop;
    
    area_curr = area_prop;
  }

  _positive_sequence_curr = _positive_sequence_prop;
  _negative_sequence_curr = _negative_sequence_prop;
  _positive_sequence_best = _positive_sequence_prop;
  _negative_sequence_best = _negative_sequence_prop;

  avg_area_change = total_area_change / num_moves;
  
  init_temperature = SP_P < 1 ? (SP_T0 * avg_area_change) / log(SP_P) : 
                                avg_area_change / log(SP_P);

  return init_temperature;
}


// perform simulated annealing
void SequencePair::_simulated_annealing(const double initial_temperature) {
 
  std::uniform_real_distribution<> dis(0.0, 1.0);
  
  double temperature = initial_temperature;

  //std::vector<int> positive_sequence_prop;
  //std::vector<int> negative_sequence_prop;
  //std::vector<int> positive_sequence_curr;
  //std::vector<int> negative_sequence_curr;
  //std::vector<int> positive_sequence_best;
  //std::vector<int> negative_sequence_best;
    
  //positive_sequence_curr = _positive_sequence;
  //negative_sequence_curr = _negative_sequence;

  //positive_sequence_best = positive_sequence_curr;
  //negative_sequence_best = negative_sequence_curr;
    
  size_t area_best = _urx * _ury;
  
  size_t area_curr = area_best; 
 
  size_t area_prop = area_curr;
  
  double cost = 0;

  size_t operation = 0;
    
  while (temperature > SP_FROZEN_TEMPERATURE) {
    
    for (size_t iter = 0; iter < SP_MAX_ITERATIONS_PER_TEMPERATURE; iter++) {
      
      _positive_sequence_prop = _positive_sequence_curr;
      _negative_sequence_prop = _negative_sequence_curr;

      operation = _generate_neighbors();
      
      area_prop = _pack(_positive_sequence_prop,
                        _negative_sequence_prop, 
                        operation);

      cost = area_prop < area_curr ?
             (double)-1*(area_curr - area_prop) : 
             (area_prop - area_curr);

      _cost.emplace_back(area_best);

      if (cost < 0) {
        //_cost.push_back(area_prop);
        if (operation != 0) {
          _positive_sequence_curr = _positive_sequence_prop;
          _negative_sequence_curr = _negative_sequence_prop;
        }

        area_curr = area_prop;

        if (area_prop < area_best) {
          
          _positive_sequence_best = _positive_sequence_prop;
          _negative_sequence_best = _negative_sequence_prop;

          area_best = area_prop;
          _modules_best = _modules;
        }
      }

      else {
        
        auto prob = std::exp(-cost / temperature); 
        
        if(prob > dis(_gen)) {
          //_cost.push_back(area_prop);
          if (operation != 0) {
            _positive_sequence_curr = _positive_sequence_prop;
            _negative_sequence_curr = _negative_sequence_prop;
          }
          area_curr = area_prop; 
        }
      }
    }
    
    temperature *= 0.95;  
  }

  _positive_sequence_curr = _positive_sequence_best;
  _negative_sequence_curr = _negative_sequence_best;
}


// generate neighbors
size_t SequencePair::_generate_neighbors() {

  std::uniform_real_distribution<> dis(0.0, 4.0);
  size_t choice = static_cast<size_t>(dis(_gen));

  switch(choice) {
    
    case 0:
      _rotate_module();
      break;
    
    case 1:
      _swap_two_nodes_positive_sequence();
      break;

    case 2:
      _swap_two_nodes_negative_sequence();
      break;

    case 3:
      _swap_two_nodes_two_sequences();
      break;
  }

  return choice;
}


// pack the modules
size_t SequencePair::_pack(const std::vector<size_t>& positive_sequence,
                           const std::vector<size_t>& negative_sequence,
                           const size_t operation) {
 
  _pack_helper(positive_sequence, negative_sequence, true, operation);
  _pack_helper(positive_sequence, negative_sequence, false, operation);
 
  //std::cout << "finished pack helper\n"; 
  size_t urx = 0, ury = 0;
  
  for (size_t i = 0; i < _num_modules; ++i) {
    
    if (_modules[i].llx + _modules[i].width > urx) {      
      urx = _modules[i].llx + _modules[i].width;
    }

    if (_modules[i].lly + _modules[i].height > ury) {
      ury = _modules[i].lly + _modules[i].height;
    }
  }

  _urx = urx;
  _ury = ury;

  return (_urx * _ury);  
}


// pack helper
void SequencePair::_pack_helper(const std::vector<size_t>& positive_sequence, 
                                const std::vector<size_t>& negative_sequence,
                                const bool is_horizontal,
                                const size_t operation) {

    
  //std::cout << "pack helper : operation = " << operation << ", is_horizontal = " << is_horizontal << '\n';
  /* 
  std::cout << "adjacency list horizontal \n";
  for (size_t i = 0; i < _adjacency_list_horizontal.size(); ++i) {
    for (size_t j = 0; j < _adjacency_list_horizontal[i].size(); ++j) {
      std::cout << "[" << i << "][" << j << "] = " << _adjacency_list_horizontal[i][j] << ", ";
    }
  }
  std::cout << "\nadjacency list vertical \n";
  for (size_t i = 0; i < _adjacency_list_vertical.size(); ++i) {
    for (size_t j = 0; j < _adjacency_list_vertical[i].size(); ++j) {
      std::cout << "[" << i << "][" << j << "] = " << _adjacency_list_vertical[i][j] << ", ";
    }
  }
  std::cout << '\n';
  for (auto p : positive_sequence) std::cout << p << ' ';
  std::cout << "      ";
  for (auto n : negative_sequence) std::cout << n << ' ';
  std::cout << '\n';
 
  for (auto t : _topology_order)  std::cout << t << ' ';
  std::cout << '\n'; 
  for (auto i : _in_degree_horizontal)  std::cout << i << ' ';
  std::cout << '\n'; 
  for (auto i : _in_degree_vertical)  std::cout << i << ' ';
  std::cout << '\n';
  */ 
  if (operation != 0) {
    _generate_adjacency_list(positive_sequence, 
                             negative_sequence,
                             is_horizontal);
  }
    
  _get_topology_order(is_horizontal);
  
  _get_longest_path(is_horizontal);
  /*
  std::cout << "\nafter --- adjacency list horizontal \n";
  for (size_t i = 0; i < _adjacency_list_horizontal.size(); ++i) {
    for (size_t j = 0; j < _adjacency_list_horizontal[i].size(); ++j) {
      std::cout << "[" << i << "][" << j << "] = " << _adjacency_list_horizontal[i][j] << ", ";
    }
  }
  std::cout << "\nafter --- adjacency list vertical \n";
  for (size_t i = 0; i < _adjacency_list_vertical.size(); ++i) {
    for (size_t j = 0; j < _adjacency_list_vertical[i].size(); ++j) {
      std::cout << "[" << i << "][" << j << "] = " << _adjacency_list_vertical[i][j] << ", ";
    }
  }
  std::cout << '\n';
  for (auto p : positive_sequence) std::cout << p << ' ';
  std::cout << "      ";
  for (auto n : negative_sequence) std::cout << n << ' ';
  std::cout << '\n';
  for (auto t : _topology_order)  std::cout << t << ' ';
  std::cout << '\n';
  for (auto i : _in_degree_horizontal)  std::cout << i << ' '; 
  for (auto i : _in_degree_vertical)  std::cout << i << ' '; 
  std::cout << "\n---------------------------------------------------\n";
  std::cin.get();
  */
  //}
  //std::cout << "after generate adjacency_list, adjacency_list has size = " << _adjacency_list.size() << '\n';
  //for (size_t i = 0; i < _adjacency_list.size(); ++i) {
  //  for (size_t j = 0; j < _adjacency_list[i].size(); ++j) {
  //    std::cout << "[" << i << "][" << j << "] = " << _adjacency_list[i][j] << ", ";
  //  }
  //}
  //for (auto p : positive_sequence) std::cout << p << ' ';
  //std::cout << '\n';
  //for (auto n : negative_sequence) std::cout << n << ' ';
  //std::cout << '\n';
  //std::cin.get();

  /* 
  if (is_horizontal) {  
    for (size_t i = 0; i < _num_modules; ++i) {
      //_modules[_mapping.at(i)].llx = _longest_path[i];  
      std::cout << "_modules[" << _mapping.at(i) << "].llx = " << _modules[_mapping.at(i)].llx << '\n'; 
    }
  }
  else {
    for (size_t i = 0; i < _num_modules; ++i) {
      //_modules[_mapping.at(i)].lly = _longest_path[i];  
      std::cout << "_modules[" << _mapping.at(i) << "].lly = " << _modules[_mapping.at(i)].lly << '\n'; 
    }
  }
  */
    
  //if (is_horizontal) {


    
    /*
    _dag = _generate_dag(positive_sequence,
                         negative_sequence,
                         true);
    
    // TODO: avoid frequent use of RAII-styled vector operation
    _topology_order = _dag.get_topology_order();
    _all_nodes = _dag.get_nodes();
    _longest_path = 
      _dag.longest_path(_all_nodes[_topology_order[0]]);
    */ 
    //for (size_t i = 0; i < _num_modules; ++i) {
     // _modules[_mapping.at(i)].llx = _longest_path[i];  
   // }
  //}
  /*
  else {
    
    _dag = _generate_dag(positive_sequence,
                         negative_sequence,
                         false);

    _topology_order = _dag.get_topology_order();
    _all_nodes = _dag.get_nodes();
    _longest_path = 
      _dag.longest_path(_all_nodes[_topology_order[0]]);
  
    for (size_t i = 0; i < _longest_path.size(); ++i) {
      _modules[i].lly = _longest_path[i];  
    }
  }
  */
}


// rotate a moudle
void SequencePair::_rotate_module() {
  
  std::uniform_real_distribution<> dis(0.0, _modules.size());
  
  size_t idx = static_cast<size_t>(dis(_gen));

  std::swap(_modules[idx].width, _modules[idx].height);  
}


// swap two modules in positive sequence
void SequencePair::_swap_two_nodes_positive_sequence() {
  
  std::uniform_real_distribution<> dis(0.0, _num_modules);

  size_t idx1 = static_cast<size_t>(dis(_gen));
  size_t idx2 = static_cast<size_t>(dis(_gen));

  while (idx1 == idx2) {
    idx2 = static_cast<size_t>(dis(_gen));
  }
  
   
  //std::cout << "\noperation 1\n";
  //for (auto p : _positive_sequence_prop)  std::cout << p << ' ';
  /*
  std::cout << "       ";
  for (auto n : _negative_sequence_prop)  std::cout << n << ' ';
  std::cout << '\n';
  */
  //std::cout << "idx1 = " << idx1 << ", idx2 = " << idx2 << '\n';
  
  //size_t id1 = _positive_sequence_prop[idx1];
  //size_t id2 = _positive_sequence_prop[idx2];
  /*
  for (size_t i = 0; i < _num_modules; ++i) {
    std::cout << "[" << i << "].first = " <<  _pn_mapping[i].first << '\n'; 
  }
  */
  /*
  std::cout << "id1 = " << id1 << ", id2 = " << id2 << '\n';
  */
  std::swap(_positive_sequence_prop[idx1], _positive_sequence_prop[idx2]);
 
  _changed_id1 = _positive_sequence_prop[idx1];
  _changed_id2 = _positive_sequence_prop[idx2]; 
  //for (auto p : _positive_sequence_prop)  std::cout << p << ' ';
  //std::cout << '\n';
  /*
  for (auto n : _negative_sequence_prop)  std::cout << n << ' ';
  std::cout << '\n';
  */

  //_pn_mapping[id1].first = idx2;
  //_pn_mapping[id2].first = idx1;
  /*
  for (size_t i = 0; i < _num_modules; ++i) {
    std::cout << "[" << i << "].first = " << _pn_mapping[i].first << '\n'; 
  }
  std::cin.get();
  */
}


// swap two modules in negative sequence
void SequencePair::_swap_two_nodes_negative_sequence() {

  std::uniform_real_distribution<> dis(0.0, _num_modules);

  size_t idx1 = static_cast<size_t>(dis(_gen));
  size_t idx2 = static_cast<size_t>(dis(_gen));

  while (idx1 == idx2) {
    idx2 = static_cast<size_t>(dis(_gen));
  }
  /*
  std::cout << "\noperation 2\n";
  //for (auto p : _positive_sequence_prop)  std::cout << p << ' ';
  //std::cout << "       ";
  for (auto n : _negative_sequence_prop)  std::cout << n << ' ';
  std::cout << '\n';
  
  std::cout << "idx1 = " << idx1 << ", idx2 = " << idx2 << '\n';
  */
  //size_t id1 = _negative_sequence_prop[idx1];
  //size_t id2 = _negative_sequence_prop[idx2];
  
  //for (size_t i = 0; i < _num_modules; ++i) {
  //  std::cout << "[" << i << "].second = " << _pn_mapping[i].second << '\n'; 
  //}
  //std::cout << "id1 = " << id1 << ", id2 = " << id2 << '\n';
  //size_t id1 = _negative_sequence_prop[idx1];
  //size_t id2 = _negative_sequence_prop[idx2];
  
  std::swap(_negative_sequence_prop[idx1], _negative_sequence_prop[idx2]);

  _changed_id1 = _negative_sequence_prop[idx1];
  _changed_id2 = _negative_sequence_prop[idx2];
  /*
  for (auto p : _positive_sequence_prop)  std::cout << p << ' ';
  std::cout << "       ";
  */
  //for (auto n : _negative_sequence_prop)  std::cout << n << ' ';
  //std::cout << '\n';
  
  //_pn_mapping[id1].second = idx2;
  //_pn_mapping[id2].second = idx1;
  /*
  for (size_t i = 0; i < _num_modules; ++i) {
    std::cout << "[" << i << "].second = " << _pn_mapping[i].second << '\n'; 
  }
  std::cin.get();
  */
}


// swap two moduels in both sequences
void SequencePair::_swap_two_nodes_two_sequences() {
  
  std::uniform_real_distribution<> dis(0.0, _num_modules);

  size_t pidx1 = static_cast<size_t>(dis(_gen));
  size_t pidx2 = static_cast<size_t>(dis(_gen));

  while (pidx1 == pidx2) {
    pidx2 = static_cast<size_t>(dis(_gen));
  }
  /*
  std::cout << "\noperation 3\n";
  for (auto p : _positive_sequence_prop)  std::cout << p << ' ';
  std::cout << "       ";
  for (auto n : _negative_sequence_prop)  std::cout << n << ' ';
  std::cout << '\n';
  
  std::cout << "pidx1 = " << pidx1 << ", pidx2 = " << pidx2 << '\n';
  */
  //size_t id1 = _positive_sequence_prop[pidx1];
  //size_t id2 = _positive_sequence_prop[pidx2];
  
  /*
  for (size_t i = 0; i < _num_modules; ++i) {
    std::cout << "[" << i << "].first = " <<  _pn_mapping[i].first << ", second = " << _pn_mapping[i].second << '\n'; 
  }
  std::cout << "id1 = " << id1 << ", id2 = " << id2 << '\n';

  std::cout << "my nidx1 = " << _pn_mapping[id1].second << ", nidx2 = " << _pn_mapping[id2].second << '\n';
  */
  //size_t nidx1 = _pn_mapping[id1].second;
  //size_t nidx2 = _pn_mapping[id2].second;

  std::vector<size_t>::iterator it;
  
  it = std::find(_negative_sequence_prop.begin(), 
                 _negative_sequence_prop.end(), 
                 _positive_sequence_prop[pidx1]);

  assert (it != _negative_sequence_prop.end());
  
  size_t nidx1 = std::distance(_negative_sequence_prop.begin(), it);  

  it = std::find(_negative_sequence_prop.begin(), 
                 _negative_sequence_prop.end(), 
                 _positive_sequence_prop[pidx2]);

  assert (it != _negative_sequence_prop.end());
  
  size_t nidx2 = std::distance(_negative_sequence_prop.begin(), it);  
  /*
  for (size_t i = 0; i < _num_modules; ++i) {
    if (_negative_sequence_prop[i] == _positive_sequence_prop[pidx1]) {
      nidx1 = i;
    }

    if (_negative_sequence_prop[i] == _positive_sequence_prop[pidx2]) {
      nidx2 = i;
    }

    if ((nidx1 != _num_modules) && (nidx2 != _num_modules)) {
      break;
    }
  }
  */
  //std::cout << "old nidx1 = " << nidx1 << ", nidx2 = " << nidx2 << '\n';
  std::swap(_positive_sequence_prop[pidx1], _positive_sequence_prop[pidx2]);
  std::swap(_negative_sequence_prop[nidx1], _negative_sequence_prop[nidx2]);

  //_pn_mapping[id1].first = pidx2;
  //_pn_mapping[id2].first = pidx1;

  //_pn_mapping[id1].second = nidx2;
  //_pn_mapping[id2].second = nidx1;
  /*
  for (size_t i = 0; i < _num_modules; ++i) {
    std::cout << "[" << i << "].first = " <<  _pn_mapping[i].first << ", second = " << _pn_mapping[i].second << '\n'; 
  }

  std::cin.get();
  */
}



// TODO----------------------------------------------------------- update in_degree
// generate adjacency_list
void SequencePair::_generate_adjacency_list(
  const std::vector<size_t>& positive_sequence,
  const std::vector<size_t>& negative_sequence,
  const bool is_horizontal) {

  //std::cout << "generate_adjacency_list\n";
  /*
  for (auto p : positive_sequence)  std::cout << p << ' ';
  std::cout << '\n';
  for (auto n : negative_sequence)  std::cout << n << ' ';
  std::cout << '\n';
  */

  std::vector<size_t>::const_iterator it;

  size_t pidx = 0, nidx = 0;
 
  if (is_horizontal) {
    //_adjacency_list_horizontal.clear();
    //_adjacency_list_horizontal.resize(_num_modules);
    
    for (auto& list : _adjacency_list_horizontal) list.clear();
    
    // TODO: duplicate?
    //_in_degree_horizontal.clear();
    //_in_degree_horizontal.resize(_num_modules);
    
    for (size_t i = 0; i < _num_modules; ++i) {
      _in_degree_horizontal.at(i) = 0;
    }
    
  }
  else {
    //_adjacency_list_vertical.clear();
    //_adjacency_list_vertical.resize(_num_modules);
    //_adjacency_list_vertical.resize(_num_modules);
    
    for (auto& list : _adjacency_list_vertical) list.clear();
    
    // TODO: duplicate?
    //_in_degree_vertical.clear();
    //_in_degree_vertical.resize(_num_modules);
    for (size_t i = 0; i < _num_modules; ++i) {
      _in_degree_vertical.at(i) = 0;
    }
  }

  _is_exist.clear();
   
  for (size_t i = 1; i < _num_modules; ++i) {
      _is_exist[positive_sequence[i]] = true;
  }
  
  while (pidx < _num_modules) {
    
    size_t node1_id = positive_sequence[pidx];
   
    //nidx = _pn_mapping[node1_id].second;
    _is_exist[node1_id] = false;
    
    it = std::find(negative_sequence.begin(),
                   negative_sequence.end(),
                   node1_id);

    assert(it != negative_sequence.end());
    
    nidx = std::distance(negative_sequence.begin(), it); 
    /*
    for (size_t i = 0; i < _num_modules; ++i) {
      if (node1_id == negative_sequence[i]) {
        nidx = i;
        break;
      }
    }
    */
    //_sequence.clear();

    
    // adjacency_list for horizontal dag
    if (is_horizontal) {
      //for (size_t i = pidx+1; i < _num_modules; ++i) {
      //  _sequence.insert(positive_sequence[i]);
      //}

      //original_size = _sequence.size();
      for (size_t i = nidx+1; i < _num_modules; ++i) {
        //_sequence.insert(negative_sequence[i]);
        
        // TODO: I think this can be reduced from N^2 to N?
        /*
        it = std::find(positive_sequence.begin() + (pidx+1),
                       positive_sequence.end(),
                       negative_sequence[i]); 
        */
        //if (_sequence.size() == original_size) {
        //if (it != positive_sequence.end()) {
        if (_is_exist[negative_sequence[i]]) {
          //std::cout << "node1_id = " << node1_id << '\n';
          //std::cout << "negative_sequence["<< i << "] = " << negative_sequence[i] << '\n';
          _adjacency_list_horizontal[node1_id].emplace_back(negative_sequence[i]);
          //std::cout << "after adjacency\n";
          //std::cout << "_in_degree has size " << _in_degree.size() << '\n';
          _in_degree_horizontal.at(negative_sequence[i]) = 
            _in_degree_horizontal.at(negative_sequence[i]) + 1; 
          //std::cout << "after in degree\n";
        }

        //original_size = _sequence.size();
      }
    }

    // adjacency_list for vertical dag
    else {
      //std::cout << "vertical dag\n";
      //for (size_t i = pidx+1; i < _num_modules; ++i) {
      //  _sequence.insert(positive_sequence[i]);
      //}

      //original_size = _sequence.size();
      //
      //assert(nidx != 0);

      // TODO: bug?
      for (int i = static_cast<int>(nidx)-1; i >= 0; --i) {
        //_sequence.insert(negative_sequence[i]);
        //std::cout << "negative_sequence[" << i << "] = " << negative_sequence[i] << '\n';
        //if (_sequence.size() == original_size) {
        /*
        it = std::find(positive_sequence.begin() + (pidx+1),
                       positive_sequence.end(),
                       negative_sequence[i]);
        */
        if (_is_exist[negative_sequence[i]]) {
        //if (it != positive_sequence.end()) {
          _adjacency_list_vertical[negative_sequence[i]].emplace_back(node1_id);
          _in_degree_vertical[node1_id] = _in_degree_vertical[node1_id] + 1; 
        }
      
        //original_size = _sequence.size();
      }
    }

    ++pidx;
  }
}


// get topology order of the adjacency_list
void SequencePair::_get_topology_order(
  const bool is_horizontal) {

  //std::cout << "get_topology_order\n";
  if (is_horizontal) { 
    _in_degree = _in_degree_horizontal; 
  }
  else {
    _in_degree = _in_degree_vertical;
  }

  _topology_order.clear();


  for (size_t i = 0; i < _num_modules; ++i) {
    if (_in_degree[i] == 0) {
      _queue.push(i);
    }
  }

  while (!_queue.empty()) {
    
    size_t n = _queue.front();
    
    _queue.pop();

    _topology_order.emplace_back(n);

    if (is_horizontal) { 
      
      for (size_t i = 0; i < _adjacency_list_horizontal[n].size(); ++i) {
        if ((--_in_degree[_adjacency_list_horizontal[n][i]]) == 0) {
          _queue.push(_adjacency_list_horizontal[n][i]);
        }
      }
    }

    else {
      for (size_t i = 0; i < _adjacency_list_vertical[n].size(); ++i) {
        if ((--_in_degree[_adjacency_list_vertical[n][i]]) == 0) {
          _queue.push(_adjacency_list_vertical[n][i]);
        }
      }
    }
  }
}


// get the longest path 
void SequencePair::_get_longest_path(const bool is_horizontal) {

  //std::cout << "get_longest_path\n";
  //_longest_path.clear();
  for (size_t i = 0; i < _num_modules; ++i) {
    //_longest_path.emplace_back(0);
    if (is_horizontal) {
      _modules[_mapping.at(i)].llx = 0;
    }
    else {
      _modules[_mapping.at(i)].lly = 0;
    }
  }

  //_longest_path.resize(_num_modules);

  size_t cost = 0;

  for (size_t i = 0; i < _num_modules; ++i) {
    
    size_t sid = _topology_order[i];

    if (is_horizontal) {
      for (size_t j = 0; j < _adjacency_list_horizontal[sid].size(); ++j) {
        //std::cout << sid << '\n'; 
        cost = _modules[_mapping.at(sid)].width;
        
        if (_modules[_mapping.at(_adjacency_list_horizontal[sid][j])].llx < 
            _modules[_mapping.at(sid)].llx + cost) {

          _modules[_mapping.at(_adjacency_list_horizontal[sid][j])].llx = 
            _modules[_mapping.at(sid)].llx + cost;
        }
        //std::cout << "_modules[" << _mapping.at(_adjacency_list[sid][j]) << "].llx = " << _modules[_mapping.at(_adjacency_list[sid][j])].llx << '\n'; 
      }
    }
    else {
      for (size_t j = 0; j < _adjacency_list_vertical[sid].size(); ++j) {
        cost = _modules[_mapping.at(sid)].height;
        
        if (_modules[_mapping.at(_adjacency_list_vertical[sid][j])].lly < 
            _modules[_mapping.at(sid)].lly + cost) {
          
          _modules[_mapping.at(_adjacency_list_vertical[sid][j])].lly = 
            _modules[_mapping.at(sid)].lly + cost;
        }
          //std::cout << "_modules[" << _mapping.at(_adjacency_list[sid][j]) << "].lly = " << _modules[_mapping.at(_adjacency_list[sid][j])].lly << '\n'; 
      }
        //std::cout << "cost = " << cost << '\n';
        //std::cout << "longest_path[" << sid << "] = " << _longest_path[sid] << '\n';
        /*
        if (_longest_path[_adjacency_list[sid][j]] < 
            _longest_path[sid] + cost) {

          _longest_path[_adjacency_list[sid][j]] = 
            _longest_path[sid] + cost;
        }
        */
    }
  }
}






}
