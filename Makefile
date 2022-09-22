LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto

all: http_downloader

http_downloader: http_downloader.c
	gcc http_downloader.c ssl_socket.c http.c $(LDFLAGS) $(LDLIBS) -pthread -g -o http_downloader

clean:
	rm -f wc http_downloader
	make 