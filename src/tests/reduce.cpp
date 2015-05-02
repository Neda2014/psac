
#include <iostream>

#include <mxx/reduction.hpp>

int main(int argc, char *argv[])
{
    // set up MPI
    MPI_Init(&argc, &argv);

    // get communicator size and my rank
    MPI_Comm comm = MPI_COMM_WORLD;
    int p, rank;
    MPI_Comm_size(comm, &p);
    MPI_Comm_rank(comm, &rank);

    /* code */
    int x = 2*rank+1;
    //int y = mxx::allreduce(x, [](int x, int y){return x+y;}, comm);
    //int y = mxx::allreduce(x, std::max<int>, comm);
    int y = mxx::allreduce(x, std::plus<int>(), comm);
    std::cout << "sum = " << y << std::endl;

    MPI_Datatype ct;
    MPI_Type_contiguous(3, MPI_INT, &ct);
    //std::string str = "hello there!";
    mxx::attr_map<int, std::string>::set(ct, 13, "hi blah blah!");
    mxx::attr_map<int, std::string>::set(ct, 13, "xxxx");
    mxx::attr_map<int, int>::set(ct, 13, 14);
    // TODO: test deletion, thread-safety, etc
    // TODO: what happens if i set some index to different types??
    // TODO: should i normalize to use a fixed type for indexing?

    mxx::attr_map<int, std::function<void()> >::set(ct, 1, [](){std::cout << "Called std::function!" << std::endl;});

    MPI_Datatype dt;
    MPI_Type_dup(ct, &dt);
    mxx::attr_map<int, std::string>::set(dt, 13, "blubb");
    mxx::attr_map<int, int>::set(ct, 13, 15);

    // and get
    std::cout << mxx::attr_map<int, std::string>::get(ct, 13) << ", " << mxx::attr_map<int,int>::get(ct,13) << std::endl;
    std::cout << mxx::attr_map<int, std::string>::get(dt, 13) << ", " << mxx::attr_map<int,int>::get(dt,13) << std::endl;

    // try executing function:
    mxx::attr_map<int, std::function<void()> >::get(ct, 1)();

    MPI_Type_free(&ct);
    //MPI_Type_free(&dt);
    // finalize MPI
    MPI_Finalize();
    return 0;
}
