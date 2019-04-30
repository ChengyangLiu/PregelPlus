#include "pregel_app_svplus.h"

int main(int argc, char* argv[]){
	init_workers();
	pregel_sv("/home/naughtycat/PregelPlus/data/toy	1", "/home/naughtycat/PregelPlus/data/toy_svplus");
	worker_finalize();
	return 0;
}
