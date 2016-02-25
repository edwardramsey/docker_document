#!/usr/bin/env python
# coding=utf-8

import os

default_cluster_deploy_conf='''
10.1.245.157 root
10.1.245.158 root
10.1.245.159 root
'''

default_single_deploy_conf='''
10.1.245.157 root Abc@1234
'''


'''
case1 单机集群模式
'''
host_ip = '10.1.245.157'
os.system('python ./etcd_deplpy.py -u')
