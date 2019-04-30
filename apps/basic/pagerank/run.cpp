#include "pregel_app_pagerank.h"

int main(int argc, char* argv[]){
	init_workers();
	pregel_pagerank("/home/naughtycat/PregelPlus/data/toy	1", "/home/naughtycat/PregelPlus/data/toy_PR", true);
	worker_finalize();
	return 0;
}
