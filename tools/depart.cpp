#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include "mpi.h"

/* Convert adj and rfile to many part-files.
 * @Input: $1=r file path; $2=adj file path; $3=output path; $4=N (part number)
 * @Output: N part files
 * @Author: chengyangliu*/

#define BUF_SIZE 50000

 std::map<unsigned int, int> which_p;

 int main(int argc, char** argv) {
   int vote = 0;
   int flag = 0;
   int myrank, numproces;
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
   MPI_Comm_size(MPI_COMM_WORLD, &numproces);

   char rfile[100] = "/home/naughtycat/PregelPlus/data/exp/vldb2014.part3";
   char adjfile[100] = "/home/naughtycat/PregelPlus/data/exp/vldb2014.adj";
   char resfile[100] = "/home/naughtycat/PregelPlus/data/exp/vldb2014";
   size_t part_num = 1;
   if (argc >= 3) {  // get 1 parameter: location
     strcpy(rfile, argv[1]);
     strcpy(adjfile, argv[2]);
     strcpy(resfile, argv[3]);
     part_num = atoi(argv[4]);
   }

   int part;
   unsigned int src;
   unsigned int cnt = 0;
   char buf[BUF_SIZE];

   // read r in
   FILE* finr = fopen(rfile, "r");
   while (fscanf(finr, "%u%u", &src, &part) != EOF) {
     if (++cnt % 5000000 == 0) {
       printf("[Worker%d]: R%u\n", myrank, cnt);
     }
     if (part % numproces == myrank) {
       which_p[src] = part;
     }
   }
   fclose(finr);

   cnt = 0;
   // open adj file
   FILE* finadj = fopen(adjfile, "r");
   // open outfile
   FILE* fout[part_num];
   for (size_t i = 0; i < part_num; i++) {
     if (i % numproces == myrank) {
       char partfile[100];
       strcpy(partfile, resfile);
       strcat(partfile, std::to_string(i).c_str());
       fout[i] = fopen(partfile, "w");
     }
   }

   // depart
   while (fscanf(finadj, "%u%[^\n]", &src, buf) != EOF) {
     if (++cnt % 5000000 == 0) {
       printf("[Worker%d]: V%u\n", myrank, cnt);
     }
     auto it = which_p.find(src);
     if (it != which_p.end()) {
       fprintf(fout[it->second], "%u%s\n", src, buf);
     }
   }

   // close adj file
   fclose(finadj);
   // close outfile
   for (size_t i = 0; i < part_num; i++) {
     if (i % numproces == myrank) {
       fclose(fout[i]);
     }
   }

   // exit
   if (numproces == 1) {
     MPI_Finalize();
   } else {
     if (myrank == 0) {
       for (size_t source = 1; source < numproces; source++) {
         MPI_Recv(&vote, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       }
       for (size_t source = 1; source < numproces; source++) {
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
