#include <float.h>
#include "basic/pregel-dev.h"
using namespace std;
// Matrix factorization R(socre) = P(user,feature)*Q(feature,item)
// input line format: vid	label N	v1	el1	v2	el2	...
// user's label is 0, item's label is 1.
// output line format: loss value

// feature number
#define FEATURE_NUM 5
// regular coefficient
#define LAMBDA 0.01
// learning rate for SGD
#define ETA 0.00005
// halt condition
#define LOSS_THRESHOLD 100
#define ITERATION 5000
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

struct MFEdge_pregel {
  int nb;
  score_type score;
};

ibinstream& operator<<(ibinstream& m, const MFEdge_pregel& v) {
  m << v.nb;
  m << v.score;
  return m;
}

obinstream& operator>>(obinstream& m, MFEdge_pregel& v) {
  m >> v.nb;
  m >> v.score;
  return m;
}

struct MFValue_pregel {
  int label;  // 0 is user, 1 is item
  K_Dim dim;
  vector<MFEdge_pregel> edges;  // user has children, item has parents
  double loss;
};

ibinstream& operator<<(ibinstream& m, const MFValue_pregel& v) {
  m << v.label;
  m << v.dim;
  m << v.edges;
  m << v.loss;
  return m;
}

obinstream& operator>>(obinstream& m, MFValue_pregel& v) {
  m >> v.label;
  m >> v.dim;
  m >> v.edges;
  m >> v.loss;
  return m;
}

//====================================

struct MFMsg_pregel {
  score_type score;
  K_Dim msg_dim;
};

ibinstream& operator<<(ibinstream& m, const MFMsg_pregel& v) {
  m << v.score;
  m << v.msg_dim;
  return m;
}

obinstream& operator>>(obinstream& m, MFMsg_pregel& v) {
  m >> v.score;
  m >> v.msg_dim;
  return m;
}

//====================================
// statistics between workers
struct Loss {
  double last_last_round;
  double last_round;
  double now;
};

ibinstream& operator<<(ibinstream& m, const Loss& v) {
  m << v.last_last_round;
  m << v.last_round;
  m << v.now;
  return m;
}

obinstream& operator>>(obinstream& m, Loss& v) {
  m >> v.last_last_round;
  m >> v.last_round;
  m >> v.now;
  return m;
}

//====================================

class MFVertex_pregel : public Vertex<VertexID, MFValue_pregel, MFMsg_pregel> {
 public:
  void broadcast() {
    vector<MFEdge_pregel>& nbs = value().edges;
    for (int i = 0; i < nbs.size(); i++) {
      MFMsg_pregel msg;
      msg.score = nbs[i].score;
      msg.msg_dim = value().dim;
      send_message(nbs[i].nb, msg);  // send MSG to its children/parents
    }
  }

  virtual void compute(MessageContainer& messages) {
    if (step_num() == 1) {
      broadcast();
    } else {
      Loss* agg = (Loss*)getAgg();
      // check whether loss does not change, or iteration max
      if (fabs(agg->last_round - agg->last_last_round) < LOSS_THRESHOLD ||
          step_num() == ITERATION) {
        vote_to_halt();
      } else {
        if (value().label == 0) {  // user
          auto& loss = value().loss;
          loss = 0.0;
          auto p_dim = value().dim;
          for (size_t i = 0; i < messages.size(); i++) {
            MFMsg_pregel msg = messages[i];
            auto& q_dim = msg.msg_dim;
            double val = p_dim.dot(q_dim) - msg.score;
            auto grad = q_dim * val + p_dim * LAMBDA;  // calculate gradient
            value().dim -= grad * ETA;                 // update dim
            loss += val * val;
          }
        } else {  // item
          auto p_dim = value().dim;
          for (size_t i = 0; i < messages.size(); i++) {
            MFMsg_pregel msg = messages[i];
            auto& q_dim = msg.msg_dim;
            double val = p_dim.dot(q_dim) - msg.score;
            auto grad = q_dim * val + p_dim * LAMBDA;  // calculate gradient
            value().dim -= grad * ETA;                 // update dim
          }
        }
        broadcast();
      }
    }
  }

  virtual void print() {}
};

class MFAgg_pregel : public Aggregator<MFVertex_pregel, Loss, Loss> {
 private:
  Loss loss;

 public:
  // all workers init loss
  virtual void init() {
    if (step_num() == 1) {
      loss.last_last_round = 0.0;
      loss.last_round = 0.0;
      loss.now = DBL_MAX;
    } else {
      loss.now = 0.0;
      if (get_worker_id() == 0) {
        printf("Round Loss:%e\n", loss.last_round);
      }
    }
  }
  // all workers calculate loss of user
  virtual void stepPartial(MFVertex_pregel* v) {
    if (step_num() != 1) {
      if (v->value().label == 0) {
        loss.now += v->value().loss;
      }
    }
  }
  // aggregate partial loss
  virtual Loss* finishPartial() { return &loss; }

  // sum partial loss in master (worker0)
  virtual void stepFinal(Loss* part_loss) {
    if (step_num() != 1) {
      loss.now += (*part_loss).now;
    }
  }

  // broadcast loss statistics to all workers
  virtual Loss* finishFinal() {
    loss.last_last_round = loss.last_round;
    loss.last_round = loss.now;
    return &loss;
  }
};

class MFWorker_pregel : public Worker<MFVertex_pregel, MFAgg_pregel> {
  char buf[1000];

 public:
  // input line:
  // seperate with "\t"
  // vid  vlabel N	v1	el1	v2	el2	...
  virtual MFVertex_pregel* toVertex(char* line) {
    char* pch;
    pch = strtok(line, "\t");
    MFVertex_pregel* v = new MFVertex_pregel;
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
      MFEdge_pregel edge = {nb, score};
      v->value().edges.push_back(edge);
    }
    return v;
  }

  // output line:
  // vid \t loss
  virtual void toline(MFVertex_pregel* v, BufferedWriter& writer) {
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
  MFWorker_pregel worker;
  // all messages from neighbors are needed, so don't set combiner.
  // MFCombiner_pregel combiner;
  // if (use_combiner) worker.setCombiner(&combiner);
  MFAgg_pregel agg;
  worker.setAggregator(&agg);
  worker.run(param);
}
