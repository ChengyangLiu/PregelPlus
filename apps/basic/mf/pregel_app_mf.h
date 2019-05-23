#include <float.h>
#include "basic/pregel-dev.h"
using namespace std;
// Matrix factorization R(socre) = P(user,feature)*Q(feature,item)
// input line format: vid	label N	v1	el1	v2	el2	...
// user's label is 0, item's label is 1.
// output line format: loss value

#define FEATURE_NUM 5
#define LAMBDA 0.01
#define ETA 0.00005
#define LOSS_THRESHOLD 0.000001
#define ITERATION INT_MAX
typedef double score_type;

class K_Dim {
  // Vector with the same length
 public:
  K_Dim() {}
  ~K_Dim() {}

  K_Dim(const size_t n) : _vec(n) {
    for (size_t i = 0; i < _vec.size(); i++) {
      _vec[i] = (1.0 * rand() / RAND_MAX - 0.5) * 2.0;
    }
  }

  void init(const size_t n) {
    for (size_t i = 0; i < n; i++) {
      _vec.emplace_back((1.0 * rand() / RAND_MAX - 0.5) * 2.0);
    }
  }

  inline size_t size() const { return _vec.size(); }

  inline double& operator[](const size_t i) { return _vec[i]; }

  inline const double& get(const size_t i) const { return _vec[i]; }

  K_Dim& operator+=(const K_Dim& that) {
    for (size_t i = 0; i < _vec.size(); i++) {
      _vec[i] += that.get(i);
    }
    return *this;
  }

  K_Dim& operator-=(const K_Dim& that) {
    for (size_t i = 0; i < _vec.size(); i++) {
      _vec[i] -= that.get(i);
    }
    return *this;
  }

  K_Dim& operator*=(double x) {
    for (size_t i = 0; i < _vec.size(); i++) {
      _vec[i] *= x;
    }
    return *this;
  }

  K_Dim operator+(const K_Dim& that) const {
    K_Dim res(*this);
    res += that;
    return res;
  }

  K_Dim operator-(const K_Dim& that) const {
    K_Dim res(*this);
    res -= that;
    return res;
  }

  K_Dim operator*(double x) const {
    K_Dim res(*this);
    res *= x;
    return res;
  }

  double dot(const K_Dim& that) const {
    double res = 0.0;
    for (size_t i = 0; i < _vec.size(); ++i) {
      res += _vec[i] * that.get(i);
    }
    return res;
  }

 public:
  vector<double> _vec;
};

ibinstream& operator<<(ibinstream& m, const K_Dim& v) { m << v._vec; }
obinstream& operator>>(obinstream& m, K_Dim& v) { m >> v._vec; }

//====================================
class SPVertex_pregel;

struct SPEdge_pregel {
  int nb;
  score_type score;
};

ibinstream& operator<<(ibinstream& m, const SPEdge_pregel& v) {
  m << v.nb;
  m << v.score;
  return m;
}

obinstream& operator>>(obinstream& m, SPEdge_pregel& v) {
  m >> v.nb;
  m >> v.score;
  return m;
}

struct SPValue_pregel {
  int label;  // 0 is user, 1 is item
  K_Dim dim;
  vector<SPEdge_pregel> edges;  // user has children, item has parents
  double loss;
};

ibinstream& operator<<(ibinstream& m, const SPValue_pregel& v) {
  m << v.label;
  m << v.dim;
  m << v.edges;
  m << v.loss;
  return m;
}

obinstream& operator>>(obinstream& m, SPValue_pregel& v) {
  m >> v.label;
  m >> v.dim;
  m >> v.edges;
  m >> v.loss;
  return m;
}

//====================================

struct SPMsg_pregel {
  score_type score;
  K_Dim msg_dim;
};

ibinstream& operator<<(ibinstream& m, const SPMsg_pregel& v) {
  m << v.score;
  m << v.msg_dim;
  return m;
}

obinstream& operator>>(obinstream& m, SPMsg_pregel& v) {
  m >> v.score;
  m >> v.msg_dim;
  return m;
}

//====================================

class SPVertex_pregel : public Vertex<VertexID, SPValue_pregel, SPMsg_pregel> {
 public:
  void broadcast() {
    vector<SPEdge_pregel>& nbs = value().edges;
    for (int i = 0; i < nbs.size(); i++) {
      SPMsg_pregel msg;
      msg.score = nbs[i].score;
      msg.msg_dim = value().dim;
      send_message(nbs[i].nb, msg);  // send MSG to its children/parents
    }
  }

  virtual void compute(MessageContainer& messages) {
    if (step_num() == 1) {
      broadcast();
    } else {
      double loss = 0.0;
      size_t k = messages.size();
      auto p_dim = value().dim;
      for (size_t i = 0; i < k; i++) {
        SPMsg_pregel msg = messages[i];
        auto& q_dim = msg.msg_dim;
        double val = p_dim.dot(q_dim) - msg.score;
        auto grad = q_dim * val + p_dim * LAMBDA;  // calculate gradient
        value().dim -= grad * ETA;                 // update dim
        loss += val * val;
      }
      loss /= k;
      loss = sqrt(loss);
      value().loss = loss;  // update loss
      if (loss < LOSS_THRESHOLD ||
          step_num() == ITERATION) {  // v loss = sqrt(sigma((r-pq)^2)/k)
        vote_to_halt();
      } else {
        broadcast();
      }
    }
  }

  virtual void print() {}
};

class SPWorker_pregel : public Worker<SPVertex_pregel> {
  char buf[1000];

 public:
  // input line:
  // seperate with "\t"
  // vid  vlabel N	v1	el1	v2	el2	...
  virtual SPVertex_pregel* toVertex(char* line) {
    char* pch;
    pch = strtok(line, "\t");
    SPVertex_pregel* v = new SPVertex_pregel;
    int id = atoi(pch);
    v->id = id;
    v->value().dim.init(FEATURE_NUM);
    pch = strtok(NULL, "\t");
    int label = atoi(pch);
    v->value().label = label;
    pch = strtok(NULL, "\t");
    int num = atoi(pch);
    for (int i = 0; i < num; i++) {
      pch = strtok(NULL, "\t");
      int nb = atoi(pch);
      pch = strtok(NULL, "\t");
      score_type score = (score_type)atof(pch);
      SPEdge_pregel edge = {nb, score};
      v->value().edges.push_back(edge);
    }
    return v;
  }

  // output line:
  // vid \t loss
  virtual void toline(SPVertex_pregel* v, BufferedWriter& writer) {
    sprintf(buf, "%d\t%e\n", v->id, v->value().loss);
    writer.write(buf);
  }
};

void pregel_mf(string in_path, string out_path, bool use_combiner) {
  WorkerParams param;
  param.input_path = in_path;
  param.output_path = out_path;
  param.force_write = true;
  param.native_dispatcher = false;
  SPWorker_pregel worker;
  // all messages from neighbors are needed, so don't set combiner.
  // SPCombiner_pregel combiner;
  // if (use_combiner) worker.setCombiner(&combiner);
  worker.run(param);
}
