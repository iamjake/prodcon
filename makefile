CC = gcc
CFLAGS = -w -o

all: producer.c consumer.c
	$(CC) $(CFLAGS) producer producer.c
	$(CC) $(CFLAGS) consumer consumer.c
producer: producer.c
	$(CC) $(CFLAGS) producer producer.c
consumer: consumer.c
	$(CC) $(CFLAGS) consumer consumer.c
runproducer: producer
	./producer sample.txt
runconsumer: consumer
	./consumer sample.txt
.PHONY: clean
clean:
	rm -i  producer consumer
