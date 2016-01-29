#!/bin/bash

g++ shared1.cpp -o s1 && g++ shared2.cpp -o s2

mkdir -p dS1
mv s1 ./dS1
echo -e "FROM ubuntu\nRUN apt-get install curl -y\nCOPY s1 /bin/\n" > dS1/Dockerfile

mkdir -p dS2
mv s2 ./dS2
echo -e "FROM ubuntu\nRUN apt-get install curl -y\nCOPY s2 /bin/\n" > dS2/Dockerfile

docker build -t "shm1:v1" ./dS1
docker build -t "shm2:v1" ./dS2

docker stop `docker ps -aq`
docker rm `docker ps -a|grep shm|awk '{print $1}'`

docker run -d --net=host --ipc=host --name "shm1" shm1:v1 /bin/s1 
sleep 2
docker run -d --ipc=host --name "shm2" shm2:v1 /bin/s2
