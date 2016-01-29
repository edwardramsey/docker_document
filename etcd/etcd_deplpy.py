#!/usr/bin/env python
# coding=utf-8

'''
    deploy script for etcd cluster

'''

from string import Template
import paramiko

def conn(ip, user, pwd):
    try:
        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        client.connect()
    except paramiko.ssh_exception.BadHostKeyException as e:
        print 'bad host key exception'
    except paramiko.ssh_exception.AuthenticationException:
        print 'AuthenticationException'
    except paramiko.ssh_exception.SSHException:
        print 'SSHException'
    except 


def composeCommand(host_list):
    '''
    host_list example 10.0.1.11, 10.0.1.12
    '''
    hosts_str = ""
    for ip in host_list:
        hosts_str += "node%d=http://%s:2380,"
    hosts_str = hosts_str[:-1]
    basic_cmd = '''
etcd -name $name -initial-advertise-peer-urls http://$host_ip:2380 \
  -listen-peer-urls http://$host_ip:2380 \
  -listen-client-urls http://$host_ip:2379,http://127.0.0.1:2379 \
  -advertise-client-urls http://$host_ip:2379 \
  -initial-cluster-token $token_name\
  -initial-cluster $hosts_str\
  -initial-cluster-state new
'''
    return Template(basic_cmd).safe_substitute()
#infra0=http://10.0.1.10:2380,infra1=http://10.0.1.11:2380,infra2=http://10.0.1.12:2380
'''
etcd -name infra1 -initial-advertise-peer-urls http://10.0.1.11:2380 \
  -listen-peer-urls http://10.0.1.11:2380 \
  -listen-client-urls http://10.0.1.11:2379,http://127.0.0.1:2379 \
  -advertise-client-urls http://10.0.1.11:2379 \
  -initial-cluster-token etcd-cluster-1 \
  -initial-cluster infra0=http://10.0.1.10:2380,infra1=http://10.0.1.11:2380,infra2=http://10.0.1.12:2380 \
  -initial-cluster-state new
'''

if __name__ == '__main__':
    pass
