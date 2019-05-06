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

/* vertex of a graph */
class Vertex {
 public:
  Vertex() {}
  ~Vertex() {}

  Vertex(long id, long label) : _id(id), _label(label) {}

  Vertex(long id) : _id(id) {}

  void addLabel(long label) { _label = label; }

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

  Node(Vertex& vertex) { _vertex = vertex; }

  /* add a neighborhood vertex to an exist vertex */
  void addNeighbor(Vertex* neighbor, long elabel) {
    _neighbors.emplace_back(neighbor);
    _elabels.emplace_back(elabel);
  }

  inline Vertex& v() { return _vertex; }
  inline vector<Vertex*>& neighbors() { return _neighbors; }
  inline vector<long>& elabels() { return _elabels; }

 private:
  Vertex _vertex;              // source
  vector<Vertex*> _neighbors;  // destinations
  vector<long> _elabels;       // labels
};
class Graph;
map<int, Graph> gmap;
map<long, int> v_parts;

/* class graph */
class Graph {
 public:
  Graph() : _num(0) {}
  Graph(long gid) : _num(0), _gid(gid) {}
  ~Graph() {}

  void addVertex(long id) {
    Vertex v(id);
    Node node(v);
    _nodes.emplace_back(node);
    _vid_map[id] = _num++;
  }

  void updateVInfo(long id, long label) {
    Node* node = Graph::node(id);
    if (node == NULL) {
      cout << "src id is not existed in its fragment when updating!\n";
      exit(-1);
    }
    node->v().addLabel(label);
  }

  // addEdge must be called after addVertex
  void addEdge(long src, long dst, long label) {
    Node* src_node = Graph::node(src);
    if (src_node == NULL) {
      cout << "src id is not existed in its fragment when adding edge!\n";
      exit(-1);
    }
    Node* dst_node = Graph::node(dst);
    if (dst_node == NULL) {
      // cout << "[warning]: " << dst << " is in anothor fragment," << src <<
      // "\n";
      dst_node = gmap[v_parts[dst]].node(dst);
      if (dst_node == NULL) {
        cout << "dst is not existed in all fragments!\n";
        exit(-1);
      }
    }
    src_node->addNeighbor(&(dst_node->v()), label);
  }
  /* write graph to file */
  void writeGraph(string& rltfile) {
    long cnt = 0;
    ofstream fout(rltfile);
    for (auto& node : _nodes) {
      Vertex& vertex = node.v();
      vector<Vertex*> neighbors = node.neighbors();
      vector<long> elabels = node.elabels();
      fout << vertex.id() << "\t" << vertex.label();
      long len = neighbors.size();
      if (len == 0) {
        fout << "\t0\n";
      } else {
        fout << "\t" << len;
        int i;
        for (i = 0; i < len; i++) {
          fout << "\t" << neighbors[i]->id() << "\t" << elabels[i];
        }
        fout << "\n";
      }
      if (++cnt % 100000 == 0) {
        cout << "G" << _gid << ":" << cnt << "\n";
      }
    }
  }

  inline Node* node(long vid) {
    return _vid_map.find(vid) != _vid_map.end() ? &_nodes[_vid_map[vid]] : NULL;
  }

 private:
  int _gid;
  long _num;                 // as max node id
  vector<Node> _nodes;       // nodes of graph
  map<long, long> _vid_map;  //(original VID, its position)
};

/* load graph */
void loadFile(string& filename, string& partnum) {
  string rfile = filename + ".part" + partnum;
  string vfile = filename + ".v";
  string efile = filename + ".e";

  string line;
  long src, dst, label, part;
  long cnt = 0;
  try {
    ifstream fin_r(rfile);
    // read vertex in
    while (getline(fin_r, line)) {
      stringstream ss(line);
      ss >> src >> part;
      gmap[part].addVertex(src);
      v_parts[src] = part;
      if (++cnt % 100000 == 0) {
        cout << "R:" << cnt << "\n";
      }
    }
    fin_r.close();

    cnt = 0;
    ifstream fin_v(vfile);
    // update vertex
    while (getline(fin_v, line)) {
      stringstream ss(line);
      ss >> src >> label;
      gmap[v_parts[src]].updateVInfo(src, label);
      if (++cnt % 100000 == 0) {
        cout << "V:" << cnt << "\n";
      }
    }
    fin_v.close();

    cnt = 0;
    ifstream fin_e(efile);
    // read edge in
    while (getline(fin_e, line)) {
      stringstream ss(line);
      ss >> src >> dst >> label;
      gmap[v_parts[src]].addEdge(src, dst, label);
      if (++cnt % 500000 == 0) {
        cout << "E:" << cnt << "\n";
      }
    }
    fin_e.close();
  } catch (exception& e) {
    cout << e.what();
    exit(0);
  }
}

void writeFile(string& filename) {
  for (auto it : gmap) {
    string rltfile = filename + to_string(it.first);
    Graph& g = it.second;
    g.writeGraph(rltfile);
  }
}

int main(int argc, char** argv) {
  string filename = "/home/naughtycat/PregelPlus/data/test/test";
  string partnum = "4";
  string rltfile = "/home/naughtycat/PregelPlus/data/test/test";
  if (argc >= 4) {  // get 1 parameter: location
    filename = argv[1];
    partnum = argv[2];
    rltfile = argv[3];
  } else {
    cout << "Parameter <file path> must be needed!";
    exit(0);
  }

  int gnum = atoi(partnum.c_str());
  for (int i = 0; i < gnum; i++) {
    Graph g(i);
    gmap[i] = g;
  }

  loadFile(filename, partnum);
  writeFile(rltfile);
}
