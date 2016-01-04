#!/bin/bash

C1="container1"
C2="container2"

g++ container1.cpp -o $C1 && g++ container2.cpp -o $C2

if [ $? -eq 0 ]; then
	mkdir -p d1 && mkdir -p d2
	mv $C1 d1 && mv $C2 d2

	echo -e "FROM ubuntu\nMAINTAINER edward Chen\nRUN apt-get install curl -y\nCOPY $C1 /bin/\n" > d1/Dockerfile
	echo -e "FROM ubuntu\nMAINTAINER edward Chen\nRUN apt-get install curl -y\nCOPY $C2 /bin/\n" > d2/Dockerfile

	docker build -t "container1:v1" ./d1
	docker build -t "container2:v1" ./d2

	docker stop `docker ps -aq`
	docker rm `docker ps -a|grep container|awk '{print $1}'`

	docker run -d --name "$C1" container1:v1 /bin/$C1
	docker run -ti --net=host --name "$C1" container2:v1 /bin/$C2

else
	echo "compile error"
fi


