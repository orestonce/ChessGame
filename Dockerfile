FROM ubuntu:18.04 as builder
RUN apt-get update && apt-get install golang -y
ADD src /data/src
WORKDIR /data
ARG GOPATH=/data/
RUN go run src/ProtocolGen/main.go 
RUN go build ChessServer/ChessGate
RUN go build ChessServer/ChessGame

FROM ubuntu:18.04 as cg_gateway
RUN apt-get update && apt-get install tzdata -y
COPY --from=builder /data/ChessGate /usr/local/bin/ChessGate
EXPOSE 8912
CMD  ["/usr/local/bin/ChessGate", "-laddr", ":8912", "-raddr", "redis:6379"]

FROM ubuntu:18.04 as cg_game
RUN apt-get update && apt-get install tzdata -y
COPY --from=builder /data/ChessGame /usr/local/bin/ChessGame
CMD  ["/usr/local/bin/ChessGame", "-raddr", "redis:6379", "-spath", "/data/ChessGame.dat"]
