#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <unordered_set>
#include <chrono>
#include "../lib/graph.hpp"




#define SP_FROZEN_TEMPERATURE 0.1
#define SP_MAX_ITERATIONS_PER_TEMPERATURE 1000
#define SP_RANDOM_MOVES 60
#define SP_P 0.99
#define SP_T0 -1


namespace sp {
 
 
struct Node{
  int id;
  size_t width;
  size_t height;
  size_t llx = 0;
  size_t lly = 0;  
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

    std::vector<int> _positive_sequence;

    std::vector<int> _negative_sequence;

    std::random_device _rd;

    std::mt19937 _gen{_rd()};
    
    size_t _urx = 0;

    size_t _ury = 0;

    std::vector<int> _topological_order;

    graph::DAG _dag;

    std::vector<int> _cost;

    void _generate_initial_pair();
  
    double _calculate_initial_temperature();

    void _simulated_annealing(const double initial_temperature);

    void _generate_neighbors(std::vector<int>& positive_sequence_prop,
                             std::vector<int>& negative_sequence_prop);

    size_t _pack(const std::vector<int>& positive_sequence,
                 const std::vector<int>& negative_sequence);

    void _pack_helper(const std::vector<int>& positive_sequence,
                      const std::vector<int>& negative_sequence, 
                      const bool is_horizontal);

    void _dump_cost(std::string output_file);

    void _rotate_module();

    void _swap_two_nodes_one_sequence(std::vector<int>& sequence);

    void _swap_two_nodes_two_sequences(
      std::vector<int>& positive_sequence_prop,
      std::vector<int>& negative_sequence_prop);

