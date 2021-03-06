#all: counter_server counter_driver counter_coordinator\
counter_loader counter_add_node counter_delete_node\
counter_coordinator_driver counter_driver_mt counter_driver_noop_mt
#all: counter_server counter_driver_noop_mt counter_delete_node counter_add_node counter_loader counter_driver_mt
#all: echo_server echo_client echo_client_multicore rocksdb_client fb_client rocksdb_client_multicore rocksdb_merge_client echo_logserver rocksdb_server\
 rocksdb_merge_server rocksdb_loader fb_loader rocksdb_checkpoint fb_server

#all: echo_server echo_client echo_async_client
#all: echo_server echo_client pmemkv_client pmemkv_server

server: rocksdb_loader rocksdb_checkpoint
#server: blizmap_server

#client: echo_client echo_async_client pmemkv_client pmemkv_async_client
#client: pmemkv_async_client echo_async_client rocksdb_async_client
#client: blizmap_async_client
client: rocksdb_async_client

CXXFLAGS = -O3 -DBOOST_LOG_DYN_LINK -DBOOST_THREAD_DYN_LINK
CXXFLAGS += $(CPPFLAGS)
#CXXFLAGS = -O1 -g -fno-omit-frame-pointer -DBOOST_LOG_DYN_LINK
PMEM_SLIB?=/home/pfernando/pmdk/src/nondebug
PMEM_CLIB?=../pmdk/src/nondebug

LIBS = -lc ../core/libcyclone.a ../raft/libcraft.a -lboost_system -lboost_date_time -laio

#DPDK extras
RTE_CSDK?=/home/pfernando/dpdk
RTE_SSDK?=/home/pfernando/dpdk

CXXFLAGS += -DDPDK_STACK
CXXFLAGS += -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX

SERVER_CXXFLAGS= ${CXXFLAGS} -I${RTE_SSDK}/x86_64-native-linuxapp-gcc/include
CLIENT_CXXFLAGS= ${CXXFLAGS} -I${RTE_CSDK}/x86_64-native-linuxapp-gcc/include

SERVER_LIBS= ${LIBS} -L${RTE_SSDK}/x86_64-native-linuxapp-gcc/lib
CLIENT_LIBS= ${LIBS} -L${RTE_CSDK}/x86_64-native-linuxapp-gcc/lib

LIBS2 = -Wl,--whole-archive -Wl,-lrte_distributor -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_timer -Wl,-lrte_hash -Wl,-lrte_jobstats -Wl,-lrte_lpm -Wl,-lrte_power -Wl,-lrte_acl -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_vhost -Wl,-lm -Wl,--start-group -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_ip_frag -Wl,-lrte_ethdev -Wl,-lrte_net -Wl,-lrte_cryptodev -Wl,-lrte_mempool -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_cfgfile -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_null -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_vhost -Wl,-ldl -Wl,--end-group -Wl,--no-whole-archive

SERVER_LIBS += ${LIBS2}
CLIENT_LIBS += ${LIBS2}

SERVER_LIBS += ${PMEM_SLIB}/libpmemobj.a ${PMEM_SLIB}/libpmem.a -lndctl -ldaxctl -lpthread
CLIENT_LIBS += ${PMEM_CLIB}/libpmemobj.a ${PMEM_CLIB}/libpmem.a -lndctl -ldaxctl -lpthread

#Rocksdb 
ROCKS_HOME=/home/pfernando/rocksdb
ROCKS_CXXFLAGS=-std=c++11 -I${ROCKS_HOME}/include/ -L${ROCKS_HOME} -DROCKSDB_PLATFORM_POSIX -DROCKSDB_LIB_IO_POSIX  -DOS_LINUX -fno-builtin-memcmp -DROCKSDB_FALLOCATE_PRESENT -DSNAPPY -DGFLAGS=gflags -DZLIB -DBZIP2 -DZSTD -DNUMA -DROCKSDB_MALLOC_USABLE_SIZE -DROCKSDB_PTHREAD_ADAPTIVE_MUTEX -DROCKSDB_BACKTRACE -march=native  "-DROCKSDB_JEMALLOC"
ROCKS_LIBS += ${ROCKS_HOME}/librocksdb.a ${ROCKS_HOME}/libbz2.a ${ROCKS_HOME}/libz.a ${ROCKS_HOME}/libsnappy.a -lsnappy -lgflags -lz -lbz2 -lzstd -lpthread


#pmemkv
PMEMKV_HOME=../pmemkv
PMEMKV_CXXFLAGS=-std=c++11 -I${PMEMKV_HOME}/src 
PMEMKV_LIBS=${PMEMKV_HOME}/bin/libpmemkv.a

#containers
PMEMOBJ_CPP?=/home/pfernando/libpmemobj-cpp
BLIZMAP_CXXFLAGS=-I${PMEMOBJ_CPP}/include

#BOOST_THREAD_LIB=-lboost_thread-mt
BOOST_THREAD_LIB=-lboost_thread


blizmap_server:bliz_map.cpp
	$(CXX) $(SERVER_CXXFLAGS) $(BLIZMAP_CXXFLAGS)  bliz_map.cpp $(BOOST_THREAD_LIB) $(BLIZMAP_LIBS) $(SERVER_LIBS) -o $@

