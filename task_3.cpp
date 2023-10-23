#include <cstdlib>
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

        Matrix(Matrix& ma) {
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
            m.resize(n, std::vector<double>(n, 0));
        }

        inline double& operator()(int x, int y) {return m[x][y];}

        void randomise() {
            std::default_random_engine gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
            std::uniform_real_distribution<double> rng(0, 1000);
            //Set upper bound of random real numbers to 1000 since printing matrices looks pretty ugly with higher numbers
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    m[i][j] = rng(gen);
                }
            }
        }
        
        void display() {
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    std::cout << std::fixed << std::setprecision(3) << m[i][j] << "\t";
                    //feel free to change the precision for more precise values, I feel this makes the matrix look better when printed
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        };

        void swaprows(int i, int j) {
            #pragma omp parallel for
            for (int k = 0; k < size; k++) {
                double temp = m[i][k];
                m[i][k] = m[j][k];
                m[j][k] = temp;
            }
        }

        Matrix inverse() {
            Matrix inv(size);
            Matrix copy(*this, 0);
            for (int i = 0; i < size; i++) inv.m[i][i] = 1;
            for (int i = 0; i < size; i++) {
                //i is column
                double pivot = copy.m[i][i];
                if (pivot == 0) {
                    for (int j = i+1; j < size; j++) {
                        if (copy.m[i][j] != 0) {
                            copy.swaprows(i, j);
                            inv.swaprows(i, j);
                            pivot = copy.m[i][i];
                            break;
                        }
                    }
                }

                if (pivot == 0) {
                    Matrix null_matrix(size);
                    return null_matrix;
                }

                for (int j = 0; j < size; j++) {
                    if (i != j) {
                        double mul = copy.m[j][i] / pivot;
                        for (int k = 0; k < size; k++) {
                            inv.m[j][k] -= inv.m[i][k] * mul;
                            copy.m[j][k] -= copy.m[i][k] * mul;
                        }
                    }
                }
            }


            for (int i = 0; i < size; i++) {
                double piv = copy.m[i][i];
                copy.m[i][i] = 1;
                for (int j = 0; j < size; j++) {
                    inv.m[i][j] /= piv;
                }
            }

            return inv;
        }


        Matrix inverse_parallel() {
            Matrix inv(size);
            Matrix copy(*this);

            #pragma omp parallel for
            for (int i = 0; i < size; i++) inv.m[i][i] = 1;

            for (int i = 0; i < size; i++) {
                //i is column
                double pivot = copy.m[i][i];
                if (pivot == 0) {
                    for (int j = i+1; j < size; j++) {
                        if (copy.m[i][j] != 0) {
                            copy.swaprows(i, j);
                            inv.swaprows(i, j);
                            pivot = copy.m[i][i];
                            break;
                        }
                    }
                }

                if (pivot == 0) {
                    Matrix null_matrix(size);
                    return null_matrix;
                }

                #pragma omp parallel for
                for (int j = 0; j < size; j++) {
                    if (i != j) {
                        double mul = copy.m[j][i] / pivot;
                        for (int k = 0; k < size; k++) {
                            inv.m[j][k] -= inv.m[i][k] * mul;
                            copy.m[j][k] -= copy.m[i][k] * mul;
                        }
                    }
                }
            }

            for (int i = 0; i < size; i++) {
                double piv = copy.m[i][i];
                copy.m[i][i] = 1;
                #pragma omp parallel for
                for (int j = 0; j < size; j++) {
                    inv.m[i][j] /= piv;
                }
            }

            return inv;
        }

};


int main(int argc, char **argv) {
    std::string inp2 = argv[1];
    int x = stoi(inp2);
    omp_set_num_threads(12);

    Matrix a(x); //set size of matrix to whatever

    if (a.size <= 0) {
        std::cout << "Invalid input!" << std::endl;
        exit(-1);
    }

    //a.m[0][0] = 0;
    //a.m[0][1] = 1;
    //a.m[0][2] = 0;
    //a.m[1][0] = 1;
    //a.m[1][1] = 0;
    //a.m[1][2] = 0;
    //a.m[2][0] = 0;
    //a.m[2][1] = 0;
    //a.m[2][2] = 1;
    //^ test 3x3 matrix
    //

    a.randomise();
    //a.display();
    //^ uncomment to display input matrix
    
    auto start_seq = std::chrono::high_resolution_clock::now();
    Matrix inv = a.inverse();
    //inv.display();
    //     ^ Uncomment to display the inverse
    //     Would not recommend for larger matrices
    //
    auto end_seq = std::chrono::high_resolution_clock::now();
    double time_taken_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count();

    auto start_par = std::chrono::high_resolution_clock::now();
    Matrix inv_par = a.inverse_parallel();
    //inv_par.display();
    //      ^ Uncomment to display the inverse
    //      Would not recommend for larger matrices
    //
    auto end_par = std::chrono::high_resolution_clock::now();
    double time_taken_par = std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count();


    std::cout << time_taken_par << std::endl;
    //std::cout << "ms for parallel\n";
    std::cout << time_taken_seq << std::endl;
    //std:cout << "ms for sequential\n";


    return 0;
}
