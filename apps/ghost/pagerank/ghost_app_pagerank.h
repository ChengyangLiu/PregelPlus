#include "ghost/ghost-dev.h"
using namespace std;

//input line format: vid	N	v1	el1	v2	el2	...
//output line format: v \t PageRank(v) ...

//an aggregator collects PageRank(v) for all dangling vertices, which is then redistributed to all vertices in the next superstep
//equivalent to adding edges from a dangling vertex to all vertices in the graph

struct PRValue_ghost
{
	double pr;
	int deg;
};

ibinstream & operator<<(ibinstream & m, const PRValue_ghost & v){
	m<<v.pr;
	m<<v.deg;
	return m;
}

obinstream & operator>>(obinstream & m, PRValue_ghost & v){
	m>>v.pr;
	m>>v.deg;
	return m;
}

//====================================

class PRVertex_ghost:public GVertex<VertexID, PRValue_ghost, double>
{
	public:
		virtual void compute(MessageContainer & messages)
		{
			if(step_num()==1)
			{
				value().pr=1.0/get_vnum();
			}
			else
			{
				double sum=0;
				for(MessageIter it=messages.begin(); it!=messages.end(); it++)
				{
					sum+=*it;
				}
				double* agg=(double*)getAgg();
				double residual=*agg/get_vnum();
				value().pr=0.15/get_vnum()+0.85*(sum+residual);
			}
			if(step_num()<ROUND)
			{
				double msg=value().pr/value().deg;
				broadcast(msg);
			}
			else vote_to_halt();
		}

};

//====================================

class PRAgg_ghost:public Aggregator<PRVertex_ghost, double, double>
{
	private:
		double sum;
	public:
		virtual void init(){
			sum=0;
		}

		virtual void stepPartial(PRVertex_ghost* v)
		{
			if(v->value().deg==0) sum+=v->value().pr;
		}

		virtual void stepFinal(double* part)
		{
			sum+=*part;
		}

		virtual double* finishPartial(){ return &sum; }
		virtual double* finishFinal(){ return &sum; }
};

class PRWorker_ghost:public GWorker<PRVertex_ghost, PRAgg_ghost>
{
	char buf[100];
	public:
		// seperate with "\t"
		// vid	N	v1	el1	v2	el2	...
		virtual PRVertex_ghost* toVertex(char* line)
		{
			char * pch;
			pch=strtok(line, "\t");
			PRVertex_ghost* v=new PRVertex_ghost;
			v->id=atoi(pch);

			pch=strtok(NULL, "\t");
			int num=atoi(pch);
			v->value().deg=atoi(pch);
			EdgeContainer & edges=v->neighbors();
			for(int i=0; i<num; i++)
			{
				pch=strtok(NULL, "\t");
				EdgeT edge;
				edge.id=atoi(pch);
				edges.push_back(edge);
				pch=strtok(NULL, "\t"); //filter elabel
			}
			return v;
		}

		virtual void toline(PRVertex_ghost* v, BufferedWriter & writer)
		{ // use e format for pr-value (graph may be very huge)
			sprintf(buf, "%d\t%e\n", v->id, v->value().pr);
			writer.write(buf);
		}
};

class PRCombiner_ghost:public Combiner<double>
{
	public:
		virtual void combine(double & old, const double & new_msg)
		{
			old+=new_msg;
		}
};

void ghost_pagerank(string in_path, string out_path, bool use_combiner){
	WorkerParams param;
	param.input_path=in_path;
	param.output_path=out_path;
	param.force_write=true;
	param.native_dispatcher=false;
	PRWorker_ghost worker;
	PRCombiner_ghost combiner;
	if(use_combiner) worker.setCombiner(&combiner);
	PRAgg_ghost agg;
	worker.setAggregator(&agg);
	worker.run(param);
}
