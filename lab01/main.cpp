#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

typedef vector<vector<double> > vvd;
typedef vector<double> vd;

const double EPS = 1e-9;
const double EPS_ITER = 1e-5;

vd xs;
vector<bool> isFree;

vvd readFile(const char *fileName) {
    freopen(fileName, "r", stdin);
    vvd m;

    string fileLine;
    while (getline(cin, fileLine)) {
        stringstream ss(fileLine);

        vd line;
        double num;
        while (ss >> num)
            line.push_back(num);

        m.push_back(line);
    }

    xs.resize(m.size());
    return m;
}

void printMatrix(const vvd &matrix) {
    for (const auto &m1: matrix) {
        for (const double m2: m1)
            cout << m2 << ' ';
        cout << "\n";
    }
    cout << '\n';
}

bool findMajor(const vvd &matrix,
               int &majorRow,
               int &majorCol,
               double &majorVal) {
    majorVal = 0;
    bool found = false;

    for (int i = 0; i < matrix.size(); i++)
        for (int j = 0; j < matrix.size(); j++)
            if (abs(matrix[i][j]) > EPS &&
                abs(majorVal) < abs(matrix[i][j])) {
                majorVal = matrix[i][j];
                majorRow = i;
                majorCol = j;
                found = true;
            }

    return found;
}

void sumRows(vvd &matrix,
             const int &majorRow,
             const int &majorCol,
             const double &majorVal) {
    for (int i = 0; i < matrix.size(); i++) {
        if (i == majorRow)
            continue;

        auto m = -(matrix[i][majorCol] / majorVal);
        for (int j = 0; j < matrix[i].size(); j++)
            matrix[i][j] += matrix[majorRow][j] * m;
    }
}

void deleteMajor(vvd &matrix,
                 const int &majorRow,
                 const int &majorCol) {
    vvd newMatrix;

    for (int i = 0; i < matrix.size(); i++) {
        vd line;
        for (int j = 0; j < matrix.size() + 1; j++)
            if (j == majorCol || i == majorRow)
                line.push_back(0);
            else
                line.push_back(matrix[i][j]);

        newMatrix.push_back(line);
    }

    matrix = newMatrix;
}

vd task1(vvd &matrix) {
    static int step = 1;
    const int n = matrix.size();

    int majorRow, majorCol;
    double majorVal;
    if (!findMajor(matrix, majorRow, majorCol, majorVal)) {
        for (const auto &row: matrix)
            if (abs(row[n]) > EPS)
                return vd();
        return vd(n, 0);
    }
    cout << "Step: " << step++ << '\n';
    isFree[majorCol] = false;

    sumRows(matrix, majorRow, majorCol, majorVal);
    printMatrix(matrix);

    vvd nextMatrix(matrix);

    deleteMajor(nextMatrix, majorRow, majorCol);
    printMatrix(nextMatrix);

    auto xs = task1(nextMatrix);
    if (xs.empty())
        return xs;

    double free = matrix[majorRow][n];
    for (int j = 0; j < n; j++)
        if (j != majorCol)
            free -= matrix[majorRow][j] * xs[j];
    xs[majorCol] = free / majorVal;

    return xs;
}

double normOct(const vvd &B) {
    double res = 0;
    for (int j = 0; j < B.size(); j++) {
        double colSum = 0;
        for (int i = 0; i < B.size(); i++)
            colSum += abs(B[i][j]);
        res = max(res, colSum);
    }
    return res;
}

double normCub(const vvd &B) {
    double res = 0;
    for (int i = 0; i < B.size(); i++) {
        double rowSum = 0;
        for (int j = 0; j < B.size(); j++)
            rowSum += abs(B[i][j]);
        res = max(res, rowSum);
    }
    return res;
}

