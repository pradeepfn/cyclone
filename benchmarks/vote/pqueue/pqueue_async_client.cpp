#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <rte_launch.h>
#include <rte_malloc.h>

#include <vector>

#include "../../../core/logging.hpp"
#include "../../../core/clock.hpp"
#include "../../../core/libcyclone.hpp"
#include "../../common/genzip.hpp"

#include "dpdk_client.hpp"
#include "hashmap/hashmap-client.h"
#include "priority-queue/priority-queue.h"

/* IMPORTANT - set to large enough value */
unsigned long pmemds_keys = 1000000;
double alpha = 1.08;
int nreqs = 20;

/* pmem structure names */
const uint16_t hashmap_st = 0;
const uint16_t pq_st = 1;


int driver(void *arg);

typedef struct driver_args_st
{
	int leader;
	int me;
	int mc;
	int replicas;
	int clients;
	int partitions;
	int buf_cap;
	pmemdsclient::DPDKPMClient *dpdkClient;
	pmemdsclient::HashMapEngine *hashMap;
	pmemdsclient::priority_queue *prio_queue;
	void **handles;
	void operator() ()
	{
		(void)driver((void *)this);
	}
} driver_args_t;

int driver(void *arg)
{
	unsigned long request_id = 0UL; // wrap around at MAX
	driver_args_t *dargs = (driver_args_t *)arg;
	int me = dargs->me;
	int mc = dargs->mc;
	int replicas = dargs->replicas;
	int clients = dargs->clients;
	int partitions = dargs->partitions;
	void **handles = dargs->handles;
	// char *buffer = new char[DISP_MAX_MSGSIZE];
	pmemdsclient::DPDKPMClient *pmlib = dargs->dpdkClient;
	pmemdsclient::HashMapEngine *hashMap = dargs->hashMap;
	pmemdsclient::priority_queue *prio_queue = dargs->prio_queue;
	//struct proposal *prop = (struct proposal *)buffer;
	srand(time(NULL));
	int ret;
	int rpc_flags;
	int my_core;

	unsigned  long key;
	char article_name[16]; // noria uses varchar[16] for its benchmark
	unsigned long keys = pmemds_keys;

  double frac_read = ((double)(nreqs-1))/nreqs;	
	BOOST_LOG_TRIVIAL(info) << "FRAC_READ = " << frac_read;
	BOOST_LOG_TRIVIAL(info) << "KEYS = " << keys;
	BOOST_LOG_TRIVIAL(info) << "ZIPFIAN (ALPHA) = " << alpha;

	srand(rtc_clock::current_time());
  pmlib->open("voteApp",nullptr);
	uint8_t creation_flag = 0;
	
	hashMap->create(creation_flag,nullptr);
	prio_queue->create(creation_flag,nullptr);

	// populate articles
	for(int i = 0; i < keys; i++){
	 snprintf(article_name,16,"Article #%lu",key);
	 hashmap->put(i,article_name);
	 prio_queue->insert(i,0); // 0 votes initially
	}

	for( int rcount = 0 ; ; rcount = ++rcount%nreqs){
		key = zipf(keys,alpha);
		if(rcount){ // read request
			pmlib->topk(key, nullptr);
		}else{ // update request
			prio_queue->increase_prio(key);
		}
	}
	hashMap->close(nullptr);
	prio_queue->close(nullptr);
	pmlib->close(nullptr);
	return 0;
}

int main(int argc, const char *argv[])
{
	if (argc != 11)
	{
		printf("Usage: %s client_id_start client_id_stop mc replicas clients partitions cluster_config quorum_config_prefix server_ports inflight_cap\n", argv[0]);
		exit(-1);
	}

	int client_id_start = atoi(argv[1]);
	int client_id_stop  = atoi(argv[2]);
	driver_args_t *dargs;
	void **prev_handles;
	cyclone_network_init(argv[7], 1, atoi(argv[3]), 2 + client_id_stop - client_id_start);
	driver_args_t ** dargs_array =
		(driver_args_t **)malloc((client_id_stop - client_id_start) * sizeof(driver_args_t *));
	for (int me = client_id_start; me < client_id_stop; me++)
	{
		dargs = (driver_args_t *) malloc(sizeof(driver_args_t));
		dargs_array[me - client_id_start] = dargs;
		if (me == client_id_start)
		{
			dargs->leader = 1;
		}
		else
		{
			dargs->leader = 0;
		}
		dargs->me = me;
		dargs->mc = atoi(argv[3]);
		dargs->replicas = atoi(argv[4]);
		dargs->clients  = atoi(argv[5]);
		dargs->partitions = atoi(argv[6]);
		dargs->buf_cap = atoi(argv[10]);
		dargs->handles = new void *[dargs->partitions];
		BOOST_LOG_TRIVIAL(info) << "no. of partitions: " << dargs->partitions;
		char fname_server[50]/* IMPORTANT - set to large enough value */;
		char fname_client[50];
		for (int i = 0; i < dargs->partitions; i++){
			sprintf(fname_server, "%s", argv[7]);
			sprintf(fname_client, "%s%d.ini", argv[8], i); // only one raft instance running
			dargs->handles[i] = cyclone_client_init(dargs->me,
					dargs->mc,
					1 + me - client_id_start,
					fname_server,
					atoi(argv[9]),
					fname_client,
					CLIENT_ASYNC,
					dargs->buf_cap);
			BOOST_LOG_TRIVIAL(info) << "init dpdkclient and hashmap";
			dargs->dpdkClient = new pmemdsclient::DPDKPMClient(dargs->handles[i]);
			//TBD: priority_queue and hashmap
			dargs->hashMap = new pmemdsclient::HashMapEngine(dargs->dpdkClient,hashmap_st,1000,1UL);
			dargs->prio_queue = new pmemdsclient::PriorityQueueEngine(dargs->dpdkClient,hashmap_st,1000,1UL);
		}
	}
	for (int me = client_id_start; me < client_id_stop; me++){
		cyclone_launch_clients(dargs_array[me-client_id_start]->handles[0],driver, dargs_array[me-client_id_start], 1+  me-client_id_start);
	}
	rte_eal_mp_wait_lcore();
}