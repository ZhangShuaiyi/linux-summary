all: vsock_server vsock_client

vsock_server: vsock_server.c
	gcc -o vsock_server vsock_server.c

vsock_client: vsock_client.c
	gcc -o vsock_client vsock_client.c

clean:
	rm vsock_server vsock_client