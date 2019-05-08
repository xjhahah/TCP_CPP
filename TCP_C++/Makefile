.PHONY:all
all:client server
client:client.cc
	g++ -o $@ $^ -g
server:server.cc
	g++ -o $@ $^ -lpthread -g
.PHONY:clean
clean:
	rm -f client server
