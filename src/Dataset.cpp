/*
 * No.h
 *
 *  Created on: 23 de jan. de 2022
 *      Author: jader
 */
#include <stdlib.h>
#include "Dataset.h"
#include "Problem.h"

extern int DISPLAY_OUTS;
extern int DEBUG;

using namespace std;

bool compare(double a, double b) {
	return ((a < b-std::numeric_limits<double>::epsilon()));
}

void printVectorDouble(vector<float> list, string title, int floatPrecision) {
	if(DISPLAY_OUTS) {
		cout << title << endl;
		for(unsigned int i = 0; i < list.size(); i++) {
			cout << std::setprecision(floatPrecision) << list[i] << "\t" << endl;
		}
		cout << endl;
	}
}

void printDataset(vector<vector<float>> dataset, string title = "", int floatPrecision){
	if(DISPLAY_OUTS) {
		cout << title << endl;
		for(unsigned int i = 0; i < dataset.size(); i++) {
			for(unsigned int j = 0; j < dataset[i].size(); j++) {
				cout << setprecision(floatPrecision) << dataset[i][j] << "\t";
			}
			cout << endl;
		}
	}
}

// Copia todos os elementos vector<float> de um vetor para outro vetor
bool copyVectorDouble(vector<float> source, vector<float> &destination) {
	// O processo de cópia começará após o último elemento do vetor de destino, adicionando elementos no final
	// Se o vetor de destino estiver vazio, tudo será copiado normalmente desde o início
	unsigned int i = destination.size();
	unsigned int destinationSize = destination.size()+(source.size()-destination.size());

	// Aumenta o tamanho do vetor de destino para igual ao tamanho do vetor de origem.
	if(destination.size() < source.size()) {
		destination.resize(destinationSize);
	}

	// copia todos ele
	for(; i < destinationSize; i++) {
		destination[i] = source[i];
	}
	return true;
}


// Copia todos os elementos vector<vector<float>> de um vetor para outro vetor
bool copyDataset(vector<vector<float>> source, vector<vector<float>> &destination) {
	// O processo de cópia começará após o último elemento do vetor de destino, adicionando elementos no final
	// Se o vetor de destino estiver vazio, tudo será copiado normalmente desde o início
	unsigned int i = destination.size();
	unsigned int destinationSize = destination.size()+(source.size()-destination.size());

	// Aumenta o tamanho do vetor de destino para igual ao tamanho do vetor de origem.
	if(destination.size() < source.size()) {
		destination.resize(destinationSize);
	}

	// copia todos ele
	for(; i < destinationSize; i++) {
		copyVectorDouble(source[i], destination[i]);
	}
	return true;
}

// Constructors Methods
Dataset::Dataset(){ // @suppress("Class members should be properly initialized")
	this->impurity = 0.0;
	this->splitAttribute = -1;
	this->splitValue = -1;
	this->majoritaryClass = -1;
}

Dataset::Dataset(int PAttributes, int JClasses){ // @suppress("Class members should be properly initialized")
	this->impurity = 0.0;
	this->splitAttribute = -1;
	this->majoritaryClass = -1;
	this->splitValue = -1;

	this->numberOfElementosInNode = 0;
	this->probabilityAnyElementFallsInTNode = 0;
	this->sumErrorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode = 0;
}

Dataset::Dataset(vector<vector<float>> dataset, int PAttributes, int JClasses){ // @suppress("Class members should be properly initialized")
	// Calcula automaticamente o número de atributos considerando a
	// última coluna como sendo as classes
	this->impurity = 0.0;
	this->splitAttribute = -1;
	this->splitValue = -1;
	this->dataset = dataset;

	this->numberOfElementosInNode = 0;
	this->probabilityAnyElementFallsInTNode = 0;
	this->sumErrorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode = 0;
}

Dataset::~Dataset(){}

