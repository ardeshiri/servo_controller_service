FROM ubuntu:20.04
COPY ./servo_ctrl_service /
RUN apt-get update
RUN apt-get install -y libssl-dev
RUN apt-get install -y librabbitmq-dev
ENTRYPOINT ["/servo_ctrl_service"]

