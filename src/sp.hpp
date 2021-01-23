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

    std::vector<size_t> _cost;

    std::map<size_t, size_t> _mapping;

    std::vector<std::vector<size_t>> _adjacency_list_horizontal;

    std::vector<std::vector<size_t>> _adjacency_list_vertical;
    
    std::vector<size_t> _in_degree_horizontal;

    std::vector<size_t> _in_degree_vertical;
    
    std::vector<size_t> _in_degree;
    
    std::queue<size_t> _queue;

    std::vector<size_t> _seq;
    
    std::vector<bool> _is_exist;
     
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
  
  std::ofstream outfile(output_file + "_cost.txt", std::ios::out);
    
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
    _adjacency_list_horizontal.at(i).resize(_num_modules);
    _adjacency_list_vertical.at(i).resize(_num_modules);
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
  
  size_t area_curr = _pack(_positive_sequence_curr, _negative_sequence_curr, 1);
  
  size_t area_prop = area_curr;

  while (num_moves < SP_RANDOM_MOVES) {
    
    size_t operation = _generate_neighbors();
    
    area_prop = _pack(_positive_sequence_prop, _negative_sequence_prop, operation);

    delta_area = area_prop > area_curr ? 
                 (area_prop - area_curr) : 
                 (area_curr - area_prop); 

    total_area_change += delta_area;
    
    ++num_moves;
    
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

      //_cost.emplace_back(area_best);

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
        
        if (prob > dis(_gen)) {
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
void SequencePair::_pack_helper(
  const std::vector<size_t>& positive_sequence, 
  const std::vector<size_t>& negative_sequence,
  const bool is_horizontal,
  const size_t operation) {

  if (operation != 0) {
    _generate_adjacency_list(positive_sequence, 
                             negative_sequence,
                             is_horizontal);
  }
    
  _get_topology_order(is_horizontal);
  
  _get_longest_path(is_horizontal);
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
  
  std::swap(_positive_sequence_prop[idx1], _positive_sequence_prop[idx2]);
}


// swap two modules in negative sequence
void SequencePair::_swap_two_nodes_negative_sequence() {

  std::uniform_real_distribution<> dis(0.0, _num_modules);

  size_t idx1 = static_cast<size_t>(dis(_gen));
  size_t idx2 = static_cast<size_t>(dis(_gen));

  while (idx1 == idx2) {
    idx2 = static_cast<size_t>(dis(_gen));
  }
  
  std::swap(_negative_sequence_prop[idx1], _negative_sequence_prop[idx2]);
}


// swap two moduels in both sequences
void SequencePair::_swap_two_nodes_two_sequences() {
  
  std::uniform_real_distribution<> dis(0.0, _num_modules);

  size_t pidx1 = static_cast<size_t>(dis(_gen));
  size_t pidx2 = static_cast<size_t>(dis(_gen));

  while (pidx1 == pidx2) {
    pidx2 = static_cast<size_t>(dis(_gen));
  }

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
  
  std::swap(_positive_sequence_prop[pidx1], _positive_sequence_prop[pidx2]);
  std::swap(_negative_sequence_prop[nidx1], _negative_sequence_prop[nidx2]);
}


// generate adjacency_list
void SequencePair::_generate_adjacency_list(
  const std::vector<size_t>& positive_sequence,
  const std::vector<size_t>& negative_sequence,
  const bool is_horizontal) {

  std::vector<size_t>::const_iterator it;

  size_t pidx = 0, nidx = 0;
 
  if (is_horizontal) {
    for (auto& list : _adjacency_list_horizontal) list.clear();
    
    for (size_t i = 0; i < _num_modules; ++i) {
      _in_degree_horizontal.at(i) = 0;
    }
  }
  else {
    for (auto& list : _adjacency_list_vertical) list.clear();
    
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
   
    _is_exist[node1_id] = false;
    
    it = std::find(negative_sequence.begin(),
                   negative_sequence.end(),
                   node1_id);

    assert(it != negative_sequence.end());
    
    nidx = std::distance(negative_sequence.begin(), it); 
    
    // adjacency_list for horizontal dag
    if (is_horizontal) {
      
      for (size_t i = nidx+1; i < _num_modules; ++i) {
        
        if (_is_exist[negative_sequence[i]]) {
          
          _adjacency_list_horizontal[node1_id].emplace_back(negative_sequence[i]);
          
          _in_degree_horizontal.at(negative_sequence[i]) = 
            _in_degree_horizontal.at(negative_sequence[i]) + 1; 
        }
      }
    }

    // adjacency_list for vertical dag
    else {
      for (size_t i = 0; i < nidx; ++i) {
      
        if (_is_exist[negative_sequence[i]]) {
          
          _adjacency_list_vertical[negative_sequence[i]].emplace_back(node1_id);
          
          _in_degree_vertical.at(node1_id) = _in_degree_vertical.at(node1_id) + 1; 
        }
      }
      /*
      for (int i = static_cast<int>(nidx)-1; i >= 0; --i) {
        
        if (_is_exist[negative_sequence[i]]) {
          
          _adjacency_list_vertical[negative_sequence[i]].emplace_back(node1_id);
          
          _in_degree_vertical.at(node1_id) = _in_degree_vertical.at(node1_id) + 1; 
        }
      }
      */
    }

    ++pidx;
  }
}


// get topology order of the adjacency_list
void SequencePair::_get_topology_order(
  const bool is_horizontal) {

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

  for (size_t i = 0; i < _num_modules; ++i) {
    
    if (is_horizontal) {
      _modules[_mapping.at(i)].llx = 0;
    }
    else {
      _modules[_mapping.at(i)].lly = 0;
    }
  }

  size_t cost = 0;

  size_t sid = 0;

  for (size_t i = 0; i < _num_modules; ++i) {
    
    sid = _topology_order[i];

    if (is_horizontal) {
      
      for (size_t j = 0; j < _adjacency_list_horizontal.at(sid).size(); ++j) {
        
        cost = _modules[_mapping.at(sid)].width;
        
        if (_modules[_mapping.at(_adjacency_list_horizontal.at(sid).at(j))].llx < 
            _modules[_mapping.at(sid)].llx + cost) {

          _modules[_mapping.at(_adjacency_list_horizontal.at(sid).at(j))].llx = 
            _modules[_mapping.at(sid)].llx + cost;
        }
      }
    }
    else {
      
      for (size_t j = 0; j < _adjacency_list_vertical.at(sid).size(); ++j) {
        
        cost = _modules[_mapping.at(sid)].height;
        
        if (_modules[_mapping.at(_adjacency_list_vertical.at(sid).at(j))].lly < 
            _modules[_mapping.at(sid)].lly + cost) {
          
          _modules[_mapping.at(_adjacency_list_vertical.at(sid).at(j))].lly = 
            _modules[_mapping.at(sid)].lly + cost;
        }
      }
    }
  }
}



}