// Retorna a classe majoritária no conjunto, ou seja, a classe da maioria dos elementos
// Em caso de empate qualquer uma das classes pode ser a majoritária
int Dataset::defineMajoritaryClass() {

	// Criando um vetor com um índice para salvar o contador para cada classe
	vector<float> countElementsOfClasses(Problem::JClasses);

	// Contando quantos elementos existem para de cada classe
	for(unsigned int i = 0; i < this->dataset.size(); i++) {
		int iClass = (int) this->dataset[i][Problem::PAttributes];
		countElementsOfClasses[iClass]++;
	}

	// Encontrando qual teve a maior contagem
	int majoritaryClassIndex = 0;
	for(unsigned int i = 0; i < countElementsOfClasses.size(); i++) {
		if(countElementsOfClasses[i] > countElementsOfClasses[majoritaryClassIndex]) {
			majoritaryClassIndex = i;
		}
	}

	this->majoritaryClass = majoritaryClassIndex;
	return this->majoritaryClass;
}

void Dataset::findSplits() {
	bool(*fcompare)(double,double) = compare;

	// Inicializando os vetores de limiares em this->splits
	this->splits.resize(Problem::PAttributes);

	// Percorrendo cada Atributo
	for(unsigned int p = 0; p  < Problem::PAttributes; p++) {
		// Criar um vetor de sets com os valores de linhas e colunas transpostos
		// Percorrendo cada linha de uma coluna
		set<double,bool(*)(double,double)> setAux (fcompare);

		// Filtra os valores dos splits eliminando valores duplicados
		for(unsigned int i = 0; i < this->dataset.size(); i++) {
			setAux.insert(this->dataset[i][p]);
		}
		// Recolocando os valores em um vetor para poder acessar pelos índices
		vector<float> vecAux(setAux.begin(), setAux.end());

		// Obtendo as médias entre os valores
		for(unsigned int i = 0; i < vecAux.size()-1; i++) {
			float avg = (vecAux[i] + vecAux[i+1]) / 2;
			this->splits[p].push_back(avg);
		}

		// É possível que as linhas abaixo estivessem erradas pois não vi a
		// separação dos valores por atributos (p).
		//vector<float> splits(setAux.begin(), setAux.end());
		//this->splits.push_back(splits);
	}
}

// Encontrar o melhor ponto para partição dos dados.
// O melhor ponto é o que proporciona a maior diferença de impureza de antes da separação para depois dela.
vector<Dataset*> Dataset::findBetterSplit() {
	// Verificar se os splits possíveis já foram armazenados
	if(this->splits.size() == 0) {
		this->findSplits();
		this->calcImpurity();
	}

	// Determinar a Classe majoritária do conjunto
	this->defineMajoritaryClass();

	// Calcular estatísticas do nó, pois elas são usadas para encontrar a melhor partição dos dados
	this->calcStatistics();

	// A primeira diferença é do conjunto atual para ele mesmo, que é zero.
	float rangeImpurity = 0.0;

	// Vetor que armazena os ponteiros para o subconjunto esquerdo e direito
	vector<Dataset*> splitedSets(2);
	splitedSets[0] = NULL;
	splitedSets[1] = NULL;


	// Percorrer cada split
	for(unsigned int i = 0; i < this->splits.size(); i++) {
		for(unsigned int j = 0; j < this->splits[i].size(); j++) {

			// Prepara os dois subconjuntos que receberão os dados particionados
			Dataset* leftSet = new Dataset(Problem::PAttributes, Problem::JClasses);
			Dataset* rightSet = new Dataset(Problem::PAttributes, Problem::JClasses);

			// Fazer a separação de todos os elementos do conjunto
			for(unsigned int ii = 0; ii < this->dataset.size(); ii++) {
				// Verifica se o elemento é menor que o limiar de separção
				if(this->dataset[ii][i] <= this->splits[i][j]) {
					leftSet->dataset.push_back(this->dataset[ii]);
				}
				else {
					rightSet->dataset.push_back(this->dataset[ii]);
				}
			}

			// Calcular a impureza dos conjuntos separados
			leftSet->calcImpurity();
			rightSet->calcImpurity();

			float newRangeImpurity = this->impurity - (leftSet->impurity) - (rightSet->impurity);

			// Se a diferença de impureza for maior que a diferença anterior, substituir a diferença.
			if(newRangeImpurity > rangeImpurity) {
				rangeImpurity = newRangeImpurity;
				this->splitAttribute = i;
				splitedSets[0] = leftSet;
				splitedSets[1] = rightSet;
				// Atualiza o valor da melhor impureza até o momento.
				rangeImpurity = newRangeImpurity;
			}
			else {
				delete leftSet;
				delete rightSet;
			}

		}// Repetir até que não haja mais seperações
	}

	return splitedSets;
}


