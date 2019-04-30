#include "pregel_app_mst.h"

int main(int argc, char* argv[])
{
    init_workers();
    pregel_mst("/home/naughtycat/PregelPlus/data/toy_w	1", "/home/naughtycat/PregelPlus/data/toy_w_MSF");
    worker_finalize();
    return 0;
}
