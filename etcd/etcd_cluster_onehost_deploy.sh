#!/bin/bash

if [ command -v etcd ]; then

	mkdir -p node0 node1 node2
	etcd -name node0 -data-dir ./node0 -initial-advertise-peer-urls http://10.1.245.157:2380 \
	  -listen-peer-urls http://10.1.245.157:2380 \
	  -listen-client-urls http://10.1.245.157:2379,http://127.0.0.1:2379 \
	  -advertise-client-urls http://10.1.245.157:2379 \
	  -initial-cluster-token etcd-cluster-1 \
	  -initial-cluster node0=http://10.1.245.157:2380,node1=http://10.1.245.157:2382,node2=http://10.1.245.157:2384 \
	  -initial-cluster-state new &

	etcd -name node1 -data-dir ./node1 -initial-advertise-peer-urls http://10.1.245.157:2382 \
	  -listen-peer-urls http://10.1.245.157:2382 \
	  -listen-client-urls http://10.1.245.157:2381,http://127.0.0.1:2381 \
	  -advertise-client-urls http://10.1.245.157:2381 \
	  -initial-cluster-token etcd-cluster-1 \
	  -initial-cluster node0=http://10.1.245.157:2380,node1=http://10.1.245.157:2382,node2=http://10.1.245.157:2384 \
	  -initial-cluster-state new &

	etcd -name node2 -data-dir ./node2 -initial-advertise-peer-urls http://10.1.245.157:2384 \
	  -listen-peer-urls http://10.1.245.157:2384 \
	  -listen-client-urls http://10.1.245.157:2383,http://127.0.0.1:2383 \
	  -advertise-client-urls http://10.1.245.157:2383 \
	  -initial-cluster-token etcd-cluster-1 \
	  -initial-cluster node0=http://10.1.245.157:2380,node1=http://10.1.245.157:2382,node2=http://10.1.245.157:2384 \
	  -initial-cluster-state new &

fi
