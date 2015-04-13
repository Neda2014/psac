/**
 * @file    timer.hpp
 * @author  Patrick Flick <patrick.flick@gmail.com>
 * @brief   MPI synchronized section timer.
 *
 * Copyright (c) TODO
 *
 * TODO add Licence
 */

#ifndef MXX_TIMER_HPP
#define MXX_TIMER_HPP

// MPI include
#include <mpi.h>

// C++ includes
#include <iostream>
#include <chrono>
#include <sstream>

// mxx includes
#include "reduction.hpp"

namespace mxx
{

template <typename duration>
class timer_impl {
protected:
    // use the monotonic `steady_clock` for time measurements of code
    std::chrono::steady_clock::time_point start;
public:
    // constructor
    timer_impl() {
        start = std::chrono::steady_clock::now();
    }

    // returns time elapsed since creation
    typename duration::rep elapsed() const {
      std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
      typename duration::rep elapsed_time = duration(stop-start).count();
      return elapsed_time;
    }
};

using timer = timer_impl<std::chrono::duration<double, std::milli> >;

template <typename duration>
class section_timer_impl {
protected:
    /// The output stream
    std::ostream& ostr;
    /// The MPI communicator for barrier sync and reductions
    MPI_Comm comm;
    /// the root process (only the process with this rank prints the timing
    /// results)
    int root;
    /// The seperator used for the machine readable output (default: '\t')
    std::string sep;
    /// The current depth of timer instantiations.
    static int depth;
    /// The type of the time duration
    typedef typename duration::rep time_rep;

    // use the monotonic `steady_clock` for time measurements of code
    /// Type of a time point
    std::chrono::steady_clock::time_point start;

public:
    /// Construtor with reasonable defaults
    section_timer_impl(std::ostream& outstream = std::cerr, MPI_Comm comm = MPI_COMM_WORLD, int root = 0)
      : ostr(outstream), comm(comm), root(root), sep("\t") {
        // sync on barrier
        MPI_Barrier(comm);
        start = std::chrono::steady_clock::now();
        section_timer_impl::depth++;
    }

    /// End the section with the given name. This prints out the elapsed time
    /// information aggregated from all processors
    void end_section(const std::string& name) {
        std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
        time_rep elapsed = duration(stop-start).count();
        // TODO: reduce rather than allreduce
        // TODO: use single reduction with custom operator
        time_rep min = mxx::allreduce(elapsed, [](time_rep x, time_rep y){return std::min(x,y);}, comm);
        time_rep max = mxx::allreduce(elapsed, [](time_rep x, time_rep y){return std::max(x,y);}, comm);
        time_rep sum = mxx::allreduce(elapsed, std::plus<time_rep>(), comm);
        // calc mean:
        int p, rank;
        MPI_Comm_rank(comm, &rank);
        MPI_Comm_size(comm, &p);
        time_rep mean = sum / (double)p;
        // only root process outputs the timings
        if (rank == root)
            ostr << "TIMER" << sep << min << sep << mean << sep << max << sep << depth << sep << name << std::endl;
        // restart timer
        MPI_Barrier(comm);
        start = std::chrono::steady_clock::now();
    }

    virtual ~section_timer_impl() {
      section_timer_impl::depth--;
    }
};

template <typename duration>
int section_timer_impl<duration>::depth = 0;

using section_timer = section_timer_impl<std::chrono::duration<double, std::milli> >;

} // namespace mxx



#endif // MXX_TIMER_HPP
