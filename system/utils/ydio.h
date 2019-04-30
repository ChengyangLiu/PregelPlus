#ifndef YDIO_H
#define YDIO_H

#include "mpi.h"
#include <string.h> //memcpy, memchr
#include <stdlib.h> //realloc
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include "global.h"
using namespace std;

const char* newLine = "\n";

//====== BufferedWriter ======
struct BufferedWriter {
    const char* path;
    vector<char> buf;
    int rank;

    BufferedWriter(const char* outpath, int rank): path(outpath), rank(rank) {}

    ~BufferedWriter() {
      try {
        char fname[100];
        sprintf(fname, "%s.part%d", path, rank);
        std::ofstream fout(fname);
        //TODO: more efficient function
        for(char const& b : buf) {
          fout << b;
        }
        fout.close();
      } catch(exception& e) {
        fprintf(stderr, "Failed to write file!\n");
        exit(-1);
      }
    }

    void write(const char* content)
    {
      int len = strlen(content);
      buf.insert(buf.end(), content, content + len);
    }
};

vector<vector<string> >* dispatch(const char* para)
// NOTICE!!!!!!!!!!!!!!!!!!!!!!
// multi-path is not supported!!!
// para is consisted with "filename" and "part number suffix (total number)", departed with "\t"
// missing files may cause fatal error!!!
// Only load graph randomly
// Load balancing is not supported!!! PLEASE SEPERATE GRAPH WELL AND DEFINE TASK TO EACH WORKER!!!
{
    string str = para;
    string::size_type pos = str.find("\t");
    if (pos == str.npos) {
      fprintf(stderr, "Failed to analyze input!\n");
      exit(-1);
    }
    string fname_prefix = str.substr(0, pos);
    int numFiles = atoi(str.substr(pos + 1, str.length() - pos -1).c_str());

    vector<vector<string> >* assignmentPointer = new vector<vector<string> >(_num_workers);
    vector<vector<string> >& assignment = *assignmentPointer;

    for (int task_suffix = 0; task_suffix < numFiles; task_suffix++) {
      char fname[100];
      sprintf(fname, "%s%d", fname_prefix.c_str(), task_suffix);
      assignment[task_suffix%_num_workers].push_back(fname);
    }

    return assignmentPointer;
}

vector<vector<string> >* dispatch(vector<string> para)
{ // TODO:: make it right
  string str = para[0];
  string::size_type pos = str.find("\t");
  if (pos == str.npos) {
    fprintf(stderr, "Failed to analyze input!\n");
    exit(-1);
  }
  string fname_prefix = str.substr(0, pos);
  int numFiles = atoi(str.substr(pos + 1, str.length() - pos -1).c_str());

  vector<vector<string> >* assignmentPointer = new vector<vector<string> >(_num_workers);
  vector<vector<string> >& assignment = *assignmentPointer;


  for (int task_suffix = 0; task_suffix < numFiles; task_suffix++) {
    char fname[100];
    sprintf(fname, "%s%d", fname_prefix.c_str(), task_suffix);
    assignment[task_suffix%_num_workers].push_back(fname);
  }

  return assignmentPointer;
}

#endif
