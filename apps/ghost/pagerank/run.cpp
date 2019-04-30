#include "ghost_app_pagerank.h"

int main(int argc, char* argv[]){
	init_workers();
	set_ghost_threshold(2);//set to at least 100 for real large graphs
	ghost_pagerank("/home/naughtycat/PregelPlus/data/toy	1", "/home/naughtycat/PregelPlus/data/toy_ghost_PR", true);
	worker_finalize();
	return 0;
}
