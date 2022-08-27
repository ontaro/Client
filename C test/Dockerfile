FROM ubuntu:latest
RUN apt update -y 
RUN apt install build-essential -y
RUN apt-get install libcurl4-openssl-dev -y
COPY . /ImageServer
WORKDIR /ImageServer/
RUN gcc Client.c ./source/base64.c ./source/cJSON.c -lm -o Client -lcurl
CMD ["./Client"]