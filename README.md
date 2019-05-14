# PregelPlus: A Distributed Graph Computing Framework with Effective Message Reduction

Pregel+ is not just another open-source Pregel implementation, but a substantially improved distributed graph computing system with effective message reduction. Compared with existing Pregel-like systems, Pregel+ provides simpler programming interface and yet achieves higher computational efficiency. We give ample examples and detailed tutorials to demonstrate how to program in Pregel+ and deploy Pregel+ in a distributed environment. Pregel+ is also a better choice for researchers who want to change the system to support new functionalities, as the design of Pregel+ is much simpler and more flexible than most other Pregel-like systems.

Pregel+ supports two effective message reduction techniques: (1)vertex mirroring and (2)a new request-respond paradigm. These techniques not only reduce the total number of messages exchanged through the network, but also bound the number of messages sent/received by any vertex, especially for processing power-law graphs and (relatively) dense graphs.

## Project Website
[http://www.cse.cuhk.edu.hk/pregelplus/](http://www.cse.cuhk.edu.hk/pregelplus/index.html)

## License

Copyright 2018 Husky Data Lab, CUHK

## Update For Tianhe (2019.5.14)

1. Remove HDFS IO, implement local IO, so run much easily in tianhe.

2. How to compile in tianhe.

   Firstly set $SYSTEM_PATH in Makefile, then copy it to any APP directory, and use command "make run_tianhe",  e.g.
   ```script
   # Please set $SYSTEM_PATH in Makefile
   $ cp ./Makefile ./apps/basic/sssp/
   $ cd ./apps/basic/sssp/
   $ make run_tianhe
   ```
3. How to run.

   Run it just like running any MPI program in tianhe.

4. Some programs for graph partition are in tools directory.
