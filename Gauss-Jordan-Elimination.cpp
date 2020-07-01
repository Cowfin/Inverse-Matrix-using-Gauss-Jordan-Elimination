/*
Kelvin Wong
Gauss-Jordan-Elimination
*/

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class MyMatrix {
private:
	vector<double> elements; // array containing data
	vector<double*> rows; // array of pointers to start of each row
	unsigned ncols; // number of columns
	unsigned nrows; // number of rows

public:

	MyMatrix(unsigned nrows, unsigned ncols); // constructor takes matrix dimensions as arguments

	double get(unsigned irow, unsigned jcol); // method returns the element in the ith row and jth column

	void set(unsigned irow, unsigned jcol, double value); // method sets the element in the ith row and jth column

	friend istream& operator>> (istream& in, MyMatrix&); // loads dataset from file

	void printMatrix(unsigned nrow, unsigned ncol); // method that prints the matrix

	void setup(unsigned irow, unsigned icol); // method that creates the inverse matrix with the same number of rows and columns as the main matrix

	double det(); // method to calculate and return the determinant of the matrix

	double reduce(unsigned row, unsigned row2); // method to alter the matrix to reduced row echelon form and returns the values used to achieve the reduction

	void inverseReduce(unsigned row, unsigned row2, double operate); // method using the redution values and repeating the process to the inverse matrix

	double rowToOne(unsigned row); // method that multiplies/divides the leading coefficient and its row to '1' and returns the value used to achieve this

	void inverseToOne(unsigned row, double operate); // method that repeats the process of setting the main matrix's coefficients to '1' using the returned values

	double eliminate(unsigned row, unsigned row2); // method that eliminates the rest of the values above the identity line

	void inverseEliminate(unsigned row, unsigned col, double operate); // method that repeats the elimination of the values above the identity values, using the returned values

	void add(unsigned row, unsigned col, double total); // method that adds values to certain points on the matrix
};


int main() {
	// set number of rows and columns
	unsigned nrows = 13;
	unsigned ncols = 13;

	MyMatrix matrix(nrows, ncols); // creates main matrix
	MyMatrix determinant(nrows, ncols); // creates determinant matrix
	MyMatrix inverse(nrows, ncols); // creates inverse matrix

	MyMatrix product(nrows, ncols); // creates product matrix
	MyMatrix mainMatrix(nrows, ncols); // creates mainMatrix

	ifstream file("matrix.tsv");
	ifstream file2("matrix.tsv");
	ifstream file3("matrix.tsv");

	if (!file) {
		cerr << "could not open matrix.tsv" << endl;
		return -1;
	}

	file >> mainMatrix; // fills main matrix with values from tsv file
	file2 >> determinant; // fills determinant matrix with values from tsv file
	file3 >> matrix; // fills matrix with values from tsv file

	double deter = determinant.det(); // determinant method is run and returns the value of the determinant
	if (deter == 0) { // if the determinant of the matrix is '0' the program stops and displays a message
		cerr << "determinant = 0, therefore there is no inverse" << endl;
		return -1;
	}

	inverse.setup(nrows, ncols); // fills inverse values

	cout << "INPUT" << endl; // prints the input matrix
	matrix.printMatrix(nrows, ncols);


	double operate;

	for (unsigned row = 0; row < nrows; row++) {
		operate = matrix.rowToOne(row);
		inverse.inverseToOne(row, operate);
		for (unsigned row2 = row + 1; row2 < nrows; row2++) {
			operate = matrix.reduce(row, row2);
			inverse.inverseReduce(row, row2, operate);
		}
	}

	for (int x = (ncols - 1); x >= 0; x--) {
		for (int y = (x - 1); y >= 0; y--) {
			operate = matrix.eliminate(x, y);
			inverse.inverseEliminate(x, y, operate);
		}
	}

	double value1, value2;
	double total = 0;

	for (unsigned i = 0; i < nrows; i++) {
		for (unsigned j = 0; j < ncols; j++) {
			for (unsigned k = 0; k < nrows; k++) {
				value1 = inverse.get(k, j);
				value2 = mainMatrix.get(i, k);
				total += value1 * value2;
			}
			product.add(i, j, total);
			total = 0;
		}
	}

	cout << "\nOUTPUT" << endl; // outputs the main matrix after calculations
	matrix.printMatrix(nrows, ncols);

	cout << "\nINVERSE" << endl; // outputs the inverse of the main matrix
	inverse.printMatrix(nrows, ncols);

	cout << "\nPRODUCT" << endl; // outputs the product of the original matrix and its inverse
	product.printMatrix(nrows, ncols);

}

