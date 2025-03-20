#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define N 1000  // Large matrix size for benchmarking

// Function to allocate a dynamic 2D array
int **allocate_matrix() {
    int **matrix = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        matrix[i] = (int *)malloc(N * sizeof(int));
    }
    return matrix;
}

// Function to free a dynamically allocated 2D array
void free_matrix(int **matrix) {
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to initialize matrices with random values
void initialize_matrix(int **matrix) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrix[i][j] = rand() % 100;  // Random values between 0 and 99
}

// Function to multiply matrices using different loop orders
void multiply_matrices(int **A, int **B, int **C, char *order) {
    struct timeval start, end;
    gettimeofday(&start, NULL);  // Start timing

    // Initialize result matrix
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = 0;

    // Perform multiplication based on loop order
    if (strcmp(order, "i,j,k") == 0) {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                for (int k = 0; k < N; k++)
                    C[i][j] += A[i][k] * B[k][j];
    }
    else if (strcmp(order, "i,k,j") == 0) {
        for (int i = 0; i < N; i++)
            for (int k = 0; k < N; k++)
                for (int j = 0; j < N; j++)
                    C[i][j] += A[i][k] * B[k][j];
    }
    else if (strcmp(order, "j,i,k") == 0) {
        for (int j = 0; j < N; j++)
            for (int i = 0; i < N; i++)
                for (int k = 0; k < N; k++)
                    C[i][j] += A[i][k] * B[k][j];
    }
    else if (strcmp(order, "j,k,i") == 0) {
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                for (int i = 0; i < N; i++)
                    C[i][j] += A[i][k] * B[k][j];
    }
    else if (strcmp(order, "k,i,j") == 0) {
        for (int k = 0; k < N; k++)
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    C[i][j] += A[i][k] * B[k][j];
    }
    else if (strcmp(order, "k,j,i") == 0) {
        for (int k = 0; k < N; k++)
            for (int j = 0; j < N; j++)
                for (int i = 0; i < N; i++)
                    C[i][j] += A[i][k] * B[k][j];
    }
    else {
        printf("Invalid loop order! Choose from: i,j,k / i,k,j / j,i,k / j,k,i / k,i,j / k,j,i\n");
        exit(1);
    }

    gettimeofday(&end, NULL);  // End timing
    double elapsed_time = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1e6);
    printf("Execution time for order %s: %f seconds\n", order, elapsed_time);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <loop_order>\n", argv[0]);
        return 1;
    }

    srand(42);  // Fixed seed for reproducibility

    // Allocate memory dynamically to avoid segmentation faults
    int **A = allocate_matrix();
    int **B = allocate_matrix();
    int **C = allocate_matrix();

    // Initialize matrices with random values
    initialize_matrix(A);
    initialize_matrix(B);

    multiply_matrices(A, B, C, argv[1]);

    // Free dynamically allocated memory
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);

    return 0;
}
