#include "ghost_app_pagerank.h"

int main(int argc, char* argv[]){
	//$1 = mirroring threshold
	//$2 = input file path
	//$3 = part number
	//$4 = output file path
	//$5 = iteration number
	int tau = atoi(argv[1]);
	string input = argv[2];
	string num = argv[3];
	string output = argv[4];
	int round = atoi(argv[5]);
	input = input + "\t" + num;
	
	init_workers();
	set_pagerank_round(round);
	set_ghost_threshold(tau);//set to at least 100 for real large graphs
	ghost_pagerank(input, output, true);
	worker_finalize();
	return 0;
}
