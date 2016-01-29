#!/bin/bash

###########################################################
#
#  Deploy mesos zookeeper marathon for CentOS 7.1
#
#############################################################

sudo rpm -Uvh http://repos.mesosphere.com/el/7/noarch/RPMS/mesosphere-el-repo-7-1.noarch.rpm
sudo yum -y install mesos marathon

sudo rpm -Uvh http://archive.cloudera.com/cdh4/one-click-install/redhat/6/x86_64/cloudera-cdh-4-0.x86_64.rpm
sudo yum -y install zookeeper zookeeper-server

sudo -u zookeeper zookeeper-server-initialize --myid=1
sudo service zookeeper-server start

sudo service mesos-master start
sudo service mesos-slave start
sudo service marathon start

# view marathon UI by visiting IP:8080



