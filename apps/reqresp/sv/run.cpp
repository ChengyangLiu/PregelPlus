#include "req_app_sv.h"

int main(int argc, char* argv[]){
	init_workers();
	req_sv("/home/naughtycat/PregelPlus/data/toy	1", "/home/naughtycat/PregelPlus/data/toy_req_sv");
	worker_finalize();
	return 0;
}