// Encontrar o melhor ponto para partição dos dados.
// O melhor ponto é o que proporciona a maior diferença de impureza de antes da separação para depois dela.
// Neste método, a impureza leva em consideração a proporção de elementos contidas em um nó de modo que
// Se há muita impureza em um nó filho, mas com poucos elementos, a diferença continua baixa.
vector<Dataset*> Dataset::findBetterSplitProfessor() {
	// Verificar se os splits possíveis já foram armazenados
	if(this->splits.size() == 0) {
		this->findSplits();
		this->calcImpurity();
	}

	// Determinar a Classe majoritária do conjunto
	this->defineMajoritaryClass();

	// Calcular estatísticas do nó, pois elas são usadas para encontrar a melhor partição dos dados
	this->calcStatistics();

	// A primeira diferença é do conjunto atual para ele mesmo, que é zero.
	float rangeImpurity = 0.0;

	// Vetor que armazena os ponteiros para o subconjunto esquerdo e direito
	vector<Dataset*> splitedSets(2);
	splitedSets[0] = NULL;
	splitedSets[1] = NULL;


	// Percorrer cada split
	for(unsigned int i = 0; i < this->splits.size(); i++) {
		for(unsigned int j = 0; j < this->splits[i].size(); j++) {

			// Prepara os dois subconjuntos que receberão os dados particionados
			Dataset* leftSet = new Dataset(Problem::PAttributes, Problem::JClasses);
			Dataset* rightSet = new Dataset(Problem::PAttributes, Problem::JClasses);

			// Fazer a separação de todos os elementos do conjunto
			for(unsigned int ii = 0; ii < this->dataset.size(); ii++) {
				// Verifica se o elemento é menor que o limiar de separção
				if(this->dataset[ii][i] <= this->splits[i][j]) {
					leftSet->dataset.push_back(this->dataset[ii]);
				}
				else {
					rightSet->dataset.push_back(this->dataset[ii]);
				}
			}

			// Calcular a impureza dos conjuntos separados
			leftSet->calcImpurity();
			rightSet->calcImpurity();

			// TODO substituir os calculos abaixo pelas variáveis contantes na classe Problem::

			// Calcular a diferença da impureza entre esses dois valores
			float leftRatio = ((float) leftSet->dataset.size()) / ((float) this->dataset.size());
			float rightRatio = ((float) rightSet->dataset.size()) / ((float) this->dataset.size());

			//float newRangeImpurity = this->impurity - leftSet->impurity - rightSet->impurity;
			float newRangeImpurity = this->impurity - (leftSet->impurity * leftRatio) - (rightSet->impurity * rightRatio);

			// Se a diferença de impureza for maior que a diferença anterior, substituir a diferença.
			//if(newRangeImpurity > rangeImpurity) {
			if(newRangeImpurity > rangeImpurity && newRangeImpurity > 0.07) {
				rangeImpurity = newRangeImpurity;
				this->splitAttribute = i;
				splitedSets[0] = leftSet;
				splitedSets[1] = rightSet;
				// Atualiza o valor da melhor impureza até o momento.
				rangeImpurity = newRangeImpurity;
			}
			else {
				delete leftSet;
				delete rightSet;
			}

		}// Repetir até que não haja mais seperações
	}

	return splitedSets;
}


