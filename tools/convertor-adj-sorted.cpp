#include <stdio.h>
#include <string.h>

/* Convert .e file to .adj file.
 * Only be used for sorted .e file or continous edges (it means all edges from
 * one vertex are continous in file)
 * @Input: $1=efile path; $2=output path
 * @Output: an adj file
 * @Author: chengyangliu*/

#define BUF_SIZE 10000

int main(int argc, char** argv) {
  char efile[100] = "/home/naughtycat/PregelPlus/data/exp/vldb2014.e";
  char resfile[100] = "/home/naughtycat/PregelPlus/data/exp/vldb2014.adj";
  if (argc >= 3) {  // get 1 parameter: location
    strcpy(efile, argv[1]);
    strcpy(resfile, argv[2]);
  }

  FILE* fin = fopen(efile, "r");
  FILE* fout = fopen(resfile, "w");
  unsigned int dst[BUF_SIZE];
  unsigned int label[BUF_SIZE];
  size_t len = 0;
  unsigned int src;
  unsigned int present_id = 0;
  unsigned int cnt = 0;
  // read vertex in

  while (fscanf(fin, "%u%u%u", &src, &dst[len], &label[len]) != EOF) {
    //printf("%u\t%u\t%u\n", src, dst[len], cnt);
    if (++cnt % 10000000 == 0) {  // counting
      printf("E:%u\n", cnt);
    }
    if (src != present_id) {
      // write present id
      fprintf(fout, "%u\t%u", present_id, (unsigned int)len);
      for (size_t i = 0; i < len; i++) {
        fprintf(fout, "\t%u\t%u", dst[i], label[i]);
      }
      fprintf(fout, "\n");
      // move next
      present_id = src;
      // clear list
      dst[0] = dst[len];
      label[0] = label[len];
      len = 0;
    }
    len++;
  }
  fprintf(fout, "%u\t%u", present_id, (unsigned int)len);
  for (size_t i = 0; i < len; i++) {
    fprintf(fout, "\t%u\t%u", dst[i], label[i]);
  }
  fprintf(fout, "\n");

  fclose(fin);
  fclose(fout);
}
