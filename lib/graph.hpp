#include <vector>
#include <iostream>
#include <cassert>
#include <fstream>
#include <limits>
#include "sort.hpp"


namespace graph {


struct Node {
  int id;
  int cost;

  Node() : id(-1), cost(-1) {}
  Node(int _id, int _cost) : id(_id), cost(_cost) {}
  void set_id(int _id)     { id = _id; }
  void set_cost(int _cost) { cost = _cost; }
};


class DAG {
  public:

    DAG() = default;

    DAG(const size_t num);

    void add_edge(Node src, Node dst);

    void dump(std::ostream& os) const;

    std::vector<int> shortest_path(const Node& src) const;

    std::vector<int> longest_path(const Node& src) const;

    std::vector<Node> get_nodes() const;

    std::vector<int> get_topology_order() const;
  private:
    // _adjacency_list keeps the id of a Node
    std::vector<std::vector<int> > _adjacency_list;

    std::vector<Node> _nodes;
};


// constructor
DAG::DAG(const size_t num) {

  for (size_t i = 0; i < num; ++i) {
    std::vector<int> v;
    _adjacency_list.push_back(v);
    Node node;
    _nodes.push_back(node);
  }
}


// add an edge from src to dst in the graph
void DAG::add_edge(Node src, Node dst) {
  
  //std::cout << "in add-edge\n";
  //std::cout << "src id = " << src.id << " and dst id = " << dst.id << " at " << &dst << '\n';
  assert(src.id < _adjacency_list.size());
   
  _adjacency_list[src.id].push_back(dst.id); 

  if (_nodes[src.id].id == -1) {
    _nodes[src.id] = src;
  }
  if (_nodes[dst.id].id == -1) {
    _nodes[dst.id] = dst;
  }
  /*
  for (size_t i = 0; i < _nodes.size(); ++i) {
    if (_nodes[i].id != -1)
      std::cout << _nodes[i].id << " has cost " << _nodes[i].cost << '\n';
  }
  std::cout << "------------\n";
  */
}


// get the topology order of the DAG
std::vector<int> DAG::get_topology_order() const {
  
  std::vector<int> topology_order = 
    sort::TopologicalSort::sort(_adjacency_list);

  return topology_order;
}

// dump the dag to console in a topological order
void DAG::dump(std::ostream& os) const {
  
  std::vector<int> ordered_graph = 
    sort::TopologicalSort::sort(_adjacency_list);
  os << "Thr topological sort of the graph is : \n";
  for (auto g : ordered_graph) {
    os << g << ' ';
    os << '\n';
  }
}


// return the shortest path from a node
std::vector<int> DAG::longest_path(const Node& src) const {
  
  std::vector<int> ordered_graph =
    sort::TopologicalSort::sort(_adjacency_list); 

  std::vector<int> path(_adjacency_list.size(), 0);

  size_t start_index = 0;
  for (size_t i = 0; i < ordered_graph.size(); ++i) {
    if (ordered_graph[i] == src.id) {
      start_index = i;
      break;  
    }
  }

  for (size_t i = start_index; i < ordered_graph.size(); ++i) {
    size_t sid = ordered_graph[i];
    for (size_t j = 0; j < _adjacency_list[sid].size(); ++j) {
      if (path[_adjacency_list[sid][j]] < 
          path[sid] + _nodes[sid].cost) {
        path[_adjacency_list[sid][j]] = 
          path[sid] + _nodes[sid].cost;
      }
    }
  }

  return path;
}


// get the _nodes
std::vector<Node> DAG::get_nodes() const {
  
  return _nodes;
}


}