// Encontrar o melhor ponto para partição dos dados.
// O melhor ponto é o que proporciona a maior diferença de impureza de antes da separação para depois dela.
// Neste método, a impureza leva em consideração a proporção de elementos contidas em um nó de modo que
// Se há muita impureza em um nó filho, mas com poucos elementos, a diferença continua baixa.
vector<Dataset*> Dataset::findBetterSplitProportional() {
	// Verificar se os splits possíveis já foram armazenados
	if(this->splits.size() == 0) {
		this->findSplits();
		this->calcImpurity();
	}

	// Determinar a Classe majoritária do conjunto
	this->defineMajoritaryClass();

	// Calcular estatísticas do nó, pois elas são usadas para encontrar a melhor partição dos dados
	this->calcStatistics();

	// A primeira diferença é do conjunto atual para ele mesmo, que é zero.
	float rangeImpurity = 0.0;

	// Vetor que armazena os ponteiros para o subconjunto esquerdo e direito
	vector<Dataset*> splitedSets(2);
	splitedSets[0] = NULL;
	splitedSets[1] = NULL;


	// Percorrer cada split
	for(unsigned int i = 0; i < this->splits.size(); i++) {
		for(unsigned int j = 0; j < this->splits[i].size(); j++) {

			// Prepara os dois subconjuntos que receberão os dados particionados
			Dataset* leftSet = new Dataset(Problem::PAttributes, Problem::JClasses);
			Dataset* rightSet = new Dataset(Problem::PAttributes, Problem::JClasses);

			// Fazer a separação de todos os elementos do conjunto
			for(unsigned int ii = 0; ii < this->dataset.size(); ii++) {
				// Verifica se o elemento é menor que o limiar de separção
				if(this->dataset[ii][i] <= this->splits[i][j]) {
					leftSet->dataset.push_back(this->dataset[ii]);
				}
				else {
					rightSet->dataset.push_back(this->dataset[ii]);
				}
			}

			// Calcular a impureza dos conjuntos separados
			leftSet->calcImpurity();
			rightSet->calcImpurity();

			// TODO substituir os calculos abaixo pelas variáveis contantes na classe Problem::

			// Obter a proporção de elementos que foi para o filho esquerdo e para o filho direito
			float leftRatio = ((float) leftSet->dataset.size()) / ((float) this->dataset.size());
			float rightRatio = ((float) rightSet->dataset.size()) / ((float) this->dataset.size());

			// Calcular a diferença de impureza considerando as proporções de elementos em cada nó
			float newRangeImpurity = this->impurity - (leftSet->impurity * leftRatio) - (rightSet->impurity * rightRatio);

			// Se a diferença de impureza for maior que a diferença anterior, substituir a diferença.
			//if(newRangeImpurity > rangeImpurity) {
			if(newRangeImpurity > rangeImpurity && newRangeImpurity > 0.07) {
				rangeImpurity = newRangeImpurity;
				this->splitAttribute = i;
				splitedSets[0] = leftSet;
				splitedSets[1] = rightSet;
				// Atualiza o valor da melhor impureza até o momento.
				rangeImpurity = newRangeImpurity;
			}
			else {
				delete leftSet;
				delete rightSet;
			}

		}// Repetir até que não haja mais seperações
	}

	return splitedSets;
}


// Retorna a impureza do No
float Dataset::calcImpurity() {
	bool flagShow = false;

	// Contar total de elementos
	int totalElements = this->dataset.size();

	// Tratando caso de impureza de um nó vazio
	if(totalElements == 0) {
		if(flagShow) {
			if(DISPLAY_OUTS && DEBUG) { cerr << "Erro. Não é possível calcular a impureza de um conjunto vazio" << endl; }
		}
		// Por que atribui-se o número de classes ao valor da impureza quando o conjunto é vazio?
		this->impurity = Problem::JClasses;
		return this->impurity;
	}

	// Contar total de elementos de cada classe
	if(flagShow) {
		if(DISPLAY_OUTS) { cout << "Inicializando contadores de elementos de cada classe..." << endl; }
	}
	this->countElementsClasses.resize(Problem::JClasses);
	for(unsigned int j = 0; j < Problem::JClasses; j++) {
		this->countElementsClasses[j] = 0;
	}

	if(flagShow) {
		if(DISPLAY_OUTS) { cout << "Contando quantidade de elementos de cada classe..." << endl; }
	}
	for(int i = 0; i < totalElements; i++) {
		// Adicionou-se Problem::PAttributes+1 devido uma modificação no código que fez com que adicionou o índice do elemento a
		// Problem::data[i][Problem::PAttributes] e fez com que a classe do elemento fosse jogada para uma coluna depois à direita
		int classe = (int) this->dataset[i][Problem::PAttributes+1];
		this->countElementsClasses[classe]++;
	}

	// Calcular a proporção de elementos de cada classe
	if(flagShow) {
		if(DISPLAY_OUTS) { cout << "Calculando proporções de elementos de cada classe..." << endl; }
	}
	vector<float> proporcoesClassesNoh(Problem::JClasses);
	for(unsigned int j = 0; j < Problem::JClasses; j++) {
		// Tratando exceção que causa divisão por zero.
		if(totalElements == 0) {
			proporcoesClassesNoh[j] = 0.0;
			continue;
		}
		proporcoesClassesNoh[j] = (float) this->countElementsClasses[j] / totalElements;
	}

	if(DISPLAY_OUTS && DEBUG) {
		// Imprimindo as proporções
		if(flagShow) { cout << endl << "Proporcoes de um nó:" << endl; }
		for(unsigned int j = 0; flagShow && j < Problem::JClasses; j++) {
			cout << proporcoesClassesNoh[j] << endl;
		}

		// Calcular a impureza
		if(flagShow) {
			cout << "Calculando a impureza..." << endl;
		}
	}
	this->impurity = 0;

	if(DISPLAY_OUTS && DEBUG) {
		if(flagShow) { cerr << "totalElements:\t" << totalElements << endl; }
		for(unsigned int i = 0; flagShow && i < this->countElementsClasses.size(); i++) {
			cerr << "countElementsClasses " << i << ":\t" << this->countElementsClasses[i] << ":\t" << proporcoesClassesNoh[i]*100 << '%' << endl;
		}
		/*
		for(unsigned int i = 0; flagShow && i < proporcoesClassesNoh.size(); i++) {
			cerr << "proporcoesClassesNoh " << i << ":\t" << proporcoesClassesNoh[i] << endl;
		}
		*/

		if(flagShow) {
			cout << "terminou de calcular a impureza?: "  << this->impurity << endl;
		}
	}

	for(unsigned int j = 0; j < proporcoesClassesNoh.size(); j++) {
		// Tratando caso de proporção de alguma classe for igual a zero
		if(this->countElementsClasses[j] > 0) {
			this->impurity -= proporcoesClassesNoh[j] * log10(proporcoesClassesNoh[j]);
		}
	}

	if(flagShow && DEBUG) {
		if(DISPLAY_OUTS && DEBUG) { cout << "Impurity: " << setprecision(7) << this->impurity << endl; }
	}

	return this->impurity;
}


