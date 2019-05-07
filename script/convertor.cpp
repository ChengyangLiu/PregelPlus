#include <stdlib.h>
#include <time.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "mpi.h"

using namespace std;

int my_rank = 0, comm_sz = 0;

map<long, int> v_parts;

/* vertex of a graph */
class Vertex {
 public:
  Vertex() {}
  ~Vertex() {}

  Vertex(long id, long label) : _id(id), _label(label) {}

  Vertex(long id) : _id(id) {}

  inline void addLabel(long label) { _label = label; }

  inline long id() { return _id; }
  inline long label() { return _label; }

 private:
  long _id;     // vertex id
  long _label;  // vertex label
};

/* node of a graph, that's an encapsulation for vertex */
class Node {
 public:
  Node() {}
  ~Node() {}

  Node(Vertex& vertex) : _vertex(vertex) {}

  /* add a neighborhood vertex to an exist vertex */
  inline void addNeighbor(long neighbor, long elabel) {
    _neighbors.emplace_back(neighbor);
    _elabels.emplace_back(elabel);
  }

  inline Vertex& v() { return _vertex; }
  inline vector<long>& neighbors() { return _neighbors; }
  inline vector<long>& elabels() { return _elabels; }

 private:
  Vertex _vertex;              // source
  vector<long> _neighbors;  // destinations
  vector<long> _elabels;       // labels
};

/* class graph */
class Graph {
 public:
  Graph() : _num(0) {}
  ~Graph() {}

  /* load graph */
  void loadGraph(string& filename, int partnum) {
    string rfile = filename + ".part" + to_string(partnum);
    string vfile = filename + ".v";
    string efile = filename + ".e";

    string line;
    long src, dst, label, part;
    try {
      ifstream fin_r(rfile);
      // read vertex in
      while (getline(fin_r, line)) {
        stringstream ss(line);
        ss >> src >> part;
        v_parts[src] = part;
        if (part == my_rank) {
          addVertex(src);
        }
      }
      fin_r.close();

      ifstream fin_v(vfile);
      // update vertex
      while (getline(fin_v, line)) {
        stringstream ss(line);
        ss >> src >> label;
        if (v_parts[src] == my_rank) {
          updateVInfo(src, label);
        }
      }
      fin_v.close();

      ifstream fin_e(efile);
      // read edge in
      while (getline(fin_e, line)) {
        stringstream ss(line);
        ss >> src >> dst >> label;
        if (v_parts[src] == my_rank) {
          addEdge(src, dst, label);
        }
      }
      fin_e.close();
    } catch (exception& e) {
      cout << e.what();
      exit(0);
    }
  }

  /* write graph to file */
  void writeGraph(string& filename) {
    string rltfile = filename + to_string(my_rank);
    ofstream fout(rltfile);
    for (auto& node : _nodes) {
      Vertex& vertex = node.v();
      vector<long>& neighbors = node.neighbors();
      vector<long>& elabels = node.elabels();
      fout << vertex.id() << "\t" << vertex.label();
      long len = neighbors.size();
      if (len == 0) {
        fout << "\t0\n";
      } else {
        fout << "\t" << len;
        int i;
        for (i = 0; i < len; i++) {
          fout << "\t" << neighbors[i] << "\t" << elabels[i];
        }
        fout << "\n";
      }
    }
  }

  inline Node* node(long vid) {
    return _vid_map.find(vid) != _vid_map.end() ? &_nodes[_vid_map[vid]] : NULL;
  }

 private:
   inline void addVertex(long id) {
     Vertex v(id);
     Node node(v);
     _nodes.emplace_back(node);
     _vid_map[id] = _num++;
   }

   inline void updateVInfo(long id, long label) {
     Node* node = Graph::node(id);
     node->v().addLabel(label);
   }

   // addEdge must be called after addVertex
   inline void addEdge(long src, long dst, long label) {
     Node* src_node = Graph::node(src);
     src_node->addNeighbor(dst, label);
   }

  long _num;                 // as max node id
  vector<Node> _nodes;       // nodes of graph
  map<long, long> _vid_map;  //(original VID, its position)
};

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  string filename = "/home/naughtycat/PregelPlus/data/test/test";
  string rltfile = "/home/naughtycat/PregelPlus/data/test/test";
  if (argc >= 3) {  // get 1 parameter: location
    filename = argv[1];
    rltfile = argv[2];
  }

  Graph g;
  g.loadGraph(filename, comm_sz);
  g.writeGraph(rltfile);

  MPI_Finalize();
}
