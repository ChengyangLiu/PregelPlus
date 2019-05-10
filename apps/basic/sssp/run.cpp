#include "pregel_app_sssp.h"

int main(int argc, char* argv[]){
	//$1 = input file path
	//$2 = part number
	//$3 = output file path
	//$4 = query id
	string input = argv[1];
	string num = argv[2];
	string output = argv[3];
	long id = atol(argv[4]);
	input = input + "\t" + num;

	init_workers();
	pregel_sssp(id, input, output, true);
	worker_finalize();
	return 0;
}
