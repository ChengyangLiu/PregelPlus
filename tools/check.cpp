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

using namespace std;

/* Check whether the adj file is right.
 * One vertex in different postion in file will be detected.
 * All vertex without edge will be detected.
 * @Input: $1=adjfile path; $2=total vertex number
 * @Output: a adj file containing zero vertex
 * @Author: chengyangliu*/

 int main(int argc, char** argv) {

   string adjfile = argv[1];
   long maxn = atol(argv[2]);
   cout << "checking v = " << maxn << "\n";

   vector<long> list (maxn, 0);

   string line;
   long src;
   long cnt = 0;

   ifstream fin(adjfile);
   // read vertex in
   while (getline(fin, line)) {
      if (++cnt % 1000000 == 0) {
         cout << "checked:" << cnt << "\n";
      }
      stringstream ss(line);
      ss >> src;
      list[src]++;
   }
   fin.close();

   bool flag = true;
   cnt = 0;
   ofstream fc;
   fc.open(adjfile + "-zero");
   for (long i = 0; i < maxn; i++) {
     if (list[i] == 1) {

     } else if (list[i] == 0) {
       cnt++;
       fc << i << "\t" << 0 << "\n";
     } else {
       flag = false;
       cout << i << ":" << list[i] << "\n";
     }
   }
   fc.close();

   if (flag) {
     cout << "Right adj file\n";
     cout << "zero vertex number:" << cnt << "\n";
   } else {
     cout << "wrong line\n";
   }
 }