void Dataset::print(string label, bool showDatasets, bool imprimirElementos) {
	if(DISPLAY_OUTS) {
		cout << label << endl;
		cout << "Dataset: " << this << endl;
		cout << "Total Elementos: " << this->dataset.size() << "\t" << "JClasses: " << Problem::JClasses << "\tPAttributes: " << Problem::PAttributes << endl;
		cout << std::setprecision(7) << "Impurity: " << this->impurity << endl;
	}

	/*
	cout << "Total Splits:\t\t";
	for(unsigned int j = 0; j < this->splits.size(); j++) {
		cout << "[" << j << "]: " << this->splits[j].size() << "\t\t";
	}
	cout << endl;
	*/
	if(DISPLAY_OUTS) { cout << "splitAttribute: " << this->splitAttribute << "\t"; }
	/* Ao carregar os nós, a informação this->splitIndex não é usada, portanto o valor do split
	 * é carregado diretamente no atributo
	 *
	if(this->splitAttribute > -1 && this->splitIndex > -1) {
		cout << "splitValue: " << this->splits[this->splitAttribute][this->splitIndex] << endl;
	}
	else {
	*/
	if(DISPLAY_OUTS) { cout << "splitValue: " << this->splitValue << endl; }
	/*
	}
	*/


	if(DISPLAY_OUTS) { cout << "Classe Majoritária: " << this->majoritaryClass << endl; }
	if(showDatasets) {
		printDataset(this->dataset, "[ DATASET ]");
		printDataset(this->splits, "[ SPLITS ]");
	}

	// Variáveis de estatísticas de classificação
	unsigned int quantidadeAcertos = 0;
	float taxaAcertos = 0.0;
	unsigned int quantidadeErros = 0;
	float taxaErros = 0.0;

	// Se for para imprimir os elementos e o nó atual for um nó folha, então imprima-os
	if(imprimirElementos) {
		for(unsigned int xx = 0; xx < this->dataset.size(); xx++) {
			if(DISPLAY_OUTS) { cout << xx+1 << "\t"; }
			for(unsigned int yy = 0; yy < this->dataset[xx].size(); yy++) {
				if(yy == Problem::PAttributes+1) {
					if(DISPLAY_OUTS) { cout << std::setprecision(0); }
				}
				else {
					if(DISPLAY_OUTS) { cout << std::setprecision(7); }
				}
				if(DISPLAY_OUTS) { cout << this->dataset[xx][yy] << "\t"; }

			}
			// Contando cada acerto
			// Adicionou-se Problem::PAttributes+1 devido uma modificação no código que fez com que adicionou o índice do elemento a
			// Problem::data[i][Problem::PAttributes] e fez com que a classe do elemento fosse jogada para uma coluna depois à direita
			if((int) this->dataset[xx][Problem::PAttributes+1] == (int) this->majoritaryClass) {
				quantidadeAcertos++;
				if(DISPLAY_OUTS) { cout << "Acertou"; }
			}
			else {
				if(DISPLAY_OUTS) { cout << "Errou"; }
			}
			if(DISPLAY_OUTS) { cout << endl; }
		}
		if(DISPLAY_OUTS) { cout << std::setprecision(3); }

		// Imprimindo estatísticas de classificação
		if(DISPLAY_OUTS) { cout << "Estatísticas de classificação em treinamento:" << endl; }

		taxaAcertos = quantidadeAcertos * 100 / this->dataset.size();
		quantidadeErros = this->dataset.size() - quantidadeAcertos;
		taxaErros = quantidadeErros * 100 / this->dataset.size();

		if(DISPLAY_OUTS) {
			cout << "N. erros:\t" << quantidadeErros << "\t";
			cout << "%  erros:\t" << taxaErros << endl;
			cout << "N. acertos:\t" << quantidadeAcertos << "\t";
			cout << "%  acertos:\t" << taxaAcertos << endl;

			cout << std::setprecision(7);
		}
	}
}