blizmap_async_client:bliz_map_client.cpp
	$(CXX) $(CLIENT_CXXFLAGS) bliz_map_client.cpp $(BOOST_THREAD_LIB) $(CLIENT_LIBS) -o $@

echo_server:echo_server.cpp
	$(CXX) $(SERVER_CXXFLAGS) echo_server.cpp $(BOOST_THREAD_LIB) $(SERVER_LIBS) -o $@

echo_logserver:echo_logserver.cpp
	$(CXX) $(CXXFLAGS) echo_logserver.cpp $(BOOST_THREAD_LIB) $(LIBS) -o $@

pmemkv_server:pmemkv_server.cpp
	$(CXX) $(CXXFLAGS) $(PMEMKV_CXXFLAGS) pmemkv_server.cpp $(BOOST_THREAD_LIB) $(PMEMKV_LIBS) $(SERVER_LIBS) -o $@

rocksdb_server:rocksdb_server.cpp
	$(CXX) $(CXXFLAGS) $(ROCKS_CXXFLAGS) rocksdb_server.cpp $(BOOST_THREAD_LIB) $(SERVER_LIBS) $(ROCKS_LIBS) -o $@

fb_server:fb_server.cpp
	$(CXX) $(CXXFLAGS) $(ROCKS_CXXFLAGS) fb_server.cpp $(BOOST_THREAD_LIB) $(LIBS) $(ROCKS_LIBS) -o $@

rocksdb_merge_server:rocksdb_merge_server.cpp
	$(CXX) $(CXXFLAGS) $(ROCKS_CXXFLAGS) rocksdb_merge_server.cpp $(BOOST_THREAD_LIB) $(LIBS) $(ROCKS_LIBS) -o $@

rocksdb_loader:rocksdb_loader.cpp
	$(CXX) $(CXXFLAGS) $(ROCKS_CXXFLAGS) rocksdb_loader.cpp $(BOOST_THREAD_LIB) $(LIBS) $(ROCKS_LIBS) -o $@

fb_loader:fb_loader.cpp
	$(CXX) $(CXXFLAGS) $(ROCKS_CXXFLAGS) fb_loader.cpp $(BOOST_THREAD_LIB) $(LIBS) $(ROCKS_LIBS) -o $@

rocksdb_checkpoint:rocksdb_checkpoint.cpp
	$(CXX) $(CXXFLAGS) $(ROCKS_CXXFLAGS) rocksdb_checkpoint.cpp $(BOOST_THREAD_LIB) $(LIBS) $(ROCKS_LIBS) -o $@

echo_client:echo_client.cpp 
	$(CXX) $(CLIENT_CXXFLAGS) echo_client.cpp $(BOOST_THREAD_LIB) $(CLIENT_LIBS) -o $@

echo_async_client:echo_async_client.cpp 
	$(CXX) $(CLIENT_CXXFLAGS) echo_async_client.cpp $(BOOST_THREAD_LIB) $(CLIENT_LIBS) -o $@

pmemkv_client:pmemkv_client.cpp
	$(CXX) $(CLIENT_CXXFLAGS) pmemkv_client.cpp $(BOOST_THREAD_LIB) $(CLIENT_LIBS) -o $@

pmemkv_async_client:pmemkv_async_client.cpp
	$(CXX) $(CLIENT_CXXFLAGS) pmemkv_async_client.cpp $(BOOST_THREAD_LIB) $(CLIENT_LIBS) -o $@

rocksdb_client:rocksdb_client.cpp 
	$(CXX) $(CLIENT_CXXFLAGS) rocksdb_client.cpp $(BOOST_THREAD_LIB) $(CLIENT_LIBS) -o $@

rocksdb_async_client:rocksdb_async_client.cpp 
	$(CXX) $(CLIENT_CXXFLAGS) rocksdb_async_client.cpp $(BOOST_THREAD_LIB) $(CLIENT_LIBS) -o $@

fb_client:fb_client.cpp 
	$(CXX) $(CXXFLAGS) -std=c++0x fb_client.cpp $(BOOST_THREAD_LIB) $(LIBS) -o $@

rocksdb_merge_client:rocksdb_merge_client.cpp 
	$(CXX) $(CXXFLAGS) rocksdb_merge_client.cpp $(BOOST_THREAD_LIB) $(LIBS) -o $@

rocksdb_client_multicore:rocksdb_client_multicore.cpp 
	$(CXX) $(CXXFLAGS) rocksdb_client_multicore.cpp $(BOOST_THREAD_LIB) $(LIBS) -o $@

echo_client_multicore:echo_client_multicore.cpp 
	$(CXX) $(CXXFLAGS) echo_client_multicore.cpp $(BOOST_THREAD_LIB) $(LIBS) -o $@

.PHONY:clean

clean:
	rm -f \
echo_server echo_client echo_async_client \
rocksdb_server rocksdb_client rocksdb_async_client \
rocksdb_loader rocksdb_checkpoint \
pmemkv_client pmemkv_async_client pmemkv_server
