# Operating Systems (2024/2025)

## Grade
20/20

## Group Participants
* Marco Rocha Ferreira - A106857 - MarcoFerreira05
* Marco Rafael Vieira Sèvegrand - A106807 - marcosevegrand
* Nuno Henrique Macedo Rebelo - A107372 - NunoRebelo05

## Description

This is a document indexing service that provides various keyword-based search functionalities.
It is built in C and uses Bash for the test scripts.

### Project Goals

The main goals of this project are:
* learn how to effectively use system calls to take advantage of OS provided mechanisms (open, close, read, write, exec, fork, pipe, mkfifo and dup)
* implement an efficient cache with an eviction policy that suits the workload
* develop consistent benchmarking in order to correctly evaluate the impact of the applied optimizations

### How It Works

The server program is responsible for indexing the documents and answering the search requests, whereas
the client program makes requests to the server when invoked. 
This communication is accomplished using named pipes, through which clients send one of the following requests:
1. dclient -a "title" "authors" "year" "path" - Tells the server to index a new document. A unique key must be returned to the client.
2. dclient -c "key" - Asks the server for the metadata of an indexed document.
3. dclient -d "key" - Tells the server to remove the indexed metadata referring to that key.
4. dclient -l "key" "keyword" - The server must return to the client the number of lines of a given document containing the keyword.
5. dclient -s "keyword" - Asks the server for all the keys of documents containing the keyword.
6. dclient -f - Shut down the server.
To implement the -l and -s options, the grep utility was used. In short, grep is executed in a child process of the server, which returns the result through an anonymous pipe. Futhermore, -c, -l and -s shouldn't block future clients, so they're executed in parallel.

From the base implementation, it was required to apply some optimizations to the server program:
1. Concurrent search: dclient -s "keyword" "nr_processes". It must be possible to pass the optional argument "nr_processes" which tells the server to parallelize the -s search with that number of processes.
2. Persistence: the server must keep a persistent copy of the indexed metadata which should be recovered when the server stops and starts again.
3. Caching: it must be possible to control the number of metadata entries kept in memory. This means the disk is now considered the main storage and memory acts as a cache.

Finally, the most apropriate cache eviction policy for this workload is in our opinion a plain LRU, which we implemented with a little tweak: for the -s option, the server doesn't check the cache first. It goes directly to disk to get every entry. This prevents the cache from being "poluted" with entries that were accessed in the scan but probably won't be accessed in the near future. In other words, we keep the truly "hot" entries in cache by going directly to disk.