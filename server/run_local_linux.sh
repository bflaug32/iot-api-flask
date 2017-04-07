# install docker first! https://docs.docker.com/engine/installation/
sudo service docker start

#build, name, and run docker image
sudo docker build -t fabbit-iot .
sudo docker run -p 5000:5000 --name iotapp fabbit-iot

# to login to the docker iotapp's terminal run
# sudo docker exec -it iotapp bash

# to stop the docker iotapp run
# sudo docker stop iotapp && sudo docker rm $(sudo docker ps -aq)'
