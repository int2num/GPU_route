#include "LagSerial.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include<vector>
#include<algorithm>
#include <utility>
#include <time.h>
#include<math.h>
#include <unordered_map>
#include"service.h"
#include"dijkstra.h"
#include"BFS.h"
#include"taskPath.h"
#include"const.h"
#include"routemask.h"
#include"PathArrange.h"
#include<fstream>
using namespace std;
bool UDgreater5(pair<int, float> elem1, pair<int, float> elem2)
{
	return elem1.second > elem2.second;
}
bool UPGservice5(service s1, service s2)
{
	return s1.d>s2.d;
}
bool cmp5(float a, float b)
{
	return a<b;
}
LagSerial::LagSerial(Graph &_G):G(_G),StoreRoute(Task, vector<int>(1, -1)), BestRoute(Task, vector<int>())
{
	st = new int[Task*sizeof(int)];
	te = new int[Task*sizeof(int)];
	pd = new float[Task*sizeof(float)];
	capacity = (float*)malloc(EDge*sizeof(float));
	d = (float*)malloc(Task*NODE*sizeof(float));
	pre = (int*)malloc(Task*NODE*sizeof(int));
	lambda = new float[EDge*sizeof(float)];
	mask = new int[Task];
	for (int i = 0; i < NODE; i++)
		{
			for (int j = 0; j < Task; j++)
			{
				if (st[j] == i)
				{
					d[i + j*NODE] = 0.0;
					pre[i + j*NODE] = -1;
				}
				else
				{
					d[i + j*NODE] = 100000.0;
					pre[i + j*NODE] = -1;
				}
			}
		}
}
vector<pair<string,float> > LagSerial::dijkstraSerial(vector<service> &s,ostream& Out){
	printf("Lagrange serial searching..............\n");
	srand(time(NULL));
	float start = float(1000*clock()) / CLOCKS_PER_SEC;
	for (int i = 0; i < Task; i++)
	{
		st[i] = s[i].s;
		te[i] = s[i].t;
		pd[i] = (float)s[i].d;
	}
	for (int i = 0; i < EDge; i++)
	{
		lambda[i] = 0;
	}
	for (int i = 0; i < Task; i++)
			mask[i] = i;
	double totalflow = 0;
	for (int i = 0; i < Task; i++)
		totalflow += INFHOPS *pd[i];
	int num = Task;
	int mum = EDge;
	int stillS = num;
	vector<RouteMark> bestroutes;
	float best = 0;
	int reme = 0;
	int count = 0;
	int bestround = 0;
	int bround = 0;
	int zeor = 0;
    best = totalflow;
	double bestadd = totalflow;
	int totaliter = 0;
	vector<float>middata;
	for (int i = 0; i < 1000000; i++)
	{
		reme++;
		for (int j = 0; j < stillS; j++)
		{
			int s = st[mask[j]];
			int t = te[mask[j]];
			dijkstra(&G, s, t, d + mask[j] * NODE, pre + mask[j] * NODE, lambda);
		}
		int n = 0;
		int value = rearrange(&G, capacity, lambda, pre, d, pd, te, st, num, mum, bestadd, stillS, 1, NODE, StoreRoute, BestRoute, mask, Out, bestroutes, totalflow);
		if (value<best)
		{
			best = value;
			reme = 0;
		}
		middata.push_back(value);
		time_t raend = 1000*clock()/CLOCKS_PER_SEC;
		if (stillS == 0 || reme>loomore||((raend-start)>EXPIRE&&GANOEX<0))
			break;
		totaliter = i+1;
	}
	float end=float(1000*clock())/ CLOCKS_PER_SEC;
	vector<pair<int, vector<int> > > result = GrabResult(BestRoute, num, mum, pd);
	int addin = result.size();
	pair<float,int> tf=CheckR(&G, result,s,string("Lag_Serial"));
	writejsoniter(LAGSFILE,middata,string("Lag_Serial"));
	vector<pair<string,float>> rdata;
	rdata.push_back(make_pair(string("object"),best));
	rdata.push_back(make_pair(string("inf_obj"),totalflow));
	rdata.push_back(make_pair(string("task_add_in"),addin));
	rdata.push_back(make_pair(string("flow_add_in"),tf.first));
		rdata.push_back(make_pair(string("total_weight"),tf.second));
	rdata.push_back(make_pair(string("time"),(end-start)));
	rdata.push_back(make_pair(string("iter_num"),totaliter));
	rdata.push_back(make_pair(string("iter_time"),(float)(end-start)/(float)totaliter));
	writejsondata(DATAFILE,rdata,string("Lag_Serial"));
	return rdata;
}

LagSerial::~LagSerial()
{
	delete[] st;
	delete[] te;
	delete[] pd;
	free(d);
	free(pre);
	delete[] lambda;
	delete[] mask;
	free(capacity);
}