    graph::DAG _generate_dag(
      const std::vector<int>& positive_sequence,
      const std::vector<int>& negative_sequence,
      const bool is_horizontal) const;
};


// read in the module configurations
void SequencePair::open(const std::string input_file) {

  _input_file = input_file;

  std::ifstream infile(input_file);
   
  if (!infile) {
    std::cerr << "File could not be opened!!\n";
    std::exit(EXIT_FAILURE);
  }

  size_t width, height, id;
  infile >> _num_modules;

  while (infile >> id >> width >> height) {
    Node node;
    node.id = id;
    node.width = width;
    node.height = height; 

    _modules.push_back(node);
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

  for (size_t i = 0; i < _num_modules; ++i) {
    _positive_sequence.push_back(_modules[i].id);
    _negative_sequence.push_back(_modules[i].id);
  }
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

  std::cout << "initial temperature = " << initial_temperature << '\n';

  auto tend = std::chrono::steady_clock::now();

  std::cout << "calculate initial temperature completed: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(tend-tbeg).count()
            << " ms\n";  
            
  tbeg = std::chrono::steady_clock::now();
  
  _simulated_annealing(initial_temperature);

  tend = std::chrono::steady_clock::now();

  std::cout << "simulated annealing completed: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(tend-tbeg).count()
            << " ms\n";  

  _modules = _modules_best;

  tbeg = std::chrono::steady_clock::now();
  
  size_t area = _pack(_positive_sequence, _negative_sequence);
  
  tend = std::chrono::steady_clock::now();
  
  std::cout << "pack modules completed: " 
            << std::chrono::duration_cast<std::chrono::milliseconds>(tend-tbeg).count()
            << " ms\n";  
}


// calculate initial temperature
double SequencePair::_calculate_initial_temperature() {
  
  size_t num_moves = 0;
  double total_area_change = 0.0;
  double delta_area, avg_area_change, init_temperature;
  
  std::vector<int> positive_sequence_curr = _positive_sequence;
  std::vector<int> negative_sequence_curr = _negative_sequence;
  std::vector<int> positive_sequence_prop = _positive_sequence;
  std::vector<int> negative_sequence_prop = _negative_sequence;

  size_t area_curr = _pack(positive_sequence_curr, negative_sequence_curr);
  size_t area_prop;

  while(num_moves < SP_RANDOM_MOVES) {
    _generate_neighbors(positive_sequence_prop, negative_sequence_prop);

    area_prop = _pack(positive_sequence_prop, negative_sequence_prop);
    delta_area = area_prop > area_curr ? 
                 (area_prop - area_curr) : 
                 (area_curr - area_prop); 

    total_area_change += delta_area;
    ++num_moves;
    positive_sequence_curr = positive_sequence_prop;
    negative_sequence_curr = negative_sequence_prop;
    area_curr = area_prop;
  }

  avg_area_change = total_area_change / num_moves;
  
  init_temperature = SP_P < 1 ? (SP_T0 * avg_area_change) / log(SP_P) : 
                                avg_area_change / log(SP_P);

  return init_temperature;
}


// perform simulated annealing
void SequencePair::_simulated_annealing(const double initial_temperature) {
 
  std::uniform_real_distribution<> dis(0.0, 1.0);
  
  double temperature = initial_temperature;

  std::vector<int> positive_sequence_prop;
  std::vector<int> negative_sequence_prop;
  std::vector<int> positive_sequence_curr;
  std::vector<int> negative_sequence_curr;
  std::vector<int> positive_sequence_best;
  std::vector<int> negative_sequence_best;
    
  positive_sequence_curr = _positive_sequence;
  negative_sequence_curr = _negative_sequence;

  positive_sequence_best = positive_sequence_curr;
  negative_sequence_best = negative_sequence_curr;
    
  size_t area_best = _pack(positive_sequence_best,
                           negative_sequence_best);
  size_t area_curr = area_best; 
 
  size_t area_prop;
  
  double cost;
    
  while(temperature > SP_FROZEN_TEMPERATURE) {
    for(size_t iter = 0; iter < SP_MAX_ITERATIONS_PER_TEMPERATURE; iter++) {
      positive_sequence_prop = positive_sequence_curr;
      negative_sequence_prop = negative_sequence_curr;

      _generate_neighbors(positive_sequence_prop, 
                         negative_sequence_prop);
      
      area_prop = _pack(positive_sequence_prop,
                        negative_sequence_prop);

      cost = area_prop < area_curr ?
             (double)-1*(area_curr - area_prop) : 
             (area_prop - area_curr);

      _cost.push_back(area_best);

      if(cost < 0) {
        //_cost.push_back(area_prop);
        positive_sequence_curr = positive_sequence_prop;
        negative_sequence_curr = negative_sequence_prop;
  
        area_curr = area_prop;

        if(area_prop < area_best) {
          positive_sequence_best = positive_sequence_prop;
          negative_sequence_best = negative_sequence_prop;

          area_best = area_prop;
          _modules_best = _modules;
        }
      }

      else {
        auto prob = std::exp(-cost / temperature); 
        if(prob > dis(_gen)) {
          //_cost.push_back(area_prop);
          positive_sequence_curr = positive_sequence_prop;
          negative_sequence_curr = negative_sequence_prop;
  
          area_curr = area_prop; 
        }
      }
    }
    temperature *= 0.95;  
  }

  _positive_sequence = positive_sequence_best;
  _negative_sequence = negative_sequence_best;
}


// generate neighbors
void SequencePair::_generate_neighbors(
  std::vector<int>& positive_sequence_prop,
  std::vector<int>& negative_sequence_prop) {

  std::uniform_real_distribution<> dis(0.0, 3.0);
  int choice = dis(_gen);

  switch(choice) {
    case 0:
      _rotate_module();
      break;
    case 1:
      if (dis(_gen)/3 > 0.5) {
        _swap_two_nodes_one_sequence(positive_sequence_prop);
      }
      else {
        _swap_two_nodes_one_sequence(negative_sequence_prop);
      }
      break;
    case 2:
      _swap_two_nodes_two_sequences(positive_sequence_prop,
                                    negative_sequence_prop); 
    break;
  }
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

  if (is_horizontal) {
    _dag = _generate_dag(positive_sequence,
                         negative_sequence,
                         true);
    
    // TODO: avoid frequent use of RAII-styled vector operation
    std::vector<int> topology_order = _dag.get_topology_order();
    std::vector<graph::Node> all_nodes = _dag.get_nodes();
    std::vector<int> longest_path = 
      _dag.longest_path(all_nodes[topology_order[0]]);
  
    for (size_t i = 0; i < longest_path.size(); ++i) {
      _modules[i].llx = longest_path[i];  
    }
  }

  else {
    _dag = _generate_dag(positive_sequence,
                         negative_sequence,
                         false);

    std::vector<int> topology_order = _dag.get_topology_order();
    std::vector<graph::Node> all_nodes = _dag.get_nodes();
    std::vector<int> longest_path = 
      _dag.longest_path(all_nodes[topology_order[0]]);
  
    for (size_t i = 0; i < longest_path.size(); ++i) {
      _modules[i].lly = longest_path[i];  
    }
  }
}


// rotate a moudle
void SequencePair::_rotate_module() {
  
  std::uniform_real_distribution<> dis(0.0, _modules.size());
  
  size_t id = static_cast<size_t>(dis(_gen));

  std::swap(_modules[id].width, _modules[id].height);  
}


// swap two modules in one sequence
void SequencePair::_swap_two_nodes_one_sequence(std::vector<int>& sequence) {
  
  std::uniform_real_distribution<> dis(0.0, _modules.size());

  size_t id1 = static_cast<size_t>(dis(_gen));
  size_t id2 = static_cast<size_t>(dis(_gen));

  while(id1 == id2) {
    id2 = static_cast<size_t>(dis(_gen));
  }

  std::swap(sequence[id1], sequence[id2]);
}


// swap two moduels in both sequences
void SequencePair::_swap_two_nodes_two_sequences(
  std::vector<int>& positive_sequence_prop,
  std::vector<int>& negative_sequence_prop) {
  
  std::uniform_real_distribution<> dis(0.0, _modules.size());

  size_t pid1 = static_cast<size_t>(dis(_gen));
  size_t pid2 = static_cast<size_t>(dis(_gen));

  while(pid1 == pid2) {
    pid2 = static_cast<size_t>(dis(_gen));
  }

  size_t nid1 = _modules.size();
  size_t nid2 = _modules.size();

  for (size_t i = 0; i < _modules.size(); ++i) {
    if (negative_sequence_prop[i] == positive_sequence_prop[pid1]) {
      nid1 = pid1;
    }

    if (negative_sequence_prop[i] == positive_sequence_prop[pid2]) {
      nid2 = pid2;
    }

    if ((nid1 != _modules.size()) && (nid2 != _modules.size())) {
      break;
    }
  }

  std::swap(positive_sequence_prop[pid1], positive_sequence_prop[pid2]);
  std::swap(negative_sequence_prop[nid1], negative_sequence_prop[nid2]);
}


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

    
    for (size_t i = 0; i < _modules.size(); ++i) {
      if (node1_id == _modules[i].id) {
        if (is_horizontal) {
          node1_cost = _modules[i].width; 
        }
        else {
          node1_cost = _modules[i].height;
        }
        break;
      }
    }
    
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

    if (is_horizontal) {
      for (size_t i = nidx+1; i < negative_sequence.size(); ++i) {
        
        size_t original_size = sequence.size();
        
        sequence.insert(negative_sequence[i]);
        
        if (original_size == sequence.size()) {
          size_t node2_id = negative_sequence[i]; 
          size_t node2_cost;
          
          // TODO: optimize this out...
          for (size_t i = 0; i < _modules.size(); ++i) {
            if (node2_id == _modules[i].id) {
              node2_cost = _modules[i].width; 
              break;
            }
          }

          graph::Node node2(node2_id, node2_cost);
          //std::cout << "node2 at " << &node2 << '\n';
          dag.add_edge(node1, node2);
        }
      }
    }

    else {
      //std::cout << "vertical dag with nidx = " << nidx << " \n";

      for (int i = nidx-1; i >= 0; --i) {
        
        size_t original_size = sequence.size();
        
        sequence.insert(negative_sequence[i]);
        
        if (original_size == sequence.size()) {
          //std::cout << "nidx = " << nidx << " and i = " << i << '\n';
          size_t node2_id = negative_sequence[i]; 
          size_t node2_cost;

          for (size_t i = 0; i < _modules.size(); ++i) {
            if (node2_id == _modules[i].id) {
              node2_cost = _modules[i].height;
              break;
            }
          }

          graph::Node node2(node2_id, node2_cost);
          //std::cout << "node2 at " << &node2 << '\n';
          dag.add_edge(node2, node1);
        }
      }
    }
    ++pidx;
  }
  //dag.dump(std::cout);
  /*
  std::vector<graph::Node*> all_nodes = dag.get_nodes();
  for (size_t i = 0; i < all_nodes.size(); ++i)
    std::cout << all_nodes[i]->id << " has cost = " << all_nodes[i]->cost << '\n';
  */
  return dag;




}





}
