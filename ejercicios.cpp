#include <iostream>
#include <vector>
#include <assert.h>
#include <chrono> // Para medir el tiempo
#include <cmath>
#include <iomanip>
using namespace std;
using namespace std::chrono;
#define MAX 1000
// Bucles FOR cap 2
double A[MAX][MAX], x[MAX], y[MAX];

// Función que implementa el primer par de bucles (acceso por filas)
void first_pair_of_loops(int size) {
    for (int i = 0; i < size; i++) {
        y[i] = 0;
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

// Función que implementa el segundo par de bucles (acceso por columnas)
void second_pair_of_loops(int size) {
    for (int i = 0; i < size; i++) {
        y[i] = 0;
    }
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < size; i++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

// Classic matrices multiplication
template <class T>
std::vector<std::vector<T>> matrices_multiplication(std::vector<std::vector<T>> matrix_1, std::vector<std::vector<T>> matrix_2)
{
    int m = matrix_1.size(), n = matrix_2[0].size();

    assert(matrix_1[0].size() == matrix_2.size());

    std::vector<std::vector<T>> result_matrix(m, std::vector<T>(n, 0));
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < matrix_2.size(); k++)
            {
                result_matrix[i][j] += matrix_1[i][k] * matrix_2[k][j];
            }
        }
    }
    return result_matrix;
}

int calculate_optimal_block_size(int cache_size_bytes, int element_size_bytes) {
    // Queremos que dos bloques (uno de la matriz A y otro de la B) quepan en la caché.
    // Cada bloque tiene un tamaño de block_size^2, y tenemos dos bloques.
    // 2 * block_size^2 * element_size_bytes <= cache_size_bytes
    // block_size <= sqrt(cache_size_bytes / (2 * element_size_bytes))

    int block_size = static_cast<int>(std::sqrt(cache_size_bytes / (2.0 * element_size_bytes)));
    return block_size;
}

template <class T>
std::vector<std::vector<T>> matrices_multiplication_block(std::vector<std::vector<T>> matrix_1, std::vector<std::vector<T>> matrix_2, int block_size)
{
    int m = matrix_1.size();
    int n = matrix_2[0].size();
    int inner = matrix_1[0].size();

    // Verificar que las dimensiones sean compatibles para la multiplicación
    assert(matrix_1[0].size() == matrix_2.size());

    // Crear la matriz de resultado inicializada con ceros
    std::vector<std::vector<T>> result_matrix(m, std::vector<T>(n, 0));

    // Multiplicación por bloques
    for (int i = 0; i < m; i += block_size) {
        for (int j = 0; j < n; j += block_size) {
            for (int k = 0; k < inner; k += block_size) {
                // Multiplicar cada bloque
                for (int i_block = i; i_block < std::min(i + block_size, m); ++i_block) {
                    for (int j_block = j; j_block < std::min(j + block_size, n); ++j_block) {
                        T sum_val = 0;
                        for (int k_block = k; k_block < std::min(k + block_size, inner); ++k_block) {
                            result_matrix[i_block][j_block] += matrix_1[i_block][k_block] * matrix_2[k_block][j_block];
                        }
                    }
                }
            }
        }
    }

    return result_matrix;
}

int main()
{
     // Inicializar la matriz A y el vector x con algunos valores
    for (int i = 0; i < MAX; i++) {
        x[i] = i; // Inicializar x
        for (int j = 0; j < MAX; j++) {
            A[i][j] = i + j; // Inicializar A
        }
    }

    // Imprimir el encabezado de la tabla
    cout << setw(10) << "Size" 
         << setw(20) << "First_loop_time (ms)"
         << setw(20) << "Second_loop_time (ms)" << endl;

    // Probar para diferentes tamaños de matriz (ejemplo: de 100 en 100 hasta MAX)
    for (int size = 100; size <= MAX; size += 100) {
        // Medir el tiempo del primer par de bucles (acceso por filas)
        auto start = high_resolution_clock::now();
        first_pair_of_loops(size);
        auto end = high_resolution_clock::now();
        auto first_loop_time = duration_cast<milliseconds>(end - start).count();

        // Medir el tiempo del segundo par de bucles (acceso por columnas)
        start = high_resolution_clock::now();
        second_pair_of_loops(size);
        end = high_resolution_clock::now();
        auto second_loop_time = duration_cast<milliseconds>(end - start).count();

        // Imprimir el tamaño de la matriz y los tiempos de ejecución
        cout << setw(10) << size 
             << setw(20) << first_loop_time 
             << setw(20) << second_loop_time << endl;
    }

    // int cache_size = 32 * 1024; // 32 KB = 32768 bytes
    // int element_size = sizeof(double); // Tamaño de 'double' es de 8 bytes

    // // Calcular el block_size óptimo
    // int block_size = calculate_optimal_block_size(cache_size, element_size);

    // // Imprimir encabezado
    // // cout << "Classic Matrix Multiplication" << endl;
    // cout << "Block Matrix Multiplication" << endl;
    // cout << "square_matrix_dimension\t" << "time(ms)" << endl;

    // // Ejecutar pruebas para matrices cuadradas de tamaños de 100 a 1000
    // for (int m = 100; m <= 1000; m += 100)
    // {
    //     // Crear matrices cuadradas de tamaño m x m
    //     std::vector<std::vector<int>> matrix_1(m, std::vector<int>(m, 0));
    //     std::vector<std::vector<int>> matrix_2(m, std::vector<int>(m, 0));

    //     // Inicializar las matrices con algunos valores para que no sean todas ceros
    //     for (int i = 0; i < m; i++)
    //     {
    //         for (int j = 0; j < m; j++)
    //         {
    //             matrix_1[i][j] = rand() % 10; // valores aleatorios entre 0 y 9
    //             matrix_2[i][j] = rand() % 10; // valores aleatorios entre 0 y 9
    //         }
    //     }

    //     // Medir el tiempo de ejecución de la multiplicación clásica
    //     auto start_time = high_resolution_clock::now(); // Tiempo de inicio

    //     // std::vector<std::vector<int>> result_matrix = matrices_multiplication(matrix_1, matrix_2);
    //     std::vector<std::vector<int>> result_matrix = matrices_multiplication_block(matrix_1, matrix_2, block_size);


    //     auto end_time = high_resolution_clock::now(); // Tiempo de fin
    //     auto duration = duration_cast<milliseconds>(end_time - start_time); // Duración en milisegundos

    //     // Imprimir el tamaño de la matriz y el tiempo de cálculo
    //     cout << m << "\t\t\t" << duration.count() << endl;
    // }

    return 0;
}