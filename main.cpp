/*
 * Prologue
 * ---------------------------------------------------------------
 * Name:           main.cpp
 * Lab:            EECS 348 Lab 5 
 * Author:         Hunter Noravong
 * KUID:           3162520
 * Creation date:  10/01/2026
 *
 * Description:
 *   Reads an integer N and two N x N matrices (A and B) from a text
 *   file, then performs the following operations:
 *     1. Print both matrices
 *     2. Add the matrices (A + B)
 *     3. Multiply the matrices (A * B)
 *     4. Compute the main and secondary diagonal sums
 *     5. Swap two rows
 *     6. Swap two columns
 *     7. Update a single element
 *   Every operation that takes an index checks that it is in the
 *   range 0 <= index < N before modifying anything.
 *
 * Inputs:
 *   - The name of the input file, typed by the user at the prompt.
 *   - The file contains N on the first line, then the N rows of A,
 *     then the N rows of B. Whitespace only separates numbers.
 *
 * Outputs:
 *   - The matrices and the results of each operation, printed to
 *     standard output with right-aligned columns.
 *   - Error messages for a bad file, bad N, or missing numbers are
 *     printed to standard error (cerr).
 *
 * Collaborators:  None
 * Other sources:  Claude
 */

#include <iostream>   // cout, cin, cerr, endl
#include <fstream>    // ifstream for reading the input file
#include <iomanip>    // setw for aligned output
#include <vector>     // vector for the matrices
#include <string>     // string, to_string
using namespace std;

// A matrix is a vector of rows, and each row is a vector of ints.
// So M[i] is row i, M[i][j] is the entry at row i, column j,
// and M.size() is N.
using Matrix = vector<vector<int>>;

// ======================= Problem 1: load and print =======================

/*
 * loadMatrices
 * Reads N, then matrix A, then matrix B from the named file.
 * Parameters: filename - file to read
 *             A, B     - output matrices, resized to N x N (passed by reference)
 * Returns:    true on success; false (after printing an error to cerr) if the
 *             file can't be opened, N is missing or not positive, or the file
 *             has fewer numbers than N*N for either matrix.
 */
bool loadMatrices(const string& filename, Matrix& A, Matrix& B) {
    ifstream file(filename);
    if (!file) {  // the file name was wrong or the file is unreadable
        cerr << "Error: could not open file \"" << filename << "\"" << endl;
        return false;
    }

    int N;
    // A failed read leaves the stream in a "false" state, so we can test it.
    if (!(file >> N)) {
        cerr << "Error: could not read N from the file." << endl;
        return false;
    }
    if (N <= 0) {
        cerr << "Error: N must be positive (got " << N << ")." << endl;
        return false;
    }

    // We only know N now, so this is the first point we can size the matrices.
    // assign(N, row) makes N rows, each a copy of "row" (a vector of N zeros).
    A.assign(N, vector<int>(N));
    B.assign(N, vector<int>(N));

    // Fill A row by row, checking every read in case the file runs out.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (!(file >> A[i][j])) {
                cerr << "Error: not enough numbers for matrix A." << endl;
                return false;
            }
        }
    }
    // Fill B the same way; its numbers follow A's in the file.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (!(file >> B[i][j])) {
                cerr << "Error: not enough numbers for matrix B." << endl;
                return false;
            }
        }
    }
    return true;
}

/*
 * printMatrix
 * Prints a label, then the matrix with every entry right-aligned in a
 * field of width 6 (room for 4-digit numbers, a minus sign, and a gap).
 * The matrix is const& so it is not copied and cannot be changed.
 */
void printMatrix(const Matrix& M, const string& label) {
    cout << label << endl;
    for (const auto& row : M) {
        for (int x : row)
            cout << setw(6) << x;  // setw affects only the next value, so it is repeated
        cout << endl;              // end of one row
    }
    cout << endl;                  // blank line between printed matrices
}

// ============================ Problem 2: add =============================

/*
 * addMatrices
 * Returns C = A + B, where C[i][j] = A[i][j] + B[i][j].
 * Both inputs are N x N, so each result entry needs one entry from each
 * input at the same position. Cost grows like N^2.
 */
