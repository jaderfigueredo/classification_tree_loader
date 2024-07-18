/*
 * Problem.h
 *
 *  Created on: 24 de fev. de 2022
 *      Author: jader
 */

#ifndef PROBLEM_H_
#define PROBLEM_H_

#include <iostream>
#include <vector>

using namespace std;


class Problem {
public:

	// === ATRIBUTOS DO PROBLEMA ===
	static unsigned int numberOfElemetsInProblem;
	static unsigned int JClasses;
	static unsigned int PAttributes;
	static vector<vector<float>> data;
	static vector<float> numberOfClassElementsInProblem;
	static vector<float> estimateRatioOfClassElemetsInProblem;
	static int numberOfBranches;
	static int numberOfLeafs;
	static int hight;

	Problem();
	virtual ~Problem();

	// Inicializar valores
	static void init(vector<vector<float>> data,
			unsigned int PAttributes,
			unsigned int JClasses);

	// Calcular quantos elementos são de cada classe e suas proporções.
	static bool calcRatio();

	// Imprime o objeto Problem
	static void print();
};


#endif /* PROBLEM_H_ */
