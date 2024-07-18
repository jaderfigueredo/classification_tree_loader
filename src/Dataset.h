/*
 * No.h
 *
 *  Created on: 23 de jan. de 2022
 *      Author: jader
 */
#ifndef DATASET_H_
#define DATASET_H_

#include <iostream>
#include <vector>
#include <math.h>
#include <bits/stdc++.h>
#include <cmath>
#include <limits>
#include <set>
#include "math.h"

using namespace std;

void printVectorDouble(vector<float> list, string title = "", int floatPrecision = 7);
void printDataset(vector<vector<float>> dataset, string title, int floatPrecision = 7);

bool copyVectorDouble(vector<float> source, vector<float> &destination);
bool copyDataset(vector<vector<float>> source, vector<vector<float>> &destination);

class Dataset {

public:
	vector<vector<float>> dataset;
	//int JClasses; // É o total de classes do problema
	//int PAttributes; // É o total de atributos de cada elemento
	float impurity;
	vector<vector<float>> splits;
	int splitAttribute;
	float splitValue;
	int majoritaryClass;

	vector<int> countElementsClasses;

	// É o total de elementos contidos neste subconjunto (nó)
	unsigned int numberOfElementosInNode;
	// É o total de elementos de cada classe contidos neste subconjunto (nó)
	vector<float> numberOfClassElementsInNode;
	// É a proporção estimada de elementos contidos neste subconjunto (nó)
	vector<float> estimateRateOfClassElementsInNode;
	// É a proporção de elementos da classe J contidos no subconjunto (nó t)

	// Probabilidades
	// É a probabilidade de um elemento tanto seja de uma classe j quanto de caia em um nó t
	vector<float> probabilityJClassElementIsPresentInTNode;
	// É a probabilidade de que qualquer elemento de qualquer classe caia dentro do nó t
	float probabilityAnyElementFallsInTNode;
	// É taxa de erro estimada da probabilidade de um elemento ser da classe J e cair dentro de um nó T;
	// É calculado pela probabilityJClassElementIsPresentInTNode * probabilityAnyElementFallsInTNode.
	vector<float> errorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode;
	// Soma das taxas de erro estimadas de elementos serem das classes J e cair dentro de um nó T.
	// É esperado que este valor seja igual a 1
	float sumErrorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode;

	// Metodos construtores
	Dataset();
	Dataset(int PAttributes, int JClasses);
	Dataset(vector<vector<float>> dataset, int PAttributes, int JClasses = 0);

	virtual ~Dataset();

	int defineMajoritaryClass();

	void findSplits();

	vector<Dataset*> findBetterSplit();

	vector<Dataset*> findBetterSplitProfessor();

	vector<Dataset*> findBetterSplitProportional();

	vector<Dataset*> toSplit(int splitIndex);

	// Retorna a impureza do No
	float calcImpurity();

	void print(string label = "", bool showDatasets = true, bool imprimirElementos = false);

	void calcStatistics();

	void printStatistics(string label = "");

	int nAcertos();
};


#endif /* DATASET_H2_ */