void Dataset::calcStatistics(){

	// Estatísticas específicas do subconjunto (nó)
	this->numberOfElementosInNode = this->dataset.size();


	// Contando o número de elementos de cada classes contidos no subconjunto (nó)
	this->numberOfClassElementsInNode.resize(Problem::JClasses);
	// Inicializando os contadores
	for(unsigned int j = 0; j < Problem::JClasses; j++) {
		this->numberOfClassElementsInNode[j] = 0;
	}
	for(unsigned int j = 0; j < this->dataset.size(); j++) {
		unsigned int iClass = (int) this->dataset[j][Problem::JClasses];
		this->numberOfClassElementsInNode[iClass]++;
	}
	// Calculando as proporções de elementos de cada classe contidos no subconjunto (nó) Nj/N
	this->estimateRateOfClassElementsInNode.resize(Problem::JClasses);
	for(unsigned int j = 0; j < this->numberOfClassElementsInNode.size(); j++) {
		cout << this->numberOfClassElementsInNode[j] << " / " << Problem::numberOfClassElementsInProblem[j];
		this->estimateRateOfClassElementsInNode[j] =
				this->numberOfClassElementsInNode[j] / Problem::numberOfClassElementsInProblem[j];
	}

	// Calculando a probabilidade de um elemento tanto seja de uma classe j quanto de caia em um nó t.
	// Isto dá um valor de referente à multiplicação da proporção de elementos de cada classe no problema
	// vezes a proporção de elementos de cada classe no nó
	this->probabilityJClassElementIsPresentInTNode.resize(Problem::JClasses);
	for(unsigned int j = 0; j < this->probabilityJClassElementIsPresentInTNode.size(); j++) {
		// function 2.2
		this->probabilityJClassElementIsPresentInTNode[j] =
			this->estimateRateOfClassElementsInNode[j] /* pi(j) */
			* Problem::estimateRatioOfClassElemetsInProblem[j];
	}

	// Calculando a probabilidade de que qualquer elemento caia dentro do nó t
	this->probabilityAnyElementFallsInTNode = 0;
	for(unsigned int j = 0; j < this->probabilityJClassElementIsPresentInTNode.size(); j++) {
		// function 2.3
		this->probabilityAnyElementFallsInTNode += this->probabilityJClassElementIsPresentInTNode[j];
	}

	// Calculando a taxa de erro estimada da probabilidade de um elemento ser da classe j qualquer
	// e que este elemento caia dentro de um nó t qualquer
	this->errorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode.resize(Problem::JClasses);
	this->sumErrorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode = 0;

	for(unsigned int j = 0; j < this->probabilityJClassElementIsPresentInTNode.size(); j++) {

		this->errorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode[j] =
				this->probabilityJClassElementIsPresentInTNode[j]
				/ this->probabilityAnyElementFallsInTNode;

		this->sumErrorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode +=
				this->errorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode[j];
	}
}


