LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto

all: http_downloader

http_downloader: http_downloader.c
	gcc http_downloader.c ssl_socket.c $(LDFLAGS) $(LDLIBS) -g -o http_downloader

clean:
	rm -f wc http_downloader
	make 