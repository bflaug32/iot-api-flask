FROM python:3.4-onbuild

ADD . /src
WORKDIR /src

RUN pip install -r requirements.txt

EXPOSE 5000

CMD ["python", "application.py"]