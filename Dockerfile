FROM python:3.4-onbuild

ADD . /src
WORKDIR /src
RUN mv _assets/nltk_data /usr/lib/
RUN pip install -r requirements.txt

EXPOSE 5000

CMD ["python", "application.py"]