# VMsimulator
Virtual Memory Simulator C++

To Run Our Code, it will take 5 parameters as following:

• P1: Size of pages, i.e., # of memory locations on each page. <br/>
• P2: FIFO, LRU, or Clock for type of page replacement algorithms:<br/>
  o FIFO: First-in, First-Out<br/>
  o LRU: Least Recently Used<br/>
  o Clock: Clock Page Replacement<br/>
• P3: flag to turn on/off pre-paging. If pre-paging is not turned on, we use demand paging by
default.<br/>
  o +: turn it on<br/>
  o -: turn it off<br/>
A typical command line should look like this:<br/>
  ```./VMsimulator plist.txt ptrace.txt 2 FIFO +```
  

Analysis of Page Replacement Algorithms:
What we Expected to See vs. What we Actually Saw
In regards to expectations vs actual, we had expected FIFO
to be the least efficient algorithm, as it was not only the
easiest to implement, but the algorithm simply took away the
oldest page, which may be a page that is important to the
process. We expected LRU to be better, and clock even more
significantly better than FIFO. However, looking at the numbers,
we can see that clock was barely an improvement over LRU. We
also expected to see prepaging being much more efficient than
demand paging, and we actually saw that. The numbers show that
there are significantly fewer page swaps when using prepaging.
Complexity vs Performance Benefit (Demand Paging)
1. FIFO (First in First Out)
In terms of complexity, FIFO is simple algorithm that is
easy to implement. When memory is full and a page fault
occurs, it will take out the page that was put in first
relative to the other pages in memory to make room for the
next page not in memory. Though it was the easiest to
implement, in terms of performance benefit, FIFO proved to
be the least efficient. As seen in the data we obtained
from running FIFO, it had the most page swaps.
2. LRU (Least Recently Used)
The complexity of LRU was similar to FIFO, as it was also
fairly simple to implement. However, contrary to the
performance benefit of FIFO, LRU was more efficient than
FIFO. The performance benefits are more obvious as the page
size increases, as seen in the data shown above. At page
size 16, LRU saves roughly over 30,000 more pageswaps than
FIFO.
3. Clock
Out of all three page replacement algorithms, clock was the
more difficult one to implement. Though it was the most
difficult to implement, the performance benefit was not
much better than LRU at all. Looking at the numbers, we can
see that the performance is barely improved in clock as we
increase in the page size. As we get to page size 16
however, the number of page swaps increases again.
Complexity vs Performance Benefit (Prepaging)
1. FIFO (First In First Out)
Implementing FIFO with prepaging was not any harder than
demand paging. We had to do one extra step of finding the
next adjacent page that was not in memory in addition to
the requested page and put both of them in memory. In
comparison to prepaging in LRU and clock, we can see from
the data shown above that FIFO is still the least efficient
as it still has the most page swaps out of all 3
algorithms.
2. LRU (Least Recently Used)
Similarly to demand paging, implementing prepaging for LRU
was not any more difficult than implementing prepaging for
FIFO. By looking at the number of page swaps when using
prepaging on LRU, we can see that for all page sizes
tested, LRU performs much better than FIFO.
3. Clock
Clock for prepaging was once again the hardest to
implement. With larger page sizes, it is not much better
than LRU.
Demand Paging vs Prepaging
In most cases, prepaging yields better performance than in
demand paging. However, this improvement in performance can only
be observed when using a page size of 8 or smaller. For example,
if we look at the number of page swaps in FIFO or LRU in
prepaging and compare that to the demand paging page swaps, we
can see that with a page size of 8 or less, there is definitely
an improvement in performance. However, when we look at the
number of page swaps for a page size of 16, we observed that the
number of page swaps in prepaging actually increased. So, we
only observe performance benefits in prepaging when we are using
a page size of 8 or less.
Discuss how the data might have changed if a completely random
memory access trace
Looking at the ptrace.txt file, we can observe that there
are clusters of lines that include the same process and
relatively sequential ordering of memory location. This ordering
of memory locations uses a property called the principle of
locality. Had ptrace been a completely random memory access
trace, the principle of locality would not be applicable, and we
may have seen more page swaps with a completely random ordering
of a memory access trace.
