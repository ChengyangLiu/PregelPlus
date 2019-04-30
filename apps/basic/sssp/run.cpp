#include "pregel_app_sssp.h"

int main(int argc, char* argv[]){
	init_workers();
	pregel_sssp(2, "/home/naughtycat/PregelPlus/data/toy_n	1", "/home/naughtycat/PregelPlus/data/toy_n_SSSP", true);
	worker_finalize();
	return 0;
}
