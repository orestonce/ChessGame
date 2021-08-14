FROM ubuntu:18.04 as builder
RUN apt-get update && apt-get install golang -y
ADD . /data/
WORKDIR /data
RUN go run ChessServer/BuildServer/main.go

FROM ubuntu:18.04 as cg_gateway
RUN apt-get update && apt-get install tzdata -y
COPY --from=builder /data/bin/ChessServer /usr/local/bin/ChessServer
EXPOSE 8912
CMD  ["/usr/local/bin/ChessServer", "Gate", "-laddr", ":8912", "-raddr", "redis:6379"]

FROM ubuntu:18.04 as cg_game
RUN apt-get update && apt-get install tzdata -y
COPY --from=builder /data/bin/ChessServer /usr/local/bin/ChessServer
CMD  ["/usr/local/bin/ChessServer", "Game", "-raddr", "redis:6379", "-spath", "/data/ChessGame.dat"]
