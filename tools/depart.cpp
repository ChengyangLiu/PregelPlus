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
#include <string.h>
#include <math.h>
#include "mpi.h"

using namespace std;

/* Convert adj and rfile to many part-files.
 * @Input: $1=r file path; $2=adj file path; $3=output path; $4=N (part number)
 * @Output: N part files
 * @Author: chengyangliu*/

 map<long, int> which_p;

 int main(int argc, char** argv) {
   int vote = 0;
   int flag = 0;
   int myrank, numproces;

   string rfile = argv[1];
   string adjfile = argv[2];
   string resfile = argv[3];
   int part_num = atoi(argv[4]);

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
   MPI_Comm_size(MPI_COMM_WORLD, &numproces);

   string line;
   long src;
   int part;
   long cnt = 0;

   // read r in
   ifstream fin_r(rfile);
   while (getline(fin_r, line)) {
     if (++cnt % 5000000 == 0) {
       cout << "[Worker" << myrank << "]: R " << cnt << "\n";
     }
      stringstream ss(line);
      ss >> src >> part;
      if (part % numproces == myrank) {
        which_p[src] = part;
      }
   }
   fin_r.close();

   cnt = 0;
   // open adj file
   ifstream fin_adj(adjfile);
   // open outfile
   ofstream fout[part_num];
   for (int i = 0; i < part_num; i++) {
     if (i % numproces == myrank) {
       fout[i].open(resfile + to_string(i));
     }
   }

   // depart
   while (getline(fin_adj, line)) {
     if (++cnt % 5000000 == 0) {
       cout << "[Worker" << myrank << "]: V " << cnt << "\n";
     }
     stringstream ss(line);
     ss >> src;
     auto it = which_p.find(src);
     if (it != which_p.end()) {
       fout[it->second] << line << "\n";
     }
   }

   // close adj file
   fin_adj.close();
   // close outfile
   for (int i = 0; i < part_num; i++) {
     if (i % numproces == myrank) {
       fout[i].close();
     }
   }

   // exit
   if (numproces == 1) {
     MPI_Finalize();
   } else {
     if (myrank == 0) {
       for (int source = 1; source < numproces; source++) {
         MPI_Recv(&vote, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       }
       for (int source = 1; source < numproces; source++) {
         flag = 1;
         MPI_Send(&flag, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
       }
       MPI_Finalize();
     } else {
       vote = 1;
       MPI_Send(&vote, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
       MPI_Recv(&flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       MPI_Finalize();
     }
   }
 }
