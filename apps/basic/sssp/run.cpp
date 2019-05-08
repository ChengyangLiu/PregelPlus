#include "pregel_app_sssp.h"

int main(int argc, char* argv[]){
	long id = atol(argv[1]);
	string input = argv[2];
	string num = argv[3];
	string output = argv[4];
	input = input + "\t" + num;
	//cout << id << ", I: " << input << ", O: " << output << "\n";
	init_workers();
	//pregel_sssp(1000, "/BIGDATA1/buaa_wffan_1/chengyangliu/data/usaroad/part96/usaroad    96", "/BIGDATA1/buaa_wffan_1/chengyangliu/data/usaroad/part96/usaroad_sssp_1000.rlt", true);
	pregel_sssp(id, input, output, true);
	worker_finalize();
	return 0;
}