Matrix addMatrices(const Matrix& A, const Matrix& B) {
    int N = A.size();
    Matrix C(N, vector<int>(N));  // result, N x N
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

// ========================== Problem 3: multiply ==========================

/*
 * multiplyMatrices
 * Returns C = A * B (true matrix product, not entry-by-entry).
 * C[i][j] is the dot product of row i of A and column j of B:
 *     C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + ... + A[i][N-1]*B[N-1][j]
 * Cost grows like N^3 (N^2 entries, N multiplications each).
 */
Matrix multiplyMatrices(const Matrix& A, const Matrix& B) {
    int N = A.size();
    Matrix C(N, vector<int>(N));
    for (int i = 0; i < N; i++) {          // i picks the result row
        for (int j = 0; j < N; j++) {      // j picks the result column
            int sum = 0;                   // must restart at 0 for every cell
            for (int k = 0; k < N; k++)    // k walks along row i of A and down column j of B
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    }
    return C;
}

// ======================= Problem 4: diagonal sums ========================

/*
 * diagonalSums
 * Computes both diagonal sums of a square matrix.
 *   Main diagonal:      entries M[i][i]        (top-left to bottom-right)
 *   Secondary diagonal: entries M[i][N-1-i]    (top-right to bottom-left)
 * Results are returned through the reference parameters mainSum and secSum.
 * For odd N the center entry is on both diagonals and is counted in both
 * sums, which is what the definitions require.
 */
void diagonalSums(const Matrix& M, int& mainSum, int& secSum) {
    int N = M.size();
    mainSum = 0;
    secSum = 0;
    for (int i = 0; i < N; i++) {   // both diagonals have exactly N entries
        mainSum += M[i][i];
        secSum += M[i][N - 1 - i];
    }
}

// ========================= Problem 5: swap rows ==========================

/*
 * swapRows
 * Swaps row r1 and row r2 of M in place (M is passed by reference).
 * If either index is outside 0..N-1, prints a message and leaves M unchanged.
 * Swapping a row with itself is valid and changes nothing.
 */
void swapRows(Matrix& M, int r1, int r2) {
    int N = M.size();
    // Validate BOTH indices before touching the matrix.
    if (r1 < 0 || r1 >= N || r2 < 0 || r2 >= N) {
        cout << "Invalid row index (" << r1 << ", " << r2
             << "). Valid range is 0 to " << N - 1 << ". Matrix unchanged." << endl;
        return;
    }
    swap(M[r1], M[r2]);  // each row is one vector<int>, so one call swaps the whole row
}

// ======================== Problem 6: swap columns ========================

/*
 * swapCols
 * Swaps column c1 and column c2 of M in place.
 * There is no single "column" object in a vector of vectors, so we visit
 * every row and swap the two entries at the chosen columns.
 * If either index is outside 0..N-1, prints a message and leaves M unchanged.
 */
void swapCols(Matrix& M, int c1, int c2) {
    int N = M.size();
    if (c1 < 0 || c1 >= N || c2 < 0 || c2 >= N) {
        cout << "Invalid column index (" << c1 << ", " << c2
             << "). Valid range is 0 to " << N - 1 << ". Matrix unchanged." << endl;
        return;
    }
    for (int i = 0; i < N; i++)
        swap(M[i][c1], M[i][c2]);
}

// ====================== Problem 7: update one element ====================

/*
 * updateElement
 * Sets M[r][c] to value. Row index comes first, column index second.
 * If r or c is outside 0..N-1, prints a message and leaves M unchanged.
 * (An out-of-range write into a vector is undefined behavior, so the
 * check must happen before the assignment.)
 */
void updateElement(Matrix& M, int r, int c, int value) {
    int N = M.size();
    if (r < 0 || r >= N || c < 0 || c >= N) {
        cout << "Invalid position (" << r << ", " << c
             << "). Valid range is 0 to " << N - 1 << ". Matrix unchanged." << endl;
        return;
    }
    M[r][c] = value;
}

// ================================= main ==================================

int main() {
    // Ask for the input file name.
    string filename;
    cout << "Enter input file name: ";
    cin >> filename;

    // Problem 1: load. If loading fails, an error has already been printed,
    // so exit with a nonzero status instead of continuing with bad data.
    Matrix A, B;
    if (!loadMatrices(filename, A, B))
        return 1;

    int N = A.size();

    printMatrix(A, "Matrix A:");
    printMatrix(B, "Matrix B:");

    // Problem 2: addition
    printMatrix(addMatrices(A, B), "A + B:");

    // Problem 3: multiplication
    printMatrix(multiplyMatrices(A, B), "A * B:");

    // Problem 4: diagonal sums for both matrices
    int mainSum, secSum;
    diagonalSums(A, mainSum, secSum);
    cout << "Matrix A: main diagonal sum = " << mainSum
         << ", secondary diagonal sum = " << secSum << endl;
    diagonalSums(B, mainSum, secSum);
    cout << "Matrix B: main diagonal sum = " << mainSum
         << ", secondary diagonal sum = " << secSum << endl << endl;

    // Problems 5-7 modify a matrix, so each test works on a fresh copy
    // of the original A. That way one test's changes never carry into the next.
    // The indices below are hardcoded test cases: a valid pair, the same
    // index twice, and out-of-range indices (-1 and N).
    Matrix temp = A;

    // Problem 5: swap rows
    swapRows(temp, 0, N - 1);
    printMatrix(temp, "A after swapping rows 0 and " + to_string(N - 1) + ":");

    temp = A;
    swapRows(temp, 0, 0);   // same index: valid, no change
    printMatrix(temp, "A after swapping row 0 with itself:");

    temp = A;
    swapRows(temp, -1, N);  // invalid: message printed, no change
    printMatrix(temp, "A after invalid row swap (-1, N):");

    // Problem 6: swap columns
    temp = A;
    swapCols(temp, 0, N - 1);
    printMatrix(temp, "A after swapping columns 0 and " + to_string(N - 1) + ":");

    temp = A;
    swapCols(temp, 1 % N, 1 % N);  // "1 % N" keeps the index valid even when N = 1
    printMatrix(temp, "A after swapping a column with itself:");

    temp = A;
    swapCols(temp, -1, N);
    printMatrix(temp, "A after invalid column swap (-1, N):");

    // Problem 7: update one element
    temp = A;
    updateElement(temp, 0, N - 1, 99);
    printMatrix(temp, "A after updating (0, " + to_string(N - 1) + ") to 99:");

    temp = A;
    updateElement(temp, N, 0, 99);  // invalid row index
    printMatrix(temp, "A after invalid update (N, 0):");

    return 0;
}