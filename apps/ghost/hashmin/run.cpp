#include "ghost_app_hashmin.h"

int main(int argc, char* argv[]){
	init_workers();
	set_ghost_threshold(2);//set to at least 100 for real large graphs
	ghost_hashmin("/home/naughtycat/PregelPlus/data/toy	1", "/home/naughtycat/PregelPlus/data/toy_ghost_hashmin", true);
	worker_finalize();
	return 0;
}
