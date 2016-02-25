#!/usr/bin/env python
# coding=utf-8

'''
    Deploy script for etcd cluster
    steps:
    1. read ip-username-password from deploy.conf
    2. compose deployment command
    3. execute command with pyhon paramiko

    script input parameter:
        -u: username and password
        -k: use public key
        -f: output command to sh file, name -> etcd_ip.sh
'''

from string import Template
import paramiko
import sys
import time


def conn(ip, user, pwd):
    try:
        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        client.connect(ip, 22, user, pwd, timeout=5)
        return client
    except IOError, e:
        print e, "please check host ip"
    except paramiko.PasswordRequiredException, e:
        print e, "please check username"
    except paramiko.ssh_exception.AuthenticationException, e:
        print e.message, "please check password"
    except paramiko.ssh_exception.SSHException, e:
        print e


def conn_publickey(ip, user, key_file):
    try:
        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        client.connect(ip, 22, user, key_filename=key_file)
        return client
    except paramiko.ssh_exception.BadHostKeyException, e:
        print e


def composeCommand(host_list, cluster_size=3):
    '''
    compose etcd start command

    @param
    host_list       ip list
    cluster_size    default size is 3 
    '''

    hosts_str = ""
    d_var = dict()
    basic_cmd = '''
etcd -name node$index -data-dir ./node$index \
  -initial-advertise-peer-urls http://$host_ip:$port \
  -listen-peer-urls http://$host_ip:$port \
  -listen-client-urls http://$host_ip:$port1,http://127.0.0.1:$port1 \
  -advertise-client-urls http://$host_ip:$port1 \
  -initial-cluster-token $token_name \
  -initial-cluster $hosts_str \
  -initial-cluster-state new &
'''

    # etcd environment check command template:
    # command check, instances check, port check
    check_cmd_template = '''#!/bin/bash
mkdir -p etcd_cluster && cd $_
if [ ! $(command -v etcd) ];then
    echo "etcd command not exist..."
    exit
fi
ETCD_COUNT=`ps -ef|grep etcd|grep -v grep|wc -l`
if [ $ETCD_COUNT -eq %d ];then
    echo "already exit"
    exit
fi
PORT_COUNT=`lsof -i:%d|grep -v COMMAND|wc -l`
if [ $PORT_COUNT -lt 0 ];then
    echo "port error" 
fi
'''

    test_cmd_template='''
COUNT=`ps -ef|grep etcd_cluster|grep -v grep|wc -l`
if [ $COUNT -eq %d ];then
    echo "start etcd cluster OK"
else
    echo "start etcd cluster error"
fi
'''

    cmd = []
    if len(host_list) == 1:
        print 'single host deploy cluster...'

        check_cmd = check_cmd_template % (cluster_size, 2380)
        cmd.append(check_cmd)

        for index in range(cluster_size):
            hosts_str += "node{num}=http://{ip}:{port},"\
                .format(num = index, ip = host_list[0], port = 2380+2*index)
        hosts_str = hosts_str[:-1]

        for index in range(cluster_size):
            d_var['index'] = index
            d_var['host_ip'] = host_list[0]
            d_var['port'] = 2380 + index*2
            d_var['port1'] = 2379 + index*2
            d_var['token_name'] = "etcd_cluster"
            d_var['hosts_str'] = hosts_str
            cmd[0] += Template(basic_cmd).safe_substitute(d_var)

        test_cmd = test_cmd_template % (cluster_size)
        cmd[0] += test_cmd
        
    elif len(host_list) > 1:
        print 'multi host cluster...'
        check_cmd = check_cmd_template % (1, 2380)
        test_cmd = test_cmd_template % (1)

        for index, ip in enumerate(host_list):
            hosts_str += "node{num}=http://{ip}:{port},"\
                    .format(num = index, ip=ip, port=2380)
        hosts_str = hosts_str[:-1]

        for index, ip in enumerate(host_list):
            d_var['index'] = index
            d_var['host_ip'] = ip
            d_var['port'] = 2380
            d_var['port1'] = 2379
            d_var['token_name'] = "etcd_cluster"
            d_var['hosts_str'] = hosts_str
            cmd.append(check_cmd)
            cmd[index] += Template(basic_cmd).safe_substitute(d_var)
            cmd[index] += test_cmd
    else:
        print 'no host in list'
        return None
    
    return cmd


def read_conf(conf_name):
    record = dict()
    with open(conf_name, "r") as f:
        for line in iter(f.readline, ''):
            item = line.split()
            if len(item) == 2:
                record[item[0]] = [item[1]]
            elif len(item) > 2:
                record[item[0]] = [item[1], item[2]]
            else:
                print 'deploy..conf file error'
    print record
    return record


def write_to_script(ip_list):
    cmd = composeCommand(ip_list)
    for i, ip in enumerate(ip_list):
        file_name = "etcd_%s.sh" % ip
        with open(file_name, "w+") as f:
            f.write(cmd[i])
            print "write file [%s] OK" % file_name


def exec_cmd(conf_name, mode, public_key=None):
    '''
    dict[ip](username/password) <==> conf dict[ip](username/password)
    '''
    record = read_conf(conf_name)
    cmd = composeCommand(record.keys())
    count = 0
    for ip, user_pwd in record.items():
        if mode == "u":
            client = conn(ip, user_pwd[0], user_pwd[1])
        else:
            client = conn_publickey(ip, user_pwd[0], public_key)
        if client is not None:
            # print cmd[count]
            stdin, out, err = client.exec_command(cmd[count])
            time.sleep(1)
            print out.readline(1024)
            print err.readline(1024)
            client.close()
        else:
            print 'client errror'
        count += 1


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print '''
usage:
    -f (your_ip)                output in sh script
    -u                          use username and password to login
    -k (public_key_location)    use username and public key to login

    default ip conf is deploy.conf
'''
    conf_name = "deploy.conf" 
    host_list = []
    if len(sys.argv) > 1:
        if sys.argv[1] == "-f":
            if len(sys.argv) == 2:
                record = read_conf(conf_name)
                write_to_script(record.keys())
            else:
                write_to_script(sys.argv[2:])

        if sys.argv[1] == "-u":
            exec_cmd(conf_name, "u")

        if sys.argv[1] == "-k":
            public_key = "~/.ssh/id_rsa.pub"
            if len(sys.argv) > 2:
                public_key = sys.argv[2]
            exec_cmd(conf_name, "k", public_key)
