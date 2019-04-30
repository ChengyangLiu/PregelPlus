#include "pregel_app_fieldbcast.h"

int main(int argc, char* argv[]){
	init_workers();
	bool directed=false;
	pregel_fieldbcast("/home/naughtycat/PregelPlus/data/toy	1", "/home/naughtycat/PregelPlus/data/toy_fieldbcast", directed);
	worker_finalize();
	return 0;
}
