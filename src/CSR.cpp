/*
 * CSR.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: eddy_
 */

#include <iostream>
using namespace std;

class CSR {

	protected:
		int n; //The number rows of the original matrix
		int m; //The number of columns of the original matrix
		int nonZeros; //The number of non-zeros in the original matrix
		int* values; //value array assuming all are integers
		int* rowPtr; //Array that contains number of non-zero elements in each row of the original matrix
		int* colPos; //Array that contains the column number from the original matrix for the non-zero values.

		int currRow = 0; //value of the row member to which the last non-zero value was added
		int valuesSize = 0; //value of the current number of nonZero values that have been added to values array
		int colPosSize = 0; //value of the current number row values added to the colPos array
		int valInRow = 0;//There may be others you may need

	public:
		CSR (); //default or empty constructor
		CSR (CSR& martrixB); //copy constructor
		CSR (int rows, int cols, int numNonZeros);

		int getNumRows ( );
		void addValue (int value); //add a new value in the values array
		void addColumn (int col);//add a column in the colPos array
		void addRow (int row); //add a row in the rowPtr array
		void display (); //print the contents of the three arrays. Each array must be on a different line (separated by a
                         //new line character) and you have exactly a single space between each value printed.

		void displayArray(int* array, int size); //displays the given array in console

		int* getRow(int row); //returns an array of the given row
		int* getColumn(int column); //returns an array of the given column
		int dotProduct(int* vector1, int* vector2, int size); //calculates the dot product between two vectors

		int* getColumnVector(int column);//helper function given by TA
		int* getRowVec(int row); //helper funstion given by TA

		int* matrixVectorMultiply (int* intputVector); //matrix-vector multiplication
		CSR* matrixMultiply (CSR& matrixB); //matrix-matrix multiplication
		~CSR(); //destructor
		//You may have other methods
};
//some of the methods – Sridhar Radhakrishnan

CSR::CSR ( ) {
	n = 0;
	m = 0;
	nonZeros = 0;

	currRow = 0;
	valuesSize = 0;
	colPosSize = 0;

	values = NULL;
	rowPtr = NULL;
	colPos = NULL;
}

CSR::CSR (int rows, int cols, int numNonZeros) {
	n = rows;
	m = cols;
	nonZeros = numNonZeros;
	values = new int [nonZeros];
	colPos = new int [nonZeros];
	rowPtr = new int [n];

	for (int i = 0; i < n; i++) rowPtr[i] = (nonZeros);

	currRow = 0;
	valuesSize = 0;
	colPosSize = 0;
}

CSR::CSR (CSR& martrixB){
	n = martrixB.n;
	m = martrixB.m;
	nonZeros = martrixB.nonZeros;
	values = martrixB.values;
	colPos = martrixB.colPos;
	rowPtr = martrixB.rowPtr;

	currRow = martrixB.currRow;
	valuesSize = martrixB.valuesSize;
	colPosSize = martrixB.colPosSize;
}

int* CSR::matrixVectorMultiply (int* inputVector){
	int* outputVector = new int [n];
	int* rowVector;

	for (int i=0; i < n; i++) outputVector[i] =0; // setting output vector values as 0

	for (int i=0; i < n; i++){// moving through the rows values
		rowVector = getRow(i + 1);
			 for (int j= 0 ; j < m; j++) {
				 outputVector[i] += rowVector[j] * inputVector[j];
			 }
		delete [] rowVector;
	}

	/*
	for (int i=0; i < n; i++) outputVector[i] =0; // setting output vector values as 0

	for (int i=0; i < n; i++){// moving through the rows values
		 for (int j=rowPtr[i]; j < rowPtr[i+1]; j++) { // obtaining the range of non-zero values within the row
			 outputVector[i] = outputVector[i] + values[j] * inputVector[colPos[j]];
			 // addition of the multiplication of non-zero values within the row, the value that multiplies them is
			 // found within the input vector at the index given by the colPos array
		 }
	}
 */
	return outputVector;
}

CSR::~CSR ( ) {
	if (values != NULL) delete [ ] values;
	if (rowPtr != NULL) delete [ ] rowPtr;
	if (colPos != NULL) delete [ ] colPos;

	//cout << "CSR Object Destroyed!!" << endl;

	n = 0;
	m = 0;
	nonZeros = 0;

	currRow = 0;
	valuesSize = 0;
	colPosSize = 0;
}

