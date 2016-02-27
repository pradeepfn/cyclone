#!/bin/bash
GROUP='arch-h[2-30/2].dp.intel.com'

clush -w ${GROUP} killall -9 rbtree_map_server
clush -w ${GROUP} killall -9 rbtree_map_coordinator
clush -w ${GROUP} killall -9 rbtree_map_coordinator_driver
clush -w ${GROUP} killall -9 rbtree_map_synchronous_driver
clush -w ${GROUP} killall -9 rbtree_map_driver
clush -w ${GROUP} killall -9 rbtree_map_ro_driver
clush -w ${GROUP} killall -9 rbtree_map_partitioned_driver
clush -w ${GROUP} killall -9 cyclone_test

