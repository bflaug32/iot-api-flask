# Server
where your data is saved, calculated, controlled, and sometimes viewed

## Running locally via Docker (Windows/OS X/Linux)
* [Install and start docker](https://docs.docker.com/engine/installation/)
* `cd` or `dir` to the `fabbit-iot/server` directory
* build the docker image (and label it fabbit-iot) `docker build -t fabbit-iot .`
* run the docker app you just built `docker run -p 5000:5000 --name app fabbit-iot`
* Visit the server at http://localhost:5000/

* To close the app type `docker stop app`
* if you want to rebuild with new code also run `docker rm app`

## Additional Notes
* if `localhost` does not work, get the IP of your docker machine via the following command `docker-machine ip`
* if you'd like to login to the terminal of the running docker instance use `docker exec -it app bash`
* to stop the running app (and remove the image) use `docker stop app` and `docker rm $(docker ps -aq)'`
* if the above commands don't work on linux try `sudo`!!
* see [run_local_linux.sh](./run_local_linux.sh) and [run_local_osx.sh](run_local_osx) for more examples

## Deploying to AWS
* `cd` to `fabbit-iot/server`
* zip all the files in the source directory (so that when they're extracted the Dockerfile and such will NOT be inside a diretory)... for unix-like systems something like `zip -r deployment.zip *` from inside `fabbit-iot/server`
* start an [elastic beanstalk instance, ready to receive a docker container](http://docs.aws.amazon.com/elasticbeanstalk/latest/dg/create_deploy_docker.html) (start with Sample Application if you like)
* upload your zip file
* enjoy!

## TODO
* Someone with windows please build a `run_local_windows.sh` script
* If you run on azure or linode or something, write a deployment guide
