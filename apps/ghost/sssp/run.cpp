#include "ghost_app_sssp.h"

int main(int argc, char* argv[]){
	//$1 = mirroring threshold
	//$2 = input file path
	//$3 = part number
	//$4 = output file path
	//$5 = query id
	int tau = atoi(argv[1]);
	string input = argv[2];
	string num = argv[3];
	string output = argv[4];
	long id = atol(argv[5]);
	input = input + "\t" + num;

	init_workers();
	set_ghost_threshold(tau);//set to at least 100 for real large graphs
	ghost_sssp(id, input, output, true);
	worker_finalize();
	return 0;
}
