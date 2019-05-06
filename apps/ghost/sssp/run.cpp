#include "ghost_app_sssp.h"

int main(int argc, char* argv[]){
	init_workers();
	set_ghost_threshold(100);//set to at least 100 for real large graphs
	ghost_sssp(2, "/home/naughtycat/PregelPlus/data/exp/toy	1", "/home/naughtycat/PregelPlus/data/exp/toy_ghost_SSSP", true);
	worker_finalize();
	return 0;
}
