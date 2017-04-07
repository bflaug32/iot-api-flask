# install docker first! https://docs.docker.com/engine/installation/

#build, name, and run docker image
docker build -t fabbit-iot .
docker run -p 5000:5000 --name iotapp fabbit-iot

# to login to the docker iotapp's terminal run
# docker exec -it iotapp bash

# to stop the docker iotapp run
# docker stop iotapp && docker rm $(docker ps -aq)'
