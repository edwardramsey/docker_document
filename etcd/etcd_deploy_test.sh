#!/bin/bash

CONF_FILE='deploy.conf'
HOST_IP='10.1.245.157'
USER='root'
PASSWORD='Abc@1234'

PUBLIC_KEY='/Users/edward/.ssh/id_rsa.pub'

function func1()
{
	# case1 单机集群
	cat "$HOST_IP $USER $PASSWORD" > deploy.conf
	python ./etcd_deplpy.py -u
	ssh $USER@$HOST_IP kill -9 `ps -ef|grep etcd_cluster|grep -v grep|awk '{print $2}'`
}


function func2()
{
	# case2 多机集群
	cat <<EOF > deploy.conf
$HOST_IP $USER $PASSWORD
10.1.245.158 root Abc@1234
10.1.245.159 root Abc@1234
EOF

	python ./etcd_deplpy.py -u
}


function func3()
{
# case3 单机public key 登录
cat "$HOST_IP $USER" > deploy.conf
python ./etcd_deplpy.py -k $PUBLIC_KEY
}

function func4()
{
# case4 单机生成文件
python ./etcd_deplpy.py -f $HOST_IP
scp $HOST_IP
ssh $USER@$HOST_IP
}
 

# case5 多机生成文件
function func5()
{
	echo ""	
}
