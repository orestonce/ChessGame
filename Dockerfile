FROM golang:1.16 as builder
ADD . /data/
WORKDIR /data
ARG GOPROXY=https://goproxy.io
RUN go mod tidy
RUN go run ChessServer/BuildServer/main.go

FROM ubuntu:18.04 as cg_gateway
RUN apt-get update && apt-get install tzdata -y
COPY --from=builder /data/bin/ChessServer /usr/local/bin/ChessServer
EXPOSE 8912
CMD  ["/usr/local/bin/ChessServer", "Gate", "--laddr", ":8912", "--raddr", "redis:6379"]

FROM ubuntu:18.04 as cg_game
RUN apt-get update && apt-get install tzdata -y
COPY --from=builder /data/bin/ChessServer /usr/local/bin/ChessServer
CMD  ["/usr/local/bin/ChessServer", "Game", "--raddr", "redis:6379", "--mysql_schema", "root:@tcp(mysql_server:3306)/chess?parseTime=true&charset=utf8"]

FROM mysql:8.0 as mysql_server
COPY init.sql /docker-entrypoint-initdb.d/init.sql
ENV MYSQL_ALLOW_EMPTY_PASSWORD=1