int* CSR::getColumnVector(int col) {		//all rows of column col
	int *colVector = new int[n];
	int r;
	for (int i = 0; i < n; i++)
		colVector[i] = 0;

	bool found;
	int k, j;

	k = 0;
	for (int i = 0; i < n - 1; i++) {
		r = rowPtr[i + 1] - rowPtr[i];
		k = rowPtr[i];
		found = false;
		j = 0;
		while ((j < r) && !found) {
			if (colPos[k] == col) {
				found = true;
				colVector[i] = values[k];
			}
			k++;
			j++;
		}
	}
	int p = rowPtr[n - 1];
	found = false;
	while ((p < (nonZeros)) && (!found)) {
		if (colPos[p] == col) {
			colVector[n - 1] = values[p];
			found = true;
		} else
			p++;
	}
	return colVector;
}

int* CSR::getRowVec(int row) {

	int *vector = new int[n];
	for (int i = 0; i < n; i++)
		vector[i] = 0;

	if (row < n - 1) {

		for (int i = rowPtr[row]; i < rowPtr[row + 1]; i++) {
			for (int j = 0; j < m; j++) {
				if (colPos[i] == j)
					vector[j] = values[i];
			}
		}
	} else {
		for (int i = rowPtr[row]; i < nonZeros; i++) {
			for (int j = 0; j < m; j++) {
				if (colPos[i] == j)
					vector[j] = values[i];
			}
		}
	}
	return vector;
}


CSR* CSR::matrixMultiply(CSR& matrixB){
	int nonZeroes = n * matrixB.m;
	int* rowVector;
	int* columnVector;
	CSR* result = new CSR();

	result->values = new int[nonZeroes];
	result->colPos = new int[nonZeroes];
	result->rowPtr = new int[n];

	result->n = n;
	result->m = matrixB.m;
	result->nonZeros = nonZeroes;

	for (int i = 0 ; i < nonZeroes ; i ++) {
		result->values[i] = 0;
		result->colPos[i] = 0;
	}

	for (int i = 0 ; i < result->n; i++){
		result->rowPtr[i] = nonZeroes;
	}

	int dot = 0;

	int zeroes;
	int* rowOfZeroes = new int[result->n];

	for (int row = 0; row < n; row++){
		zeroes = 0;
		rowVector = getRowVec(row);

		for (int column = 0; column < matrixB.m; column++){

			columnVector = matrixB.getColumnVector(column);
			dot = dotProduct(rowVector, columnVector, m);

			if (dot != 0){

				/*cout << "Row: " << row << endl;
				displayArray(rowVector, n);
				cout << "Column: " << column << endl;
				displayArray(columnVector, matrixB.m);
				cout << endl << "Dot product: " << dot << endl << endl;*/

				result->addValue(dot);
				result->addRow(row);
				result->addColumn(column);
			} else {
				zeroes++;
			}
		}

		if (zeroes == result->n) rowOfZeroes[row] = 1;
		else  rowOfZeroes[row] = 0;
	}

	for (int row = 0; row < n; row++){
		if (rowOfZeroes[row] == 1){
			result->rowPtr[row] = result->valuesSize;
		}
	}
	//result->displayArray(result->values, nonZeroes);
	//result->nonZeros = result->valuesSize;
	//cout << result->n << "x" << result->m << endl;
	return result;
}

int CSR::dotProduct(int* vector1, int* vector2, int size){
	int result = 0;

	for (int i = 0; i < size; i++){
		result += vector1[i] * vector2[i];
	}
	return result;
}

void CSR::addValue (int value){
	values[valuesSize] = value;
	//cout << "Value has not been assigned to values[" << valuesSize << "]" << endl;
	//cout << "Assigning the value: " << value << endl;
	//cout << endl;
	valuesSize++;
}

void CSR::addColumn(int col){
	colPos[colPosSize] = col;
	//cout << "Value has not been assigned to colPos[" << colPosSize << "]" << endl;
	//cout << "Assigning the value: " << col << endl;
	//cout << endl;
	colPosSize++;
}

