#include <iomanip>
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <chrono>
#include <omp.h>


class Matrix {
    public:
        int size;
        std::vector<std::vector<double>> m;

        Matrix(const Matrix& ma) {
            size = ma.size;
            const int s = ma.size;
            m.resize(ma.size, std::vector<double>(ma.size));
            #pragma omp parallel for 
            for (int i = 0; i < s; i++) {
                #pragma omp parallel for 
                for (int j = 0; j < s; j++) {
                    m[i][j] = ma.m[i][j]; //Easily parallelised since we're just copying it around, each element is independent of all others
                }
            }
        };


        Matrix(const Matrix& ma, const int np) { 
            //Non-parallelized matrix copying
            size = ma.size;
            m.resize(ma.size, std::vector<double>(ma.size));
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    m[i][j] = ma.m[i][j];
                }
            }
        };

        Matrix(double n) {
            size = n;
            m.resize(n, std::vector<double>(n));
        }

        inline double& operator()(int x, int y) {return m[x][y];}

        void randomise() {
            std::default_random_engine gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            std::uniform_real_distribution<double> rng(0, 1000);
            //Set upper bound of random real numbers to 1000 since printing matrices looks pretty ugly with larger numbers
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    m[i][j] = rng(gen);
                }
            }
        }
        
        void display() {
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    std::cout << std::setprecision(4) << m[i][j] << "\t";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        };
        
        std::pair<Matrix, Matrix> lu() {
            std::pair<Matrix, Matrix> lu_pair = std::make_pair(Matrix(size), Matrix(*this, 0));
            for (int i = 0; i < size; i++) {
                double pivot = lu_pair.second.m[i][i];
                lu_pair.first.m[i][i] = 1;
                for (int j = i+1; j < size; j++) {
                    double mul = lu_pair.second.m[j][i] / pivot;
                    lu_pair.first.m[j][i] = mul;
                    lu_pair.second.m[j][i] = 0;
                    for (int k = i+1; k < size; k++) {
                        lu_pair.second.m[j][k] -= (mul * lu_pair.second.m[i][k]);
                    }
                }
            }
            return lu_pair;
        }

        std::pair<Matrix, Matrix> lu_parallel() {
            std::pair<Matrix, Matrix> lu_pair = std::make_pair(Matrix(size), Matrix(*this));
            for (int i = 0; i < size; i++) {
                double pivot = lu_pair.second.m[i][i];
                lu_pair.first.m[i][i] = 1;
                #pragma omp parallel for //since each row is independent of the other
                for (int j = i+1; j < size; j++) {
                    double mul = lu_pair.second.m[j][i] / pivot;
                    lu_pair.first.m[j][i] = mul;
                    lu_pair.second.m[j][i] = 0;
                    for (int k = i+1; k < size; k++) {
                        lu_pair.second.m[j][k] -= (mul * lu_pair.second.m[i][k]);
                    }
                }
            }
            return lu_pair;
        }

};


int main(int argc, char** argv) {
    std::string inp2 = argv[1];
    int x = stoi(inp2);
    omp_set_num_threads(12);

    if (x <= 0) {
        std::cout << "Invalid input\n";
        exit(-1);
    }

    Matrix a(x); //set size of matrix to whatever

    //a.m[0][0] = 1;
    //a.m[0][1] = 2;
    //a.m[0][2] = 3;
    //a.m[1][0] = 2;
    //a.m[1][1] = 3;
    //a.m[1][2] = 1;
    //a.m[2][0] = -2;
    //a.m[2][1] = 3;
    //a.m[2][2] = -2;
    //^ test 3x3 matrix
    //

    a.randomise();
    //a.display();
    
    auto start_seq = std::chrono::high_resolution_clock::now();
    std::pair<Matrix, Matrix> lu = a.lu();
    //lu.first.display();
    //lu.second.display();
    //          ^ Uncomment to display the LU factorization
    //          Would not recommend for larger matrices
    //
    auto end_seq = std::chrono::high_resolution_clock::now();
    double time_taken_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count();

    auto start_par = std::chrono::high_resolution_clock::now();
    std::pair<Matrix, Matrix> luf = a.lu_parallel();
    //luf.first.display();
    //luf.second.display();
    //          ^ Uncomment to display the LU factorization
    //          Would not recommend for larger matrices
    //
    auto end_par = std::chrono::high_resolution_clock::now();
    double time_taken_par = std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count();


    std::cout << time_taken_par << std::endl;
    //std::cout << "ms for parallel\n";
    std::cout << time_taken_seq << std::endl;
    //std::cout << "ms for sequential\n";


    return 0;
}
