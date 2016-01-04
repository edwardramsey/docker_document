#!/bin/bash
g++ server.cpp -o server && g++ client.cpp -o client

mkdir -p dServer
mv server ./dServer
echo -e "FROM ubuntu\nRUN apt-get install curl -y\nCOPY server /bin/\n" > dServer/Dockerfile

mkdir -p dClient
mv client ./dClient
echo -e "FROM ubuntu\nRUN apt-get install curl -y\nCOPY client /bin/\n" > dClient/Dockerfile

docker build -t "server:v1" ./dServer
docker build -t "client:v1" ./dClient

docker stop `docker ps -aq`
docker rm `docker ps -a|grep test|awk '{print $1}'`

docker run -d --net=host --name "test_server" server:v1 /bin/server -P:4455
docker run -d --net=host --name "test_client" client:v1 /bin/client

