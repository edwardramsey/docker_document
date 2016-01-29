##etcd interface

####部署 deployment

- 单机部署 standby mode

```
etcd -data-dir your_path
```

- 集群部署 cluster mode

`-data-dir` 数据路径，包括日志(wal)路径和快照(snaoshot)路径

`-wal-dir` 指定日志路径

`-initial-advertise-peer-urls` 将节点的url告知集群，用于etcd数据的通信

`-initial-cluster` 启动配置，所有节点信息

`-initial-cluster-state` 默认为new，若设为existing，则加入存在的集群

`-listen-peer-urls` 接受集群中节点的url

`-listen-client-urls` 接受客户端请求和http请求的url

示范命令

```
etcd -name node -initial-advertise-peer-urls http://10.0.1.11:2380 \
  -listen-peer-urls http://10.0.1.11:2380 \
  -listen-client-urls http://10.0.1.11:2379,http://127.0.0.1:2379 \
  -advertise-client-urls http://10.0.1.11:2379 \
  -initial-cluster-token etcd-cluster-1 \
  -initial-cluster infra0=http://10.0.1.10:2380,infra1=http://10.0.1.11:2380,infra2=http://10.0.1.12:2380 \
  -initial-cluster-state new
```


####接口

- 添加/修改

```
// 添加节点
curl http://ip:port/v2/keys/your_key -XPUT -d value="your_value" 
// 设置超时节点，即超过时间该节点会消失，单位秒
curl http://ip:port/v2/keys/your_key -XPUT -d value="your_value" -d ttl=your_seconds
// 修改与添加的接口相同，但是会改变etcd的index值，防止修改已存在的接口
curl http://ip:port/v2/keys/your_key?prevExist=false -XPUT -d value="your_value"
```

- 删除

```
curl http://ip:port/v2/keys/key -XDELETE
```

- 查询

```
// 查询单个节点或目录
curl http://ip:port/v2/keys/your_key_or_dir
// 查询全部节点，循环遍历
curl http://ip:port/v2/keys/?recursive=true
```

- 统计信息接口

```
// 查看是否为leader
curl http://ip:port/v2/stats/leader
// 查看该IP节点的信息
curl http://ip:port/v2/stats/self
// 查看统计信息
curl http://ip:port/v2/stats/store
```


[参考文档1 etcd API](https://coreos.com/etcd/docs/latest/api.html#running-a-single-machine-cluster)

[参考文档2 Members API](https://coreos.com/etcd/docs/latest/other_apis.html)

[参考文档3 Configuration Flags](https://coreos.com/etcd/docs/latest/configuration.html)
