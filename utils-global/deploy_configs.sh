#!/bin/bash
if [ $# -ne 4 ]
    then echo "Usage $0 config_dir deploy_dir bench_name bench_subname"
    exit
fi
output_dir=$1
deploy_dir=$2
bench_name=$3
bench_subname=$4
echo "config dir = $output_dir"
echo "deploy dir = $deploy_dir"


for i in ${output_dir}/* 
do
    if [ -d "$i" ] ; then
	node=$(basename $i)
	ip=`cat ${i}/ip_address`
	clush -w ${ip} rm -rf ${deploy_dir}/${node}
	clush -w ${ip} mkdir ${deploy_dir}/${node}
	echo "deploying configs to node $node"
	echo '#!/bin/bash' > exec_servers.sh
	echo "#export LD_LIBRARY_PATH=/usr/lib:/usr/local/lib" >> exec_servers.sh
	echo "#export PATH=$PATH:~/cyclone/cyclone.git/benchmarks/${bench_name}/${bench_subname}" >> exec_servers.sh
	#echo "export PATH=$PATH:$deploy_dir/cyclone.git/test" >> exec_servers.sh
	echo "ulimit -c unlimited" >> exec_servers.sh
	echo "cd $deploy_dir/$node" >> exec_servers.sh
#echo "export MALLOC_CHECK_=3" >> exec_servers.sh
	cp exec_servers.sh exec_inactive_servers.sh
	cp exec_servers.sh exec_preload.sh
	cp exec_servers.sh exec_clients.sh
	cp exec_servers.sh killall.sh
	echo "source launch_servers" >> exec_servers.sh
	echo "source launch_inactive_servers" >> exec_inactive_servers.sh
	echo "source launch_preload" >> exec_preload.sh
	echo "source launch_clients" >> exec_clients.sh
	echo "source kill_all" >> killall.sh
	chmod u+x exec_servers.sh
	chmod u+x exec_inactive_servers.sh
	chmod u+x exec_preload.sh
	chmod u+x exec_clients.sh
	chmod u+x killall.sh
	scp ${i}/* ${ip}:${deploy_dir}/${node}
	if [ -f "$i/launch_servers" ] ; then
	    scp exec_servers.sh ${ip}:${deploy_dir}/${node}
	else
		echo '#!/bin/bash' > build_client.sh
		echo 'pushd ' ${deploy_dir}'/cyclone.git' >> build_client.sh
		echo 'unzip -q client_src.zip' >> build_client.sh
		echo 'cd core;make clean;make' >> build_client.sh
		echo 'cd ..' >> build_client.sh
		echo 'cd benchmarks/'${bench_name}'/'${bench_subname}';make clean;make client' >> build_client.sh
		echo 'popd' >> build_client.sh
		chmod u+x build_client.sh
	    scp build_client.sh ${ip}:${deploy_dir}/${node}
	fi
	if [ -f "$i/launch_inactive_servers" ] ; then
	    scp exec_inactive_servers.sh ${ip}:${deploy_dir}/${node}
	fi
	scp exec_preload.sh ${ip}:${deploy_dir}/${node}
	scp exec_clients.sh ${ip}:${deploy_dir}/${node}
	scp killall.sh ${ip}:${deploy_dir}/${node}
    fi
done