void CSR::addRow(int row){
	if (currRow != row){
		valInRow = 0;
	}

	if (valInRow == 0){
		currRow = row;
		valInRow++;
		rowPtr[row] = valuesSize - 1;
	}
}

int* CSR::getRow(int row){//rows start counting from 1

	int index_start = rowPtr[row - 1];
	int index_end = valuesSize;

	if (row != n){
		index_end = rowPtr[row];
	}

	//cout << "Index in array of values for row N." << row << " have a range of [" << index_start << ", " << index_end << ")" << endl;

	int* rows = new int [m];

	for (int col = 0; col < m; col++){
		rows[col] = 0;
	}

	for (int index = index_start; index < index_end ; index++){
		int colVal = colPos[index];
		rows[colVal] = values[index];
	}

	/*
	cout << "Extracted row N." << row << endl;
	cout << "[ ";
	for (int col = 0; col < m; col++){
		cout << rows[col] << " ";
	}
	cout << "]" << endl << endl;
	*/

	return rows;
}

int* CSR::getColumn(int column){
	int* vector = new int[n];

	for (int i = 0; i < n; i++){
		vector[i] = (getRow(i + 1))[column - 1];
	}
	//displayArray(vector, n);
	return vector;
}

void CSR::displayArray(int* array, int size){
	for(int i = 0; i < size; i++){
		cout << array[i] << " ";
	}
	cout << endl;
}

void CSR::display(){

	int* tempRow;

	for (int row = 0 ; row < n ; row++){
		tempRow = getRowVec(row);
		displayArray(tempRow, m);
		delete [] tempRow;
	}

	cout << "rowPtr: ";
	displayArray(rowPtr, n);
	cout << "colPos: ";
	displayArray(colPos, nonZeros);
	cout << "values: ";
	displayArray(values, nonZeros);
}

int CSR::getNumRows(){
	return n;
}

int main ( ) {

   CSR* A;
   CSR* B;
   int* aVector;
   int numRows, numColumns, numNonZeros;
   int row, col, value;

   //read in the first matrix
   cin >> numRows >> numColumns;
   cin >> numNonZeros;

   A = new CSR (numRows, numColumns, numNonZeros);

   for (int i=0; i < numNonZeros; i++) {
	cin >> row >> col >> value;
	(*A).addValue (value);
	(*A).addRow (row);//needs to be done cleverly in the method
	(*A).addColumn (col);
   }

   cout << "Matrix A:" << endl;

   (*A).display ( );

   CSR* C = new CSR (*A); //calling the copy constructor
   cout << "Copied Matrix C:" << endl;
   (*C).display ( );

//read in the second matrix which is similar to the first into the CSR pointer object B and display; Write code for this
   cin >> numRows >> numColumns;
   cin >> numNonZeros;
   B = new CSR (numRows, numColumns, numNonZeros);

   for (int i=0; i < numNonZeros; i++) {
		cin >> row >> col >> value;
		(*B).addValue (value);
		(*B).addRow (row);//needs to be done cleverly in the method
		(*B).addColumn (col);
   }

   cout << "Matrix B:" << endl;

   (*B).display ( );


//read in the vector
   //cout << "Reading vector" << endl;
    cin >> numColumns;
   //cout << numColumns << endl;
    aVector = new int [numColumns];

    for (int i=0; i < numColumns; i++){
    	cin >> aVector[i];
    }

    cout << "Vector:" << endl;
    CSR* temp = new CSR();
    (*temp).displayArray(aVector, numColumns);

//Matrix-Vector Multiplication
    cout << "A*vector:" << endl;
    int* resultVector = (*A).matrixVectorMultiply (aVector);
    for (int i=0; i < (*A).getNumRows ( ); i++)
		cout << resultVector [i] << " ";
    cout << endl;


//Matrix-Matrix Multiplication
    cout << "A*B:" << endl;
    CSR* resultMatrix = (*C).matrixMultiply (*B);
    (*resultMatrix).display ( );

// Call the destructors
     delete [ ] aVector;
     delete [ ] resultVector;
     delete A;
     delete B;
     delete C;
     delete resultMatrix;

    return 0;
}
