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

/* Convert .e file to .adj file.
 * Only be used for sorted .e file or continous edges (it means all edges from one vertex are continous in file)
 * @Input: $1=efile path; $2=output path
 * @Output: an adj file
 * @Author: chengyangliu*/

 int main(int argc, char** argv) {

   string efile = "/home/naughtycat/PregelPlus/data/test/test.e";
   string resfile = "/home/naughtycat/PregelPlus/data/test/test.adj";
   if (argc >= 3) {  // get 1 parameter: location
     efile = argv[1];
     resfile = argv[2];
   }

   string line;
   long src, dst, label;
   long pos_id = 0;

   vector<long> dst_list;
   vector<long> label_list;

   ifstream fin(efile);
   ofstream fout(resfile);
   // read vertex in
   pos_id = 0;
   long cnt = 0;
   while (getline(fin, line)) {
      // counting
      if (++cnt % 5000000 == 0) {
        cout << "E:" << cnt << "\n";
      }
      // read in
      stringstream ss(line);
      ss >> src >> dst >> label;
      if (src != pos_id) {
        // write present id
        int len = dst_list.size();
        fout << pos_id << "\t" << len;
        for (int i = 0; i < len; i++) {
          fout << "\t" << dst_list[i] << "\t" << label_list[i];
        }
        fout << "\n";
        // move next
        pos_id = src;
        // clear list
        dst_list.clear();
        dst_list.shrink_to_fit();
        label_list.clear();
        label_list.shrink_to_fit();
      }
      dst_list.emplace_back(dst);
      label_list.emplace_back(label);
   }
   int len = dst_list.size();
   if (len != 0) {
     fout << pos_id << "\t" << len;
     for (int i = 0; i < len; i++) {
       fout << "\t" << dst_list[i] << "\t" << label_list[i];
     }
     fout << "\n";
   }

   fin.close();
   fout.close();
 }
