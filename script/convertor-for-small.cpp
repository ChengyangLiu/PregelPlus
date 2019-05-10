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

using namespace std;

/* Convert ver file to .adj file.
 * Only be used for small graph (both numbers of v and e are below 50 million or less)
 * @Input: $1=file path; $2=part number; $3=output path
 * @Output: an adj file
 * @Author: chengyangliu*/

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
  void loadGraph(string& filename, int p_num) {
    string rfile = filename + ".part" + to_string(p_num);
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
        addVertex(src);
      }
      fin_r.close();

      ifstream fin_v(vfile);
      // update vertex
      while (getline(fin_v, line)) {
        stringstream ss(line);
        ss >> src >> label;
        updateVInfo(src, label);
      }
      fin_v.close();

      ifstream fin_e(efile);
      // read edge in
      while (getline(fin_e, line)) {
        stringstream ss(line);
        ss >> src >> dst >> label;
        addEdge(src, dst, label);
      }
      fin_e.close();
    } catch (exception& e) {
      cout << e.what();
      exit(0);
    }
  }

  /* write graph to file */
  void writeGraph(string& filename, int p_num) {
    ofstream fout[p_num];
    for (int i = 0; i < p_num; i++) {
      fout[i].open(filename + to_string(i));
    }
    for (auto& node : _nodes) {
      Vertex& vertex = node.v();
      vector<long>& neighbors = node.neighbors();
      vector<long>& elabels = node.elabels();
      long id = vertex.id();
      int part = v_parts[id];
      fout[part] << id;
      long len = neighbors.size();
      if (len == 0) {
        fout[part] << "\t0\n";
      } else {
        fout[part] << "\t" << len;
        int i;
        for (i = 0; i < len; i++) {
          fout[part] << "\t" << neighbors[i] << "\t" << elabels[i];
        }
        fout[part] << "\n";
      }
    }
    for (int i = 0; i < p_num; i++) {
      fout[i].close();
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

  string filename = "/home/naughtycat/PregelPlus/data/test/test";
  int p_num = 64;
  string rltfile = "/home/naughtycat/PregelPlus/data/test/test";
  if (argc >= 4) {  // get 1 parameter: location
    filename = argv[1];
    p_num = atoi(argv[2]);
    rltfile = argv[3];
  }

  Graph g;
  g.loadGraph(filename, p_num);
  g.writeGraph(rltfile, p_num);

}