void Dataset::printStatistics(string label) {
	if(DISPLAY_OUTS) {
		cout << label << endl;
		cout << "[ ESTIMATIVAS ]" << endl;
		cout << "Total de elementos no problema:\t" << Problem::numberOfElemetsInProblem << endl;
		printVectorDouble(Problem::numberOfClassElementsInProblem, "Total de elementos de cada classe no problema:\t", 0);
		printVectorDouble(Problem::estimateRatioOfClassElemetsInProblem, "Proporção estimada de elementos de cada classe no problema:\t");
		cout << endl;

		cout << "Total de elementos neste nó:\t" << this->numberOfElementosInNode << endl;
		printVectorDouble(this->numberOfClassElementsInNode, "Total de elementos de cada classe contidos neste nó:\t", 0);
		printVectorDouble(this->estimateRateOfClassElementsInNode, "Proporção estimada de elementos contidos neste nó:\t");
		cout << endl;

		cout << "[ PROBABILIDADES ]" << endl;
		printVectorDouble(this->probabilityJClassElementIsPresentInTNode, "Probabilidade de um elemento tanto seja de uma classe j quanto de caia em um nó t");
		cout << "Probabilidade de que qualquer elemento de qualquer classe caia dentro deste nó: " << this->probabilityAnyElementFallsInTNode << endl;
		printVectorDouble(this->errorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode, "Taxa de erro estimada da probabilidade de um elemento ser da classe J e cair dentro de um nó T");
		cout << "Soma das taxas de erro estimadas de elementos serem das classes J e cair dentro de um nó T: " << this->sumErrorRateEstimateProbabilityThatElementIsInJClassAndFallsIntoTNode << endl;
		cout << endl;
	}
}

int Dataset::nAcertos() {
	// Se for galho, retorna 0
	if(this->majoritaryClass == -1) {
		return 0;
	}

	// Se não
	// Retorna o otal de elementos da classe do nó
	int nAcertos = 0;
	for(long unsigned int i = 0; i < this->dataset.size(); i++) {
		int iClass = Problem::PAttributes+1;
		int _class = (int) this->dataset[i][iClass];
		if(_class == this->majoritaryClass) {
			nAcertos++;
		}
	}
	return nAcertos;
}



/**
 * [OK] 1 - Fazer o cálculo do delta da impureza ser proporcional à quantidade de elementos enviados para
 * o filho direito e filho esquerdo
 *
 * 2 - Voltar na árvore com otimização global para relembrar como estava sendo feito. Se necessário
 * voltar também no artigo.
 *
 *
 * 4 - Começar a escrever a dissertação.
 *
 * 5 - Imprimir àrvore
 * 		https://www.overleaf.com/project/621ff2c7d2f6369f7763bba4
 *
 * 3 - Encontrar um dataset que toque meu coração.
	***
	Human Stress Detection in and through Sleep
	Human Stress Detection in and through Sleep by monitoring physiological data.
	https://www.kaggle.com/laavanya/human-stress-detection-in-and-through-sleep

  	**
  	Human Stress Detection
	Human stress level detection using physiological data
	https://www.kaggle.com/laavanya/stress-level-detection

	*
	The Depression Dataset
	Help researchers to automatically detect depression status of a person
	https://www.kaggle.com/arashnic/the-depression-dataset

	Loan Default Dataset
	Loan Default Classification Problem
	https://www.kaggle.com/yasserh/loan-default-dataset

	skin-cancer-detection-dcnn
	Categorizes the skin cancer images into benign and malignant
	https://www.kaggle.com/hasinisadunikasilva/skincancerdetectiondcnn

	Amazon Seller - Order Status Prediction
	Predict the order success
	https://www.kaggle.com/pranalibose/amazon-seller-order-status-prediction

	Classification in asteroseismology
	RGB-HeB classification
	https://www.kaggle.com/fernandolima23/classification-in-asteroseismology

	Brain Tumor
	Brain Tumor with 7466
	https://www.kaggle.com/jillanisofttech/brain-tumor

	Heart Attack Analysis & Prediction Dataset
	A dataset for heart attack classification
	https://www.kaggle.com/rashikrahmanpritom/heart-attack-analysis-prediction-dataset

 */










