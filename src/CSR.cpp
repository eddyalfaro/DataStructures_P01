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
		int valInRow = 0;//counts the number of times that a number is added to a row when the matrix is being filled

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

int* CSR::matrixVectorMultiply (int* inputVector){//multiplies a matrix of size nxm by a vector of lenght m
	int* outputVector = new int [n];//initializing array that will be returned
	int* rowVector;//array that will be used to temporaly store the row vectors of the matrix

	for (int i=0; i < n; i++) outputVector[i] =0; // setting output vector values as 0

	for (int i=0; i < n; i++){// moving through the rows values
		rowVector = getRow(i + 1); //obtaining row (i+1) from the matrix
		outputVector[i] = dotProduct(rowVector, inputVector, m); //calculates the dot product between the row vector and the input vector
		delete [] rowVector;//deletes content of the pointer object
	}

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

/**
 * This method will help you fetch a column of values from the matrix
 *
 * Provided the column number as an argument,
 * the method returns an array which contains the column values.
 *
 */
int* CSR::getColumnVector(int col) {//all rows of column col
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

/**
 * This method will help you fetch a row of values from the matrix
 *
 * Provided the row number as an argument,
 * the method returns an array which contains the row values.
 *
 */
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


CSR* CSR::matrixMultiply(CSR& matrixB){//multiplies a matrix of size nxm by another matrix of size mxi the result will be a matrix of size nxi
	int nonZeroes = n * matrixB.m; //maximun number of values that the matrix can hold
	int* rowVector; //array pointer to rows extracted from the matrix on wich the method is called
	int* columnVector; //array pointer to columns extracted from the matrix obtained as a parameter
	CSR* result = new CSR(); //initializing the matrix which will be returned

	//initializing the resulting matrix variables
	result->values = new int[nonZeroes];
	result->colPos = new int[nonZeroes];
	result->rowPtr = new int[n];

	result->n = n; //resultant number of rows equivalent to the number of rows in the caller matrix type object
	result->m = matrixB.m;//resultant number of columns equivalent to the number of columns in the parameter matrix
	result->nonZeros = nonZeroes; //number of nonZeroes initialized to its maximun predicted value

	for (int i = 0 ; i < nonZeroes ; i ++) {//filling arrays wit zeroes
		result->values[i] = 0;
		result->colPos[i] = 0;
	}

	for (int i = 0 ; i < result->n; i++){//filling array with maximun number of nonZeroes
		result->rowPtr[i] = nonZeroes;
	}

	int dot = 0;

	int zeroes; //counter variable that will be used to check if the row in the result matrix is filled with zeroes
	int* rowOfZeroes = new int[result->n]; //this array will be filled with 1 and 0s to determined if a given row is filled with zeroes
											// the index of the array is equivalent to the row number - 1
	for (int row = 0; row < n; row++){//initializing multiplication
		zeroes = 0;//restarting count to zero
		rowVector = getRowVec(row);//obtaining row from calling matrix

		for (int column = 0; column < matrixB.m; column++){//running through the columns in the parameter matrix

			columnVector = matrixB.getColumnVector(column);//obtaining a column from the parameter matrix
			dot = dotProduct(rowVector, columnVector, m); //calculating dot product between the row and the column

			if (dot != 0){//checking if the resultant value of the produt was zero or not and adding it to the matrix
				result->addValue(dot);
				result->addRow(row);
				result->addColumn(column);
			} else {//if the cvalue was zero the counter for zeroes is increased by one
				zeroes++;
			}
		}

		if (zeroes == result->n) rowOfZeroes[row] = 1; //checks if the the given row multiplication result is a row full of zeros
		else  rowOfZeroes[row] = 0; // if it is the array at the index of the given row is filled with 1, otherwise is filled with 0
	}

	for (int row = 0; row < n; row++){//going through the array of zeroes to check which rows are filled with zeroes
		if (rowOfZeroes[row] == 1){//if the row is filled with zeroes the value within the rowPtr array will be changed
			result->rowPtr[row] = result->valuesSize;
		}
	}
	return result;
}

int CSR::dotProduct(int* vector1, int* vector2, int size){// calculates the dot product between two arrays of the same size
	int result = 0;

	for (int i = 0; i < size; i++){
		result += vector1[i] * vector2[i];
	}

	return result;
}

void CSR::addValue (int value){
	values[valuesSize] = value;//assings the value to the array
	valuesSize++;//increases the index
}

void CSR::addColumn(int col){
	colPos[colPosSize] = col;//assigns the calue to the array
	colPosSize++;//increases the index
}

void CSR::addRow(int row){//adds the index of the first nonzero value in the row specified by the parameter within the values array
	if (currRow != row){//checking first if the value that is being added is in the same or in a different row
		valInRow = 0;//if it is a different row the counter of number of values in a row is restarted
	}

	if (valInRow == 0){//when the addRow method is called for the first time in a given row the method enters this statement
		currRow = row; // assings the parameter value to the  current row.
		rowPtr[row] = valuesSize - 1; // inputs the index of this first value in the values array
									// because of this the addrow method has to be called after the addValue method has already been called
	}

	valInRow++; // increases the number of alues that have been inputed in this row
}

int* CSR::getRow(int row){//obtains a row from the matrix starting from 1

	int index_start = rowPtr[row - 1];//obtains the starting index of the nonzero present in the values array
	int index_end = valuesSize; // sets the default ending index as the value before the last element filled

	if (row != n){ // if the row that is being extracted is not the last, the ending index is changed for the element that follows the one presen
					// in the starting index
		index_end = rowPtr[row];
	}

	//cout << "Index in array of values for row N." << row << " have a range of [" << index_start << ", " << index_end << ")" << endl;

	int* rows = new int [m];//generates a pointer array to present the row array

	for (int col = 0; col < m; col++){ // filling the array with zeros
		rows[col] = 0;
	}

	for (int index = index_start; index < index_end ; index++){ // replacing the zero values with the nonZero values contained in the values array
		int colVal = colPos[index];//obtaining the column index value
		rows[colVal] = values[index];//using the column index value the value in values is assigned
	}
	return rows;
}

int* CSR::getColumn(int column){ // gets a column array from the matrix
	int* vector = new int[n]; //pointer object that will be returned

	for (int i = 0; i < n; i++){
		vector[i] = (getRow(i + 1))[column - 1]; // obtaining the value in the column at the i row
	}
	//displayArray(vector, n);
	return vector;
}

void CSR::displayArray(int* array, int size){ // helper method made to dispay the diferent arrays
	for(int i = 0; i < size; i++){
		cout << array[i] << " ";
	}
	cout << endl;
}

void CSR::display(){ //display method to present the matrixes and its elements

	int* tempRow;

	for (int row = 0 ; row < n ; row++){// prints out each row
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
