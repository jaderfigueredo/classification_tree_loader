/*
 * Problem.cpp
 *
 *  Created on: 24 de fev. de 2022
 *      Author: jader
 */

#include "Problem.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iomanip>

extern int DISPLAY_OUTS;
extern int DEBUG;

using namespace std;

// === ATRIBUTOS DO PROBLEMA ===
unsigned int Problem::numberOfElemetsInProblem;
unsigned int Problem::JClasses;
unsigned int Problem::PAttributes;
vector<vector<float>> Problem::data;
vector<float> Problem::numberOfClassElementsInProblem;
vector<float> Problem::estimateRatioOfClassElemetsInProblem;
int Problem::numberOfBranches;
int Problem::numberOfLeafs;
int Problem::hight;

Problem::Problem(){

}

Problem::~Problem() {
	// TODO Auto-generated destructor stub
}

void Problem::init(vector<vector<float>> data,
				unsigned int PAttributes,
				unsigned int JClasses
		){
	Problem::data = data; // Não funciona
	if(DISPLAY_OUTS && DEBUG) { cout << "Problem::data.size():\t" << Problem::data.size() << endl; }
	if(DISPLAY_OUTS && DEBUG) { cout << "Problem::data[0].size():\t" << Problem::data[0].size() << endl; }
	//copyDataset(data, Problem::data); // Não funciona | COMO COPIAR DE UM DATASET PRA OUTRO?
	Problem::numberOfElemetsInProblem = data.size();
	Problem::PAttributes = PAttributes;
	Problem::JClasses = JClasses;
	Problem::calcRatio();
	Problem::numberOfBranches = 0;
	Problem::numberOfLeafs = 0;
	Problem::hight = 0;
}

// Calcular quantos elementos são de cada classe e suas proporções.
bool Problem::calcRatio() {
	vector<string> messages;

	// Verificar inconsistências
	if(Problem::numberOfElemetsInProblem < 2) {
		messages.push_back("O número de elementos do conjunto não pode ser menor que 2.");
	}
	if(Problem::PAttributes < 1) {
		messages.push_back("O número de atributos do conjunto não pode ser menor que 1.");
	}
	if(Problem::JClasses < 2) {
		messages.push_back("O número de classes do conjunto não pode ser menor que 2.");
	}
	/*
	if(Problem::numberOfElemetsInProblem != Problem::data.size()) {
		messages.push_back("O número de elementos informado é diferente do que realmente há nos dados.");
	}
	*/

	//return false;

	/*
	if(Problem::PAttributes != Problem::data[0].size()) {
		messages.push_back("É possível que o número informado de atributos seja diferente dos existentes nos dados");
	}
	*/

	// Contar total de elementos de cada classe
	Problem::numberOfClassElementsInProblem.resize(Problem::JClasses);
	// Zerando contadores
	for(unsigned int j = 0; j < Problem::JClasses; j++) {
		Problem::numberOfClassElementsInProblem[j] = 0;
	}

	// Contanto quantos elementos há de cada classe
	for(unsigned int i = 0; i < Problem::numberOfElemetsInProblem; i++) {
		// Adicionou-se Problem::PAttributes+1 devido uma modificação no código que fez com que adicionou o índice do elemento a
		// Problem::data[i][Problem::PAttributes] e fez com que a classe do elemento fosse jogada para uma coluna depois à direita
		int classe = (int) Problem::data[i][Problem::PAttributes+1];

		Problem::numberOfClassElementsInProblem[classe]++;
	}


	// Calcular a proporção de elementos de cada classe
	Problem::estimateRatioOfClassElemetsInProblem.resize(Problem::JClasses);
	for(unsigned int j = 0; j < Problem::JClasses; j++) {
		Problem::estimateRatioOfClassElemetsInProblem[j] = (float) Problem::numberOfClassElementsInProblem[j] / Problem::numberOfElemetsInProblem;
	}


	return (messages.size() < 1);
}

// Imprime o objeto Problem
void Problem::print() {
	if(DISPLAY_OUTS && DEBUG) {
		cout << "Número de elementos no problema:\t" << Problem::numberOfElemetsInProblem << endl;
		cout << "Número de classes no problema:\t\t" << Problem::JClasses << endl;
		cout << "Número de atributos no problema:\t" << Problem::PAttributes << endl;
		cout << "Dados:\t" << Problem::data.size() << "x" << Problem::data[0].size() << endl;
		cout << "Número de elementos em cada classe:\t" << endl;
		for(unsigned int j = 0; j < Problem::numberOfClassElementsInProblem.size(); j++) {
			cout << "[" << j << "]\t" << Problem::numberOfClassElementsInProblem[j] << '\t' << setprecision(7) << Problem::estimateRatioOfClassElemetsInProblem[j]*100 << "%" << endl;
		}

		/*
		cout << "Proporções estimadas de elementos em cada classe:\t" << endl;
		for(unsigned int j = 0; j < Problem::estimateRatioOfClassElemetsInProblem.size(); j++) {
			cout << "[" << j << "]\t" << setprecision(7) << Problem::estimateRatioOfClassElemetsInProblem[j] << endl;
		}
		*/
		cout << "Número de galhos:\t" << Problem::numberOfBranches << endl;
		cout << "Número de folhas:\t" << Problem::numberOfLeafs << endl;
	}
}






















