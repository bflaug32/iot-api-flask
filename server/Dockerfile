FROM python:3.6

RUN apt-get update && apt-get install -y vim memcached

COPY requirements.txt /src/
RUN pip install -r /src/requirements.txt

ADD . /src
WORKDIR /src

EXPOSE 5000

CMD ["sh","run_docker.sh"]
