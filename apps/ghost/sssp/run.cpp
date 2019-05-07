#include "ghost_app_sssp.h"

int main(int argc, char* argv[]){
	long id = atoi(argv[1]);
	string input = argv[2];
	string num = argv[3];
	string output = argv[4];
	input = input + "\t" + num;
	init_workers();
	set_ghost_threshold(100);//set to at least 100 for real large graphs
	//ghost_sssp(2, "/home/naughtycat/PregelPlus/data/exp/toy	1", "/home/naughtycat/PregelPlus/data/exp/toy_ghost_SSSP", true);
	ghost_sssp(id, input, output, true);
	worker_finalize();
	return 0;
}
