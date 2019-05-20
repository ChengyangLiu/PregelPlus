#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Check whether the adj file is right.
 * One vertex in different postion in file will be detected.
 * All vertex without edge will be detected.
 * @Input: $1=adjfile path; $2=total vertex number
 * @Output: a adj file containing zero vertex
 * @Author: chengyangliu*/

 int main(int argc, char** argv) {

   char adjfile[100] = "/home/naughtycat/PregelPlus/data/exp/vldb2014.adj";
   unsigned int maxn = 0;
   if (argc >= 3) {  // get 1 parameter: location
     strcpy(adjfile, argv[1]);
     maxn = atol(argv[2]);
   }
   printf("checking v = %u\n", maxn);

   unsigned int list[maxn]{0};
   unsigned int src;
   unsigned int cnt = 0;

   FILE* fin = fopen(adjfile, "r");
   // read vertex in
   while (fscanf(fin, "%u%*c%*[^\n]", &src) != EOF) {
      if (++cnt % 10000000 == 0) {
        printf("checked:%u\n", cnt);
      }
      list[src]++;
   }
   fclose(fin);

   bool flag = true;
   cnt = 0;
   strcat(adjfile, "-zero");
   FILE* fout = fopen(adjfile, "w");
   for (size_t i = 0; i < maxn; i++) {
     if (list[i] == 1) {

     } else if (list[i] == 0) {
       cnt++;
       fprintf(fout, "%u\t0\n", (unsigned int)i);
     } else {
       flag = false;
       printf("%u:%u\n", (unsigned int)i, list[i]);
     }
   }
   fclose(fout);

   if (flag) {
     printf("right adj file\n");
     printf("zero vertex number:%u\n", cnt);
   } else {
     printf("wrong line\n");
   }
 }
