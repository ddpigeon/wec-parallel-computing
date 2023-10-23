#include <iomanip>
#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include <random>
const int mod = (int) 1E9 + 7;

#pragma omp declare reduction(mod_sum : int :  omp_out = (omp_out + omp_in) % 1000000007)

int parallel_sum(std::vector<int> &arr) {
    int total = 0;
    #pragma omp parallel for reduction(mod_sum: total)
    for (int i = 0; i < arr.size(); i++) {
        total += arr[i];
        total %= mod;
    }
    return total % mod;
}

int seq_sum(std::vector<int> &arr) {
    int total = 0;
    for (int i = 0; i < arr.size(); i++) {
        total += arr[i];
        total %= mod;
    }
    return total;
}

int main(int argc, char** argv){

    std::string inp2 = argv[1];
    long long x = stoll(inp2);
    omp_set_num_threads(12);

    if (x <= 0) {
        std::cout << "Invalid input\n";
        exit(-1);
    }

    std::ranlux24_base rng;
    std::vector<int> a;
    rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    for (int i = 0; i < x; i++) {
        a.push_back(rng());
        //std::cout << a[i] << " ";
        //                  ^ uncomment to print array
    }
    //std::cout << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    int total = parallel_sum(a);
    //std::cout << total << std::endl;
    //                          ^ uncomment to print the actual answer
    auto end = std::chrono::high_resolution_clock::now();
    double time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << time_taken << std::endl; 
    //std::cout << "ms for parallel\n";
    //                  ^ uncomment to know which is parallel/sequential etc.

    start = std::chrono::high_resolution_clock::now();
    total = seq_sum(a);
    //std::cout << total << std::endl;
    end = std::chrono::high_resolution_clock::now();
    time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << time_taken << std::endl; 
    //std::cout << "ms for sequential\n";
    return 0;
}
