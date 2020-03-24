FROM ubuntu:18.04
USER root

RUN apt-get update
RUN apt-get -y install libboost-all-dev && apt -y install g++ && apt-get -y install cmake

WORKDIR /source
COPY . /source

COPY ./httpd.conf /etc/httpd.conf
COPY ./http-test-suite/httptest /var/www/html/httptest

RUN cmake .
RUN make

EXPOSE 80

CMD ./HighLoadWebServer
