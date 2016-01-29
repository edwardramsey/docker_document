#!/bin/bash

# for add password
sudo apt-get -y install apache2-utils

if [ ! command -v docker-compose ];then
	curl -L https://github.com/docker/compose/releases/download/1.5.2/docker-compose-`uname -s`-`uname -m` > ./docker-compose
	sudo mv ./docker-compose /usr/local/bin/docker-compose
	chmod +x /usr/local/bin/docker-compose
fi


mkdir ~/docker-registry && cd $_
mkdir -p data
mkdir -p nginx

cat <<EOF > private_hub.yml
nginx:
  image: "nginx:1.9"
  ports:
    - 5043:443
  links:
    - registry:registry
  volumes:
    - ./nginx/:/etc/nginx/conf.d
registry:
  image: registry:2
  ports:
    - 127.0.0.1:5000:5000
  environment:
    REGISTRY_STORAGE_FILESYSTEM_ROOTDIRECTORY: /data
  volumes:
    - ./data:/data
EOF
