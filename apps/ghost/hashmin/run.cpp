#include "ghost_app_hashmin.h"

int main(int argc, char* argv[]){
	//$1 = input file path
	//$2 = part number
	//$3 = output file path
	string input = argv[1];
	string num = argv[2];
	string output = argv[3];
	input = input + "\t" + num;
	init_workers();
	set_ghost_threshold(100);//set to at least 100 for real large graphs
	ghost_hashmin(input, output, true);
	worker_finalize();
	return 0;
}
