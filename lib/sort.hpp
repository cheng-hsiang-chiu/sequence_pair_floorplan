#include <vector>
#include <queue>
#include <iostream>




namespace sort {


class TopologicalSort {
  public:
    static std::vector<int> sort(
      const std::vector<std::vector<int>> adjacency_list);
};


std::vector<int> TopologicalSort::sort(
  const std::vector<std::vector<int>> adjacency_list) {
  
  size_t count = 0;
  std::vector<int> result;

  // calculate the in_degree of each node
  std::vector<int> in_degree(adjacency_list.size(), 0);
  for (size_t i = 0; i < adjacency_list.size(); ++i) {
    if (!adjacency_list[i].empty()) {
      for (size_t j = 0; j < adjacency_list[i].size(); ++j) {
        in_degree[adjacency_list[i][j]] = in_degree[adjacency_list[i][j]] + 1;        
      }
    }
  }

  std::queue<int> q;
  for (size_t i = 0; i < in_degree.size(); i++) {
    if (in_degree[i] == 0) {
      q.push(i);
    }
  }

  while (!q.empty()) {
    int n = q.front();
    q.pop();
    
    result.push_back(n);
    
    for (size_t i = 0; i < adjacency_list[n].size(); ++i) {
      if ((--in_degree[adjacency_list[n][i]]) == 0)
        q.push(adjacency_list[n][i]);   
    }

    count++;
  }

  if (count != adjacency_list.size()) {
    std::cerr << "There exists a cycle in the DAG\n";
    std::exit(EXIT_FAILURE);
  }

  return result;  
}



}