MyMatrix::MyMatrix(unsigned _nrows, unsigned _ncols) {
	ncols = _ncols;
	nrows = _nrows;

	elements.resize(nrows * ncols);
	rows.resize(nrows);

	// the & returns "the address of" the element in the array
	for (unsigned irow = 0; irow < nrows; irow++)
		rows[irow] = &(elements[irow * ncols]);
}

double MyMatrix::get(unsigned irow, unsigned jcol) {
	return rows[irow][jcol];
}

void MyMatrix::set(unsigned irow, unsigned jcol, double value) {
	rows[irow][jcol] = value;
}

istream& operator>> (istream& in, MyMatrix& matrix) {
	unsigned ncols = matrix.ncols;
	unsigned nrows = matrix.rows.size();

	for (unsigned irow = 0; irow < nrows; irow++)
	{
		for (unsigned icol = 0; icol < ncols; icol++)
		{
			double value;
			in >> value;

			matrix.set(irow, icol, value);
		}
	}

	return in;
}

void MyMatrix::printMatrix(unsigned irow, unsigned icol) {
	for (unsigned row = 0; row < irow; row++) {
		for (unsigned col = 0; col < icol; col++) {
			cout << rows[row][col] << "\t";
		}
		cout << endl;
	}
}

void MyMatrix::setup(unsigned nrow, unsigned ncol) {
	for (unsigned i = 0; i < nrow; i++) {
		for (unsigned j = 0; j < ncol; j++) {
			if (i = j) {
				rows[i][j] = 1;
			}
			else {
				rows[i][j] = 0;
			}
		}
	}
	rows[0][0] = 1;
}

double MyMatrix::det() {
	double operate;
	double determinant = 1;

	for (unsigned row = 0; row < nrows; row++) {
		for (unsigned row2 = row + 1; row2 < nrows; row2++) {
			operate = -rows[row2][row];
			for (unsigned col = 0; col < ncols; col++) {
				rows[row2][col] = rows[row2][col] + (operate / rows[row][row]) * rows[row][col];
			}
		}
	}

	for (unsigned i = 0; i < nrows; i++) {
		determinant = determinant * rows[i][i];
	}

	return determinant;
}

double MyMatrix::reduce(unsigned row, unsigned row2) {
	double operate = -rows[row2][row];
	for (unsigned col = 0; col < ncols; col++) {
		rows[row2][col] = rows[row2][col] + (operate)* rows[row][col];
	}
	return operate;
}

void MyMatrix::inverseReduce(unsigned row, unsigned row2, double operate) {
	for (unsigned col = 0; col < ncols; col++) {
		rows[row2][col] = rows[row2][col] + (operate)* rows[row][col];
	}
}

double MyMatrix::rowToOne(unsigned row) {
	double operate = rows[row][row];
	for (unsigned col = 0; col < ncols; col++) {
		rows[row][col] = rows[row][col] / operate;
	}
	return operate;
}

void MyMatrix::inverseToOne(unsigned row, double operate) {
	for (unsigned col = 0; col < ncols; col++) {
		rows[row][col] = rows[row][col] / operate;
	}
}

double MyMatrix::eliminate(unsigned row, unsigned row2) {
	double operate = -rows[row2][row];
	rows[row2][row] = rows[row2][row] + (operate) * rows[row][row];
	return operate;
}

void MyMatrix::inverseEliminate(unsigned row, unsigned row2, double operate) {
	for (unsigned i = 0; i < ncols; i++) {
		rows[row2][i] = rows[row2][i] + (operate)* rows[row][i];
	}
}

void MyMatrix::add(unsigned row, unsigned col, double total) {
	rows[row][col] = total;
}
