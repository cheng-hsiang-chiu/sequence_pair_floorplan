#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>
#include <map>
#include <chrono>
#include <fstream>
#include <queue>
#include <utility>
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
    
    void dump_json(std::string output_file);

  private:
    size_t _num_modules;
     
    std::string _input_file;

    std::vector<Node> _modules;

    std::vector<Node> _modules_best;

    std::vector<int> _positive_sequence_curr;

    std::vector<int> _negative_sequence_curr;

    std::vector<int> _positive_sequence_prop;

    std::vector<int> _negative_sequence_prop;
    
    std::vector<int> _positive_sequence_best;

    std::vector<int> _negative_sequence_best;
    
    std::random_device _rd;

    std::mt19937 _gen{_rd()};
    
    size_t _urx = 0;

    size_t _ury = 0;

    std::vector<int> _topology_order;

    //graph::DAG _dag;

    std::vector<int> _cost;

    //std::vector<graph::Node> _all_nodes;
    
    //std::vector<int> _longest_path;
    
    std::map<size_t, size_t> _mapping;

    std::vector<std::vector<size_t>> _adjacency_list;

    std::unordered_set<size_t> _sequence;
   
    std::vector<size_t> _in_degree;

    //std::map<size_t, std::pair<size_t, size_t>> _pn_mapping;


     
    void _generate_initial_pair();
  
    double _calculate_initial_temperature();

    void _simulated_annealing(const double initial_temperature);

    size_t _generate_neighbors();

    size_t _pack(const std::vector<int>& positive_sequence,
                 const std::vector<int>& negative_sequence);

    void _pack_helper(const std::vector<int>& positive_sequence,
                      const std::vector<int>& negative_sequence, 
                      const bool is_horizontal);

    void _dump_cost(std::string output_file);

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
      const std::vector<int>& positive_sequence,
      const std::vector<int>& negative_sequence,
      const bool is_horizontal); 
    
    void _get_topology_order();

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

  for (size_t i = 0; i < _modules.size(); ++i) {
    os << _modules[i].id     << ' '
       << _modules[i].llx    << ' '
       << _modules[i].lly    << ' '
       << _modules[i].width  << ' '
       << _modules[i].height << '\n';
  }
}


// dump floorplan to a file with a json extesion
// not yet finished
void SequencePair::dump_json(std::string output_file) {
 
  _dump_cost(output_file);
   
  if(output_file.rfind(".json") == std::string::npos) {
    output_file.append(".json");
  }
  
  std::ofstream outfile(output_file, std::ios::out);
 
  if(!outfile) {
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
    if(i == _num_modules-1)
      outfile << "}";
    else
      outfile << "},";
  }

  outfile << "]}"; 
}


// dump the cost 
void SequencePair::_dump_cost(const std::string output_file) {
  
  std::ofstream outfile(output_file+"_cost.txt", std::ios::out);
    
  if(!outfile) {
    std::cerr << "File could not be opened for writing\n";
    std::exit(EXIT_FAILURE); 
  }

  for (auto c : _cost) {
    outfile << c << '\n';
  }
}


