#include "pregel_app_hashmin.h"

int main(int argc, char* argv[]){
	init_workers();
	pregel_hashmin("/home/naughtycat/PregelPlus/data/toy	1", "/home/naughtycat/PregelPlus/data/toy_hashmin", true);
	worker_finalize();
	return 0;
}
