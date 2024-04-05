all: pThreads.c
	gcc -o pThreads-100k -pthread pThreads-100k.c
	gcc -o pThreads-500k -pthread pThreads-500k.c
	gcc -o pThreads-1mil -pthread pThreads-1mil.c
clean:
	$(RM) pthread-100k
	$(RM) pthread-500k
	$(RM) pthread-1mil