bool isDominant(const vvd &m) {
    int n = m.size();
    for (int i = 0; i < n; i++) {
        double off = 0;
        for (int j = 0; j < n; j++)
            if (i != j) off += abs(m[i][j]);
        if (abs(m[i][i]) <= off) return false;
    }
    return true;
}

vvd makeDominant(const vvd &m) {
    if (isDominant(m)) return m;
    int n = m.size();
    vvd res = m;

    for (int i = 0; i < n; i++) {
        bool found = false;
        for (int w0 = -1; w0 <= 1 && !found; w0++) {
            for (int w1 = -1; w1 <= 1 && !found; w1++) {
                for (int w2 = -1; w2 <= 1 && !found; w2++) {
                    if (w0 == 0 && w1 == 0 && w2 == 0) continue;
                    vd row(n + 1, 0);
                    int w[3] = {w0, w1, w2};
                    for (int r = 0; r < n; r++)
                        for (int c = 0; c <= n; c++)
                            row[c] += w[r] * m[r][c];

                    double off = 0;
                    for (int j = 0; j < n; j++)
                        if (i != j) off += abs(row[j]);

                    if (abs(row[i]) > off) {
                        res[i] = row;
                        found = true;
                    }
                }
            }
        }
    }
    return res;
}

vd task2(const vvd &matrix) {
    int n = matrix.size();
    cout << "Task 2: Simple Iteration Method\n";

    vvd m = makeDominant(matrix);
    cout << "Transformed matrix (diagonal dominance):\n";
    printMatrix(m);

    vvd B(n, vd(n, 0));
    vd c(n, 0);
    for (int i = 0; i < n; i++) {
        c[i] = m[i][n] / m[i][i];
        for (int j = 0; j < n; j++)
            if (i != j)
                B[i][j] = -m[i][j] / m[i][i];
    }

    cout << "Matrix B:\n";
    printMatrix(B);
    cout << "Vector c:\n";
    for (double val : c) cout << val << ' ';
    cout << "\n\n";

    double nOct = normOct(B);
    double nCub = normCub(B);
    cout << "Octahedral norm ||B||_1   = " << nOct << "\n";
    cout << "Cubic norm      ||B||_inf = " << nCub << "\n";

    if (nOct < 1.0 || nCub < 1.0) {
        cout << "Convergence condition satisfied (norm < 1).\n\n";
    } else {
        cout << "Convergence condition NOT satisfied.\n\n";
    }

    vd x_prev = c;
    vd x_curr(n);

    int step = 0;
    while (true) {
        step++;
        double maxDiff = 0;

        for (int i = 0; i < n; i++) {
            x_curr[i] = c[i];
            for (int j = 0; j < n; j++)
                x_curr[i] += B[i][j] * x_prev[j];

            maxDiff = max(maxDiff, abs(x_curr[i] - x_prev[i]));
        }

        cout << "Iter " << step << ": ";
        for (double val : x_curr) cout << val << ' ';
        cout << "| diff = " << maxDiff << "\n";

        if (maxDiff < EPS_ITER)
            break;

        x_prev = x_curr;
    }

    cout << "\nResult task 2:\n";
    for (double val : x_curr) cout << val << ' ';
    cout << "\n";

    return x_curr;
}

int main() {
    auto matrix = readFile("11.in");
    freopen("11.out", "w", stdout);

    cout << "Original matrix\n";
    printMatrix(matrix);

    cout << "Task 1: Gauss Method\n";
    auto matrixForTask1 = matrix;
    isFree.assign(matrixForTask1.size(), true);

    auto xs1 = task1(matrixForTask1);
    if (xs1.empty()) {
        cout << "No solutions\n";
        return 0;
    }

    bool infinite = false;
    for (bool f : isFree)
        infinite = infinite || f;
    if (infinite) {
        cout << "Infinite solutions\n";
    } else {
        cout << "\nResult task 1:\n";
        for (double val : xs1) cout << val << ' ';
        cout << "\n\n";
    }

    task2(matrix);

    return 0;
}
