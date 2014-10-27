- [ ] integrate libdivsufsort as reference (for single node run time and
      testing)
- [ ] unified C++ interface between libdivsufsort and psac
- [ ] (see previous) psac interface for global mem (scatter + psac + gather)
- [ ] implement testing against libdivsufsort
- [ ] document library functions
- [ ] clean iterator interface
- [ ] distributed correctness test
- [ ] more fine grained performance profiling
- [ ] multiseq merge after samplesort all2all (should be faster than another
      round of sorting) [in-place]
- [ ] find other bottlenecks and optimize
- [ ] LCP
- [ ] BWT
- [ ] get wavefront and other paper's code and compare
- [ ] implementation for few unresolved buckets
- [ ] clean up test code