// generate an initial pair
void SequencePair::_generate_initial_pair() {

  _adjacency_list.resize(_num_modules);
  
  for (size_t i = 0; i < _num_modules; ++i) {
    _adjacency_list.reserve(_num_modules);
  }

  _in_degree.reserve(_num_modules);
  _topology_order.reserve(_num_modules);

  _positive_sequence_curr.reserve(_num_modules);
  _negative_sequence_curr.reserve(_num_modules);
  _positive_sequence_prop.reserve(_num_modules);
  _negative_sequence_prop.reserve(_num_modules);
  _positive_sequence_best.reserve(_num_modules);
  _negative_sequence_best.reserve(_num_modules);

  for (size_t i = 0; i < _num_modules; ++i) {
    _positive_sequence_curr.emplace_back(_modules[i].id);
    _negative_sequence_curr.emplace_back(_modules[i].id);
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
  
  size_t area = _pack(_positive_sequence_curr, _negative_sequence_curr);
  
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
  size_t area_curr = _pack(_positive_sequence_curr, _negative_sequence_curr);
  size_t area_prop;

  while(num_moves < SP_RANDOM_MOVES) {
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
    
    area_prop = _pack(_positive_sequence_prop, _negative_sequence_prop);

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
 
  size_t area_prop;
  
  double cost;

  size_t operation;
    
  while (temperature > SP_FROZEN_TEMPERATURE) {
    
    for (size_t iter = 0; iter < SP_MAX_ITERATIONS_PER_TEMPERATURE; iter++) {
      
      _positive_sequence_prop = _positive_sequence_curr;
      _negative_sequence_prop = _negative_sequence_curr;

      operation = _generate_neighbors();
      
      area_prop = _pack(_positive_sequence_prop,
                        _negative_sequence_prop);

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
  int choice = dis(_gen);

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
size_t SequencePair::_pack(const std::vector<int>& positive_sequence,
                           const std::vector<int>& negative_sequence) {
 
  _pack_helper(positive_sequence, negative_sequence, true);
  _pack_helper(positive_sequence, negative_sequence, false);
 
  //std::cout << "finished pack helper\n"; 
  size_t urx = 0, ury = 0;
  
  for (size_t i = 0; i < _modules.size(); ++i) {
    
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
void SequencePair::_pack_helper(const std::vector<int>& positive_sequence, 
                                const std::vector<int>& negative_sequence,
                                const bool is_horizontal) {

  //if (operation != 0) {
  //std::cout << "pack helper : operation = " << operation << ", adjacency_list has size = " << _adjacency_list.size() << '\n';
  //for (size_t i = 0; i < _adjacency_list.size(); ++i) {
  //  for (size_t j = 0; j < _adjacency_list[i].size(); ++j) {
  //    std::cout << "[" << i << "][" << j << "] = " << _adjacency_list[i][j] << ", ";
  //  }
  //}
  //for (auto p : positive_sequence) std::cout << p << ' ';
  //std::cout << '\n';
  //for (auto n : negative_sequence) std::cout << n << ' ';
  //std::cout << '\n';
  _generate_adjacency_list(positive_sequence, 
                           negative_sequence,
                           is_horizontal);
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
  _get_topology_order();
  //}
  _get_longest_path(is_horizontal);

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

  std::swap(_positive_sequence_prop[idx1], _positive_sequence_prop[idx2]);
  
  //size_t id1 = _positive_sequence_prop[idx1];
  //size_t id2 = _positive_sequence_prop[idx2];

  //_pn_mapping[id1].first = idx2;
  //_pn_mapping[id2].first = idx1;
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

  //size_t id1 = _negative_sequence_prop[idx1];
  //size_t id2 = _negative_sequence_prop[idx2];

  //_pn_mapping[id1].second = idx2;
  //_pn_mapping[id2].second = idx1;
}


// swap two moduels in both sequences
void SequencePair::_swap_two_nodes_two_sequences() {
  
  std::uniform_real_distribution<> dis(0.0, _num_modules);

  size_t pidx1 = static_cast<size_t>(dis(_gen));
  size_t pidx2 = static_cast<size_t>(dis(_gen));

  while (pidx1 == pidx2) {
    pidx2 = static_cast<size_t>(dis(_gen));
  }


  //size_t id1 = _positive_sequence_prop[pidx1];
  //size_t id2 = _positive_sequence_prop[pidx2];
  
  
  //size_t nidx1 = _pn_mapping[id1].second;
  //size_t nidx2 = _pn_mapping[id2].second;
  size_t nidx1 = _num_modules;
  size_t nidx2 = _num_modules;

   
  for (size_t i = 0; i < _num_modules; ++i) {
    if (_negative_sequence_prop[i] == _positive_sequence_prop[pidx1]) {
      nidx1 = pidx1;
    }

    if (_negative_sequence_prop[i] == _positive_sequence_prop[pidx2]) {
      nidx2 = pidx2;
    }

    if ((nidx1 != _num_modules) && (nidx2 != _num_modules)) {
      break;
    }
  }
  
  std::swap(_positive_sequence_prop[pidx1], _positive_sequence_prop[pidx2]);
  std::swap(_negative_sequence_prop[nidx1], _negative_sequence_prop[nidx2]);


  //_pn_mapping[id1].first = pidx2;
  //_pn_mapping[id2].first = pidx1;

  //_pn_mapping[id1].second = nidx2;
  //_pn_mapping[id2].second = nidx1;
}



// TODO----------------------------------------------------------- update in_degree
// generate adjacency_list
void SequencePair::_generate_adjacency_list(
  const std::vector<int>& positive_sequence,
  const std::vector<int>& negative_sequence,
  const bool is_horizontal) {

  //std::cout << "generate_adjacency_list\n";
  /*
  for (auto p : positive_sequence)  std::cout << p << ' ';
  std::cout << '\n';
  for (auto n : negative_sequence)  std::cout << n << ' ';
  std::cout << '\n';
  */

  size_t pidx = 0, nidx = 0;
 
  size_t original_size = 0;

  _in_degree.clear();
  _adjacency_list.clear();
  
  _in_degree.resize(_num_modules);
  _adjacency_list.resize(_num_modules);
 
  while (pidx < _num_modules) {
    
    size_t node1_id = positive_sequence[pidx];
   
    //nidx = _pn_mapping[node1_id].second;
    
    for (size_t i = 0; i < _num_modules; ++i) {
      if (node1_id == negative_sequence[i]) {
        nidx = i;
        break;
      }
    }
    
    _sequence.clear();
    
    // adjacency_list for horizontal dag
    if (is_horizontal) {
      for (size_t i = pidx+1; i < _num_modules; ++i) {
        _sequence.insert(positive_sequence[i]);
      }

      original_size = _sequence.size();

      for (size_t i = nidx+1; i < _num_modules; ++i) {
        _sequence.insert(negative_sequence[i]);
        
        if (_sequence.size() == original_size) {
          //std::cout << "node1_id = " << node1_id << '\n';
          //std::cout << "negative_sequence["<< i << "] = " << negative_sequence[i] << '\n';
          _adjacency_list[node1_id].emplace_back(negative_sequence[i]);
          //std::cout << "after adjacency\n";
          //std::cout << "_in_degree has size " << _in_degree.size() << '\n';
          _in_degree[negative_sequence[i]] = _in_degree[negative_sequence[i]] + 1; 
          //std::cout << "after in degree\n";
        }

        original_size = _sequence.size();
      }
    }

    // adjacency_list for vertical dag
    else {
      //std::cout << "vertical dag\n";
      for (size_t i = pidx+1; i < _num_modules; ++i) {
        _sequence.insert(positive_sequence[i]);
      }

      original_size = _sequence.size();

      for (int i = nidx-1; i >= 0; --i) {
        _sequence.insert(negative_sequence[i]);
        //std::cout << "negative_sequence[" << i << "] = " << negative_sequence[i] << '\n';
        if (_sequence.size() == original_size) {
          _adjacency_list[negative_sequence[i]].emplace_back(node1_id);
          _in_degree[node1_id] = _in_degree[node1_id] + 1; 
        }
      
        original_size = _sequence.size();
      }
    }

    ++pidx;
  }
}


// get topology order of the adjacency_list
void SequencePair::_get_topology_order() {

  //std::cout << "get_topology_order\n";
   
  _topology_order.clear();

  std::queue<int> q;
  for (size_t i = 0; i < _num_modules; ++i) {
    if (_in_degree[i] == 0) {
      q.push(i);
    }
  }

  while (!q.empty()) {
    int n = q.front();
    q.pop();

    _topology_order.emplace_back(n);

    for (size_t i = 0; i < _adjacency_list[n].size(); ++i) {
      if ((--_in_degree[_adjacency_list[n][i]]) == 0) {
        q.push(_adjacency_list[n][i]);
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
    for (size_t j = 0; j < _adjacency_list[sid].size(); ++j) {
      //std::cout << sid << '\n'; 
      if (is_horizontal) {
        cost = _modules[_mapping.at(sid)].width;
        
        if (_modules[_mapping.at(_adjacency_list[sid][j])].llx < 
            _modules[_mapping.at(sid)].llx + cost) {

          _modules[_mapping.at(_adjacency_list[sid][j])].llx = 
            _modules[_mapping.at(sid)].llx + cost;
        }
        //std::cout << "_modules[" << _mapping.at(_adjacency_list[sid][j]) << "].llx = " << _modules[_mapping.at(_adjacency_list[sid][j])].llx << '\n'; 
      }
      else {
        cost = _modules[_mapping.at(sid)].height;
        
        if (_modules[_mapping.at(_adjacency_list[sid][j])].lly < 
            _modules[_mapping.at(sid)].lly + cost) {
          
          _modules[_mapping.at(_adjacency_list[sid][j])].lly = 
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


/*
// generate a DAG
graph::DAG SequencePair::_generate_dag(
  const std::vector<int>& positive_sequence,
  const std::vector<int>& negative_sequence,
  const bool is_horizontal) const {

  graph::DAG dag(positive_sequence.size());
  //graph::Node node1(0, 0);
  //graph::Node node2(0, 0);
   
  int pidx = 0, nidx = 0;

  std::unordered_set<int> sequence;

  while (pidx < positive_sequence.size()) {
    
    //std::unordered_set<int> sequence;
    sequence.clear();
    
    size_t node1_id = positive_sequence[pidx]; 
    size_t node1_cost;

    //auto found = _mapping.find(node1_id);
    
    //if (found != _mapping.end()) {
    if (is_horizontal) {
      node1_cost = _modules[_mapping.at(node1_id)].width;
    }
    else {
      node1_cost = _modules[_mapping.at(node1_id)].height;
    }
  
    //else {
    //  std::cerr << "Wrong positive_sequence";
    //  std::exit(EXIT_FAILURE);
    //}
     
    //for (size_t i = 0; i < _num_modules; ++i) {
      
      //if (node1_id == _modules[i].id) {
        
        //if (is_horizontal) {
          //node1_cost = _modules[i].width; 
        //}
        //else {
          //node1_cost = _modules[i].height;
        //}

        //break;
      //}
    //}
    
    graph::Node node1(node1_id, node1_cost);
    //std::cout << "node1 at " << &node1 << '\n';
    //node1.set_id(node1_id);
    //node1.set_cost(node1_cost);

    for (size_t i = pidx+1; i < positive_sequence.size(); i++) {
      sequence.insert(positive_sequence[i]);
    }

    for (size_t i = 0; i < negative_sequence.size(); ++i) {
    
      if (positive_sequence[pidx] == negative_sequence[i]) {
        nidx = i;
        break;
      }
    }

    // horizontal dag
    if (is_horizontal) {
    
      for (size_t i = nidx+1; i < negative_sequence.size(); ++i) {
        
        size_t original_size = sequence.size();
        
        sequence.insert(negative_sequence[i]);
        
        if (original_size == sequence.size()) {
          size_t node2_id = negative_sequence[i]; 
          size_t node2_cost;
          
          //auto found = _mapping.find(node2_id);
          
          //if (found != _mapping.end()) {
          node2_cost = _modules[_mapping.at(node2_id)].width;
          //}
          //else {
          //  std::cerr << "Wrong negative_sequence";
          //  std::exit(EXIT_FAILURE);
          //}


          // TODO: optimize this out...
          
          //for (size_t i = 0; i < _modules.size(); ++i) {
            //if (node2_id == _modules[i].id) {
              //node2_cost = _modules[i].width; 
              //break;
            //}
          //}
          

          graph::Node node2(node2_id, node2_cost);
          //std::cout << "node2 at " << &node2 << '\n';
          dag.add_edge(node1, node2);
        }
      }
    }

    // vertical dag
    else {
      //std::cout << "vertical dag with nidx = " << nidx << " \n";

      for (int i = nidx-1; i >= 0; --i) {
        
        size_t original_size = sequence.size();
        
        sequence.insert(negative_sequence[i]);
        
        if (original_size == sequence.size()) {
          //std::cout << "nidx = " << nidx << " and i = " << i << '\n';
          size_t node2_id = negative_sequence[i]; 
          size_t node2_cost;

          //auto found = _mapping.find(node2_id);
          
          //if (found != _mapping.end()) {
          node2_cost = _modules[_mapping.at(node2_id)].height;
          //}
          //else {
          //  std::cerr << "Wrong negative_sequence";
          //  std::exit(EXIT_FAILURE);
          //}

          
          //for (size_t i = 0; i < _modules.size(); ++i) {
          //  if (node2_id == _modules[i].id) {
          //    node2_cost = _modules[i].height;
          //    break;
          //  }
          //}
          
     
          graph::Node node2(node2_id, node2_cost);
          //std::cout << "node2 at " << &node2 << '\n';
          dag.add_edge(node2, node1);
        }
      }
    }
    ++pidx;
  }
  //dag.dump(std::cout);
  
  //std::vector<graph::Node*> all_nodes = dag.get_nodes();
  //for (size_t i = 0; i < all_nodes.size(); ++i)
    //std::cout << all_nodes[i]->id << " has cost = " << all_nodes[i]->cost << '\n';
  
  return dag;
}
*/




}
