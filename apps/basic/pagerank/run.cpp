#include "pregel_app_pagerank.h"

int main(int argc, char* argv[]){
	//$1 = input file path
	//$2 = part number
	//$3 = output file path
	//$4 = iteration number
	string input = argv[1];
	string num = argv[2];
	string output = argv[3];
	int round = atoi(argv[4]);
	input = input + "\t" + num;

	init_workers();
	set_pagerank_round(round);
	pregel_pagerank(input, output, true);
	worker_finalize();
	return 0;
}
