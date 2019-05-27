#ifndef YDIO_H
#define YDIO_H

#include <stdlib.h>  //realloc
#include <string.h>  //memcpy, memchr
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "global.h"
#include "mpi.h"
using namespace std;

const char* newLine = "\n";

//====== BufferedWriter ======
struct BufferedWriter {
  const char* path;
  vector<char> buf;
  int rank;

  BufferedWriter(const char* outpath, int rank) : path(outpath), rank(rank) {}

  ~BufferedWriter() {
    try {
      char fname[100];
      sprintf(fname, "%s.part%d", path, rank);
      std::ofstream fout(fname);
      // TODO: more efficient function
      for (char const& b : buf) {
        fout << b;
      }
      fout.close();
    } catch (exception& e) {
      fprintf(stderr, "Failed to write file!\n");
      exit(-1);
    }
  }

  void write(const char* content) {
    int len = strlen(content);
    buf.insert(buf.end(), content, content + len);
  }
};

vector<vector<string> >* dispatch(const char* para)
// NOTICE!!!!!!!!!!!!!!!!!!!!!!
// multi-path is not supported!!!
// para is filename prefix
// missing files may cause fatal error!!!
{
  string filename_prefix = para;

  vector<vector<string> >* assignmentPointer =
      new vector<vector<string> >(_num_workers);
  vector<vector<string> >& assignment = *assignmentPointer;

  for (int task_suffix = 0; task_suffix < _num_workers; task_suffix++) {
    assignment[task_suffix].push_back(filename_prefix);
  }

  return assignmentPointer;
}

vector<vector<string> >* dispatch(
    vector<string> para) {  // TODO:: make it right
  string filename_prefix = para[0];

  vector<vector<string> >* assignmentPointer =
      new vector<vector<string> >(_num_workers);
  vector<vector<string> >& assignment = *assignmentPointer;

  for (int task_suffix = 0; task_suffix < _num_workers; task_suffix++) {
    assignment[task_suffix].push_back(filename_prefix);
  }

  return assignmentPointer;
}

#endif
