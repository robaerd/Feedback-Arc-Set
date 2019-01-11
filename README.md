# Feedback-Arc-Set
A program which can be run parallel that uses an monte-carlo-algorithm to find a minimal feedback-arc-set. 
#### Info
- A server/client structure is used
- Generators(clients) write their found solution to a circular buffer which is then read by the supervisor(server).
- The shared memory has a size of around 4kB
- If the supervisor receives a signal (SIGINT or SIGTERM) all running generators will terminate also
- The supervisor write each time it has found a smaller(optimal) solution, the feedback-arc-set to stdout and searches for a more optimal solution.
- In case 0 edges are found, the graph is acyclic and the supervisor and all running generators terminate
- If error in generator occure (i.e. wrong input) the supervisor keeps running and reads from the circular buffer or waits for any generator to write into the buffer

#### Building from source
For development:
```sh
$ make all
```
Remove all executables and object files with:
```sh
$ make clean
```
#### Usage 
##### Supervisor must be started before any generator!
Start supervisor:
```sh
$ ./supervisor
```
Start generator(s):
- SYNOPSIS:
```sh
$ ./generators EDGE1 EDGE2 EDGE3 ...
```
- EXAMPLE:
Each positional argument is one edge; at least one edge must be given. An edge is specified by a string, with the indices of the two nodes it connects separated by a -. Since the graph is directed, the edge starts at the first node and leads to the second node. Note that the number of nodes of the graph is implicitly provided through the indices in the edges.
```sh
$ ./generators 1-4 1-5 2-6 3-4 3-6 4-5 6-0 6-5
```

Start multiple generators (i.e. in for loop from shell)
```sh
$ for i in {1..10}; do (./generator 0-1 1-2 2-0 &); done
```

The generators can also be started asynchronously. 
