#include "pregel_app_svplus.h"

int main(int argc, char* argv[]){
	//$1 = input file path
	//$2 = part number
	//$3 = output file path
	string input = argv[1];
	string num = argv[2];
	string output = argv[3];
	input = input + "\t" + num;

	init_workers();
	pregel_sv(input, output);
	worker_finalize();
	return 0;
}
