#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <float.h>

using namespace std;

typedef vector<vector<double> > vvd;
typedef vector<double> vd;

vd xs;

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

void findMajor(const vvd &matrix,
               int &majorRow,
               int &majorCol,
               double &majorVal) {
    majorVal = DBL_MIN;

    for (int i = 0; i < matrix.size(); i++)
        for (int j = 0; j < matrix.size(); j++)
            if (matrix[i][j] != 0 &&
                abs(majorVal) < abs(matrix[i][j])) {
                majorVal = matrix[i][j];
                majorRow = i;
                majorCol = j;
            }
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
    if (step == n + 1)
        return vd(n, 0);
    cout << "Step: " << step++ << '\n';

    int majorRow, majorCol;
    double majorVal;
    findMajor(matrix, majorRow, majorCol, majorVal);

    sumRows(matrix, majorRow, majorCol, majorVal);
    printMatrix(matrix);

    vvd nextMatrix(matrix);

    deleteMajor(nextMatrix, majorRow, majorCol);
    printMatrix(nextMatrix);

    auto xs = task1(nextMatrix);

    double rest = matrix[majorRow][n];
    for (int j = 0; j < n; j++)
        if (j != majorCol)
            rest -= matrix[majorRow][j] * xs[j];
    xs[majorCol] = rest / majorVal;

    return xs;
}


int main() {
    auto matrix = readFile("11.in");
    freopen("11.out", "w", stdout);
    printMatrix(matrix);

    auto xs = task1(matrix);
    for (double xs1 : xs) cout << xs1 << ' ';
}
