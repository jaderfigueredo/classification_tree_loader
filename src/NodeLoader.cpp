/*
 * No.h
 *
 *  Created on: 23 de jan. de 2022
 *      Author: jader
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include "Dataset.h"
#include "Node.h"
#include "Problem.h"

using namespace std;
namespace fs = std::filesystem;

/*
#ifndef DISPLAY_OUTS
	#define DISPLAY_OUTS 1
	// Outros arquivos com a mesma DEFINITION:
	// Dataset.cpp
	// Node.cpp
	// Problem.cpp
#endif
*/
int DISPLAY_OUTS = 1;
int DEBUG = 0;
#define BRANCH "galho"
#define LEAF "folha"

// Definir pasta e nome do arquivo
// std::filesystem::path cwd = std::filesystem::current_path();
string datasets =  ""; //"/home/jader/Documentos/ambiente-dev-mestrado/Datasets/Iris/";
string fileTreeName = "tree-settings.txt";
string fileElementsName = datasets+"iris.norm.csv";
string filePredictName = "predict.csv";
vector<string> problemClasses;

// Abrir o arquivo
fstream fileTree;
fstream fileElements;
fstream filePredict;

stack<Node*> stack;
string separator = " ";

vector<vector<float>> elements;



// Retorna o índice do filho esquerdo
int getLeftChild(int t) {
	return 2*t+1;
}

// Retorna o índice do filho direito
int getRightChild(int t) {
	return 2*t+2;
}

// Retorna o nó pai de um nó filho
int getParent(int t) {
	return ceil((float) t/2)-1;
}

// Retorna true se o a é <= b
bool doubleCompare(double a, double b) {
	return !(a >= b+(2*std::numeric_limits<double>::epsilon()));
}


// Encontrar uma string em um vector<string>
// Retorna o índice onde a string foi encontrada ou -1, caso contrário
int findStringInVector(string const _key, vector<string> list) {
	for(unsigned int i = 0; i < list.size(); i++) {
		if(_key.compare(list[i]) == 0) {
			return i;
		}
	}
	return -1;
}

string getTabs(int n = 1, string _char = "\t"){
	string tabs = "";
	while(n > 0) {
		tabs += _char;
		n--;
	}
	return tabs;
}

int getLevelFromIndex(int index) {
	// Caso base
	if(index == 0) {
		return 0;
	}

	// Passo recursivo
	return 1+getLevelFromIndex(getParent(index));
}



// Ler um nó
/**
 * Params
 * fstream arquivo: é o ponteiro para o arquivo do qual os dados serão lidos
 */
Node* readNode() {

	// Variáveis
	Node* node = NULL;
	string aux = "";

	// Caso base 1: Se for o fim do arquivo
	if(fileTree.eof()) {
		return NULL;
	}

	fileTree >> aux; // ler um caracter
	//cout << aux << " | "; // << "(" << aux.size() << ")" << " | ";
	//return readNode();

	// Caso base 2: Se o fim do nó, ou seja: ")"
	if(aux.compare(")") == 0) {
		if(DISPLAY_OUTS && DEBUG) { cout << ")" << separator; }
		return NULL;
	}

	// Apenas para evitar falha de seguimentação
	if(aux.compare("(") != 0) {
		if(DISPLAY_OUTS && DEBUG) { cout << separator; }
		return NULL;
	}

	// Criar um novo nó
	node = new Node(new Dataset());
	if(DISPLAY_OUTS && DEBUG) { cout << "(" << separator; }

	// Ler o ID do nó
	int id = 0;
	fileTree >> id; // lendo o id
	if(DISPLAY_OUTS && DEBUG) { cout << id << separator; }

	// ler o tipo do nó
	string kindNode = "-";
	fileTree >> kindNode; // lendo o tipo do nó
	if(DISPLAY_OUTS && DEBUG) { cout << kindNode << separator; }

	// Se o tipo do nó for nó galho
	if(kindNode.compare(BRANCH) == 0) {
		// ler o parâmetro de divisão
		fileTree >> node->dataset->splitAttribute;
		if(DISPLAY_OUTS && DEBUG) { cout << node->dataset->splitAttribute << separator; }

		// ler o valor do limiar
		fileTree >> std::fixed >> std::setprecision(17) >> node->dataset->splitValue;
		cout << std::fixed << std::setprecision(16);
		if(DISPLAY_OUTS && DEBUG) { cout << node->dataset->splitValue << separator; }

		// Anular os atributos de nó folha
		node->dataset->majoritaryClass = -1;
		//cout << node->dataset->majoritaryClass << separator;
		// Passo recursivo: Ler filho esquerdo
		node->left = readNode();

		// Passo recursivo: Ler filho direito
		node->right = readNode();

	}
	// Se o tipo do nó for nó folha
	else {
		// ler a classe do nó
		fileTree >> node->dataset->majoritaryClass;
		if(DISPLAY_OUTS && DEBUG) { cout << node->dataset->majoritaryClass; }

		// Anular os atributos de nó galho
		node->dataset->splitAttribute = -1;
		//cout << node->dataset->splitAttribute;
		node->dataset->splitValue = -1;

		//cout << node->dataset->splitAttribute;

		// Passo recursivo: Ler filho esquerdo
		node->left = NULL;

		// Passo recursivo: Ler filho direito
		node->right = NULL;
	}

	if(DISPLAY_OUTS && DEBUG) { cout << " )"; }
	// Descarta o ")"
	fileTree >> aux;
	return node;
}

vector<vector<float>> loadElements(string fileElementsName, int p, int K) {
	// Abrindo o arquivo
	if(DISPLAY_OUTS && DEBUG) { cout << endl; }
	if(DISPLAY_OUTS && DEBUG) { cout << "O arquivo '" << fileElementsName << "'" << ((fileElements.is_open() ? "" : " não")) << " está aberto" << endl; }


	// CARREGANDO DADOS
	vector<vector<float>> x;
	fileElements.close();
	fileElements.open(fileElementsName, fstream::in);
	if(DISPLAY_OUTS && DEBUG) { cout << endl; }
	if(DISPLAY_OUTS && DEBUG) { cout << "O arquivo '" << fileElementsName << "'" << ((fileElements.is_open() ? "" : " não")) << " está aberto" << endl; }

	/* Testando leitura de arquivo */
	string aux;
	string line;
	string word;
	const char SEPARATOR = ',';

	// Descartando a primeira linha contendo as legendas
	fileElements >> aux;


	// Lendo os dados do arquivo
	string cl;
	int n; // = 150; // Total de elementos
	// int p = 4; // Quantidade de atributos
	// A quantidade de classes será definida durante a leitura do arquivo
	//int K; // = 3; // Quantidade de classes
	//int	somaClasses[K]; // Qu/ TODO AQUIantidade de elementos associados a cada classe K
	//float minValueAttribute = 0; // Valor mínimo de um atributo
	//float maxValueAttribute = 0; // Valor máximo de um atributo
	// TODO AQUI

	// LENDO ARQUIVO CSV SEM ESPAÇAMENTO DEPOIS DA VÍRGULA
	if(!fileElements.is_open()) {
		cerr << "O arquivo não pode ser aberto." << endl;
		return x;
	}

	// A contagem de classes será feita em uma estrutura de map (chave-valor)
	// já durante a leitura do arquivo
	set<string> setClasses;


	// Zerando a quantidade de elementos inseridos.
	n = 0;
	getline(fileElements, line); // descartando a primeira linha
	for(int i = 0; !fileElements.eof(); i++) {
		//x[i].resize(p+1);
		vector<float> attributes(p+1);

		// Lê uma linha do arquivo
		line.clear();
		getline(fileElements, line);

		// Se encontrar uma linha em branco, seja no meio ou no fim do arquivo, continua a na próxima linha
		// Até encontrar o fim do arquivo.
		if(!line.compare("")) {
			continue;
		}

		//transforma a string num fluxo de dados (stream)
		stringstream str(line);
		word.clear();
		int j = 0;
		while(getline(str, word, SEPARATOR)) {
			// Converte apenas os atributos numericos que estã nos índices 1,2,3,4;
			if(j > 0 && j <= p) {
				// Ao copiar para o novo vetor e descartar o ID, o dados vão de line[j] -> attributes[j-1].
				attributes[j-1] = stod(word);
			}
			// PAttributes(p)+1 é o índice onde está a classe
			else if(j == p+1) {
				// DEPRECATED HARDCODE: Convertendo a classe de string para float
				//attributes.push_back(word.compare("Iris-setosa") == 0 ? 0 : (word.compare("Iris-versicolor") == 0 ? 1 : 2));


				// SOFTCODE:
				// Adiciona todos os valores de classes a um conjunto, desta forma
				// será possível tê-los associados à um índice
				// setClasses.insert(word);
				// Obtem o índice equivalente posição que a classe foi inserida na estrutura set
				// unsigned int index = std::distance(setClasses.begin(), setClasses.find(word));

				// Obtem o índice equivalente posição que a classe foi inserida na estrutura set
				unsigned int index = findStringInVector(word, problemClasses);

				// Se não encontra a classe no vetor, então ocorreu um erro
				unsigned int last = problemClasses.size()-1;
				if(index < 0 || index > last) {
					cerr << "Classe: " << word << endl;
					cerr << "Indice da classe: " << index << endl;
					cerr << "Houve um erro ao tentar encontrar a classe no vetor: findStringInVector(word, problemClasses)" << endl;
					break;
				}

				// Associa o elemento à classe representada aqui equivalentemente pelo seu índice
				attributes.push_back(index);
			}
			// O else abaixo vai funcionar porque vai se executado quando i = 0 e
			// não será executado quando i > p+1 porque p+1 é o índice das classes do dataset
			else {
				// Armazenando o índice do elemento no dataset depois do 4º atributo (Sim, é gambiarra)
				attributes[p] = (i+1);
			}

			word.clear();
			j++;
		}


		// Colocando os atributos no índice do elemento
		x.push_back(attributes);

		// Contando a quantidade de elementos inseridos
		n++;
	}

	// FIM DA LEITURA DO ARQUIVO CSV COM ESPAÇAMENTO DEPOIS DA VÍRGULA

	// Apresentando quantas classes foram encontradas para o problema
	if(DISPLAY_OUTS && DEBUG) { cout << "Num. de classes encontradas:\t" << setClasses.size() << endl; }

	if(DISPLAY_OUTS && DEBUG) { cout << "n:\t" << n << endl; }

	if(DISPLAY_OUTS && DEBUG) { cout << "Matriz de dados original" << endl; }
	for(unsigned int i = 0; i < x.size(); i++) {
		if(DISPLAY_OUTS && DEBUG) { cout << i << "\t"; }
		for(unsigned int j = 0; j < x[i].size(); j++) {
			if(DISPLAY_OUTS && DEBUG) { cout << x[i][j] << "\t"; }
		}
		if(DISPLAY_OUTS && DEBUG) { cout << endl; }
	}
	// FIM DA LEITURA DO ARQUIVO CSV SEM ESPAÇAMENTO DEPOIS DA VÍRGULA



/*
	// ### Normalizando os dados ###
	float menorValorColuna = 0;
	float maiorValorColuna = 0;
	float amplitudeColuna = 0;
	float xNormalizado[n][p+2];
	// int	   somaClasses[K];

	// Para cada coluna:
	for(int j = 0; j < p; j++) {
		// Inicializando variáveis
		menorValorColuna = 0;
		maiorValorColuna = 0;

		// Para cada linha:
		// O primeiro valor é inicialmente o maior e o menor valor da coluna
		menorValorColuna = x[0][j];
		maiorValorColuna = x[0][j];

		for(int i = 0; i < n; i ++) {
			// Obter o menor valor
			if(x[i][j] < menorValorColuna) { menorValorColuna =  x[i][j]; }
			// Obter o maior valor
			if(x[i][j] > maiorValorColuna) { maiorValorColuna =  x[i][j]; }
		}

		// Calcular a amplitude
		amplitudeColuna = maiorValorColuna - menorValorColuna;
		// Normalizar a linha i calculando (xi - médiaDeX) / ampliturdeDeX
		for(int i = 0; i < n; i++){
			xNormalizado[i][j] = (x[i][j] - menorValorColuna) / amplitudeColuna;
		}
	}


	// Imprimindo os valores normalizados
	if(DISPLAY_OUTS && DEBUG) {
		cout << "--- Valores ---" << endl;
		std::cout << std::fixed;
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < p; j++) {
				//x[i][j] = xNormalizado[i][j];
				cout << std::setprecision(16) << x[i][j] << "\t";
			}
			cout << std::setprecision(1) << x[i][p] << "\t";
			cout << endl;
		}
	}
*/

	return x;
}

/**
 * Inserir os elementos na árvore
 */
void insertElement(Node* root, vector<float> element) {
	// Apenas para evitar falha de seguimentação
	if(root == NULL) { return; }

	// Verificar se o nó corrente é folha ou galho
	bool isBranch = root->dataset->majoritaryClass == -1;
	// Se for folha,
	if(!isBranch) {
		//adicionar o elemento ao nó
		root->dataset->dataset.push_back(element);
		return;
	}

	// Se for galho e...
	// Se o atributo correspondente ao atributo avaliado pelo nó
	// for menor que o valor limiar do nó

	double difference = abs(element[root->dataset->splitAttribute] - root->dataset->splitValue);
	string comparator = element[root->dataset->splitAttribute] < root->dataset->splitValue ? " < " : " >= ";
	if(0.0 < difference && difference < 0.000000000000001) {
		if(DEBUG) {
			cout << std::fixed << std::setprecision(32)
				<< "[" << root->dataset->splitAttribute << "] " << element[root->dataset->splitAttribute]
				<< comparator << root->dataset->splitValue << "\t Diff. " << difference << endl;
			cout << "epsilon: " << std::numeric_limits<double>::epsilon()
				<< " == " << 2*(abs(element[root->dataset->splitAttribute] - root->dataset->splitValue)) << endl;//0.0000000000000001
			cout << "Diff. Ajustada. " << (difference - std::numeric_limits<double>::epsilon()/2) << endl;
		}
	}

	if(element[root->dataset->splitAttribute] < root->dataset->splitValue) {

		// Enviar o elemento para o nó filho esquerdo
		insertElement(root->left, element);
		return;
	}

	if (root->dataset->splitValue == 0) {
		cout << "element[" << root->dataset->splitAttribute << "] ->\t" <<  element[root->dataset->splitAttribute] << " >= " << root->dataset->splitValue << endl;
	}

	// Senão e....
	// Enviar o elemento para o nó filho direito.
	insertElement(root->right, element);
}

void calcImpurity(Node* root) {
	// Se árvore vazia, não faz nada
	if(root == NULL) { return; }

	// Se for nó folha, calcula a impureza
	if(root->dataset->majoritaryClass != -1) {
		root->dataset->calcImpurity();
		return;
	}

	// Se não for nó folha, prossegue para o filho esquerdo
	calcImpurity(root->left);

	// Se não for nó folha, prossegue para o filho direito
	calcImpurity(root->right);
}


void listarElementosClassificados(Node* root, vector<int> &elements, int iNode = 0) {
	// Se a árvore está vazia, não há o que imprimir
	if(root == NULL) {
		if(DISPLAY_OUTS && DEBUG) { cout << "A árvore está vazia." << endl << endl; }
		return;
	}

	if(DISPLAY_OUTS && DEBUG) { cout << endl << "Node: " << iNode << endl; }

	// Se é um nó folha, copiar os elementos que está no nó para o vetor 'elements'
	if(root->left == NULL && root->right == NULL) {

		// Salva em 'elements' a classe na qual o elemento foi classificado no respectivo índice do elemento
		for(unsigned int i = 0; i < root->dataset->dataset.size(); i++) {
			// Os índices armazenados em root->dataset->dataset[i][Problem::PAttributes] começam em 1
			// Então eles precisam ser subtraídos em 1 para corresponder ao respectivo índice em C++
			unsigned int _index = (int) (root->dataset->dataset[i][Problem::PAttributes]-1);
			if(DISPLAY_OUTS && DEBUG) { cout << "elements[" << _index << "] = " << root->dataset->majoritaryClass << ";" << endl; }
			elements[_index] = root->dataset->majoritaryClass;
		}
		if(DISPLAY_OUTS && DEBUG) { cout << "Numero de elementos: " << root->dataset->dataset.size() << endl; }
		for(unsigned int i = 0; i < root->dataset->dataset.size() && i < 5; i++) {
			vector<float>line = root->dataset->dataset[i];
			for(unsigned int j = 0; j < line.size(); j++) {
				if(DISPLAY_OUTS && DEBUG) { cout << line[j] << "\t"; }
			}
			if(DISPLAY_OUTS && DEBUG) { cout << endl; }
		}
		return;
	}

	// Se não é nó folha, faz o passo recursivo para ambos os filhos do nó
	listarElementosClassificados(root->left, elements, getLeftChild(iNode));
	listarElementosClassificados(root->right, elements, getRightChild(iNode));
}


void printTreeInLevels(Node* root, int index = 0) {
	if(root == NULL) {
		return;
	}

	if(DISPLAY_OUTS && DEBUG && index == 0) {
		cout << endl << endl << endl;
	}

	int level = getLevelFromIndex(index);
	if(DISPLAY_OUTS && DEBUG) {
		cout << getTabs(level) << "Node " << index;
		//root->printContentForOverleafFormat();
		cout << endl;
	}

	printTreeInLevels(root->left, getLeftChild(index));
	printTreeInLevels(root->right, getRightChild(index));
}



void summary(Node* root, int index = 0) {
	/* Format
	TipoNoh #
	classe 0:	qtd
	classe 1:	qtd
	...
	classe: K:	qtd
	 */

	if(root == NULL) {
		return;
	}

	if(DISPLAY_OUTS && index == 0) {
		cout << endl << endl << endl;
	}

	int level = getLevelFromIndex(index);
	string nodeType = (root->left == NULL && root->right == NULL) ? "Folha" : "Galho";

	if(DISPLAY_OUTS) {
		cout << nodeType << " " << index << endl;

		if(nodeType == "Folha") {
			vector<unsigned int> numberElementsByClass;
			unsigned int acertos = 0;
			unsigned int erros = 0;
			numberElementsByClass.resize(Problem::JClasses);

			// Salva em 'elements' a classe na qual o elemento foi classificado no respectivo índice do elemento
			for(unsigned int i = 0; i < root->dataset->dataset.size(); i++) {
				vector<float> line = root->dataset->dataset[i];
				int _class = line[Problem::PAttributes+1];
				numberElementsByClass[_class]++;

				// Verifica se a classe do elemento é a mesma do nó folha que ele está inserido
				if(_class == root->dataset->majoritaryClass) {
					acertos++;
				}
			}
			erros = root->dataset->dataset.size()-acertos;

			cout << "Elementos: " << root->dataset->dataset.size()
					<< "\tRótulo: " << root->dataset->majoritaryClass
					<< "\tImpureza: " << std::setprecision(16) << root->dataset->impurity <<  endl;

			float acertosPercent = float(acertos)/root->dataset->dataset.size()*100;
			float errosPercent = float(erros)/root->dataset->dataset.size()*100;

			cout << std::fixed;
			cout << "Acertos: " << acertos << "\t(" << std::setprecision(2) << acertosPercent << "%)" << endl;
			cout << "Erros: " << erros << "\t(" << std::setprecision(2) << errosPercent << "%)" << endl;
			cout << "-----------" << endl;

			for(unsigned int _class = 0; _class < numberElementsByClass.size(); _class++) {
				cout << "Classe " << _class << ":  " << numberElementsByClass[_class] << endl;
			}

		}
		else {
			cout << "Atributo[" << root->dataset->splitAttribute << "] = " << std::setprecision(17) << root->dataset->splitValue << endl;
		}
		cout << endl << endl;
	}

	summary(root->left, getLeftChild(index));
	summary(root->right, getRightChild(index));

}




bool createDirectory(const string& directoryPath) {
    if (!fs::exists(directoryPath)) {
        if (fs::create_directory(directoryPath)) {
            if(DEBUG) { std::cout << "Directory created: " << directoryPath << std::endl; }
            return true;
        } else {
            std::cerr << "Failed to create directory: " << directoryPath << std::endl;
            return false;
        }
    }
    return true;  // Directory already exists
}

bool createFile(const string& filePath, const string& fileContent) {
    std::ofstream outputFile(filePath, std::ios::app);
    if (outputFile.is_open()) {
        outputFile << fileContent;
        outputFile.close();
        if(DEBUG) { std::cout << "File created: " << filePath << std::endl; }
        return true;
    } else {
        std::cerr << "Failed to create file: " << filePath << std::endl;
        return false;
    }
}





/**
 * Espera-se os seguintes argumentos:
 * int total de elementos
 * int total de atributos
 * int total de classes
 * endereço completo do arquivo do dataset
 */
int main(int argc, char** argv) {

	ostringstream results;


	// Calculando o tempo gasto
	auto startTime = std::chrono::high_resolution_clock::now();

	if(argc > 1) {
		DISPLAY_OUTS = atoi(argv[1]);
	}

	if(DISPLAY_OUTS) {
		if(argc == 1) {
			cout << "Você também pode passar os seguintes parametros:" << endl;
			cout << "[1] Mostrar Saídas (padrão: 1 - Sim, 0 - Não)" << endl;
			cout << "[2] Arquivo do dataset (padrão: iris.norm.csv)" << endl;
			cout << "[3] Qtd. de atributos (padrão: 4)" << endl;
			cout << "[4] Qtd. de classes (padrão: 3)" << endl;
			cout << "[5] Classes separadas por vírgula ('0,1,2')" << endl;
			cout << "[6] Arquivo de config. da árvore (tree-settings.txt)" << endl;
			cout << "[7] ID da execução" << endl;
			cout << endl;
			cout << "Deseja continuar mesmo assim? [S ou n]: ";
			char continuar;
			cin >> continuar;
			if(continuar != 's' && continuar != 'S') {
				return 0;
			}
		}

		if(DEBUG) {
			cout << "Argumentos do programa:" << endl;
			for(int i = 0; i < argc; i++) {
				cout << i << ": " << argv[i] << endl;
			}
			cout << endl;
		}
	}

	// Recebendo atributos por parâmetro //

	if(DISPLAY_OUTS && DEBUG) { cout << "1 - Mostrar Saídas:\t" << (DISPLAY_OUTS ? "Sim" : "Não") << endl; }

	// Recebe o nome do arquivo de classificação por parâmetro quando é passado
	fileElementsName = (argc > 2) ? string(argv[2]) : fileElementsName;
	if(DISPLAY_OUTS && DEBUG) { cout << "2 - Dataset:\t\t\t" << fileElementsName << endl; }

	// Recebe a quantidade de parâmetros do dataset quando é passado
	int p;
	p = (argc > 3) ? stoi(argv[3]) : 4;
	if(DISPLAY_OUTS && DEBUG) { cout << "3 - N. parametros:\t\t" << p << endl; }

	// Recebe a quantidade de classes do dataset quando é passado
	int K;
	K = (argc > 4) ? stoi(argv[4]) : 3;
	if(DISPLAY_OUTS && DEBUG) { cout << "4 - N. classes:\t\t\t" << K << endl; }

	// Recebe a lista de classes possíveis para o problema
	string classesInLine = (argc > 5) ? string(argv[5]) : "0,1,2";
	if(DISPLAY_OUTS && DEBUG) { cout << "5 - Classes:\t\t\t" << classesInLine << endl; }

	// Recebe o nome do arquivo de configuração por parâmetro quando é passado
	fileTreeName = (argc > 6) ? string(argv[6]) : string(fileTreeName);
	if(DISPLAY_OUTS && DEBUG) { cout << "6 - Arquivo config. árvore:\t" << fileTreeName << endl; }

	// Pega o ID da execução para abrir o arquivo onde depositar alguns resultados
	unsigned int idExec = (argc > 7) ? atoi(argv[7]) : 0;
	if(DISPLAY_OUTS && DEBUG) { cout << "7 - ID da execução:\t\t" << idExec << endl; }

	string debugParams = results.str();
	if(DISPLAY_OUTS && DEBUG) { cout << debugParams; }

	// Criando um dicionário de classes
	stringstream _lineClasses(classesInLine);
	string _class;
	while(getline(_lineClasses, _class, ',')) {
		problemClasses.push_back(_class);
		_class.clear();
	}
	// Imprimindo o dicionário de classes
	if(DISPLAY_OUTS && DEBUG) {
		for(unsigned int i = 0; i < problemClasses.size(); i++) {
			cout << problemClasses[i] << ",\t";
		}
		cout << endl;
	}

	// CARREGANDO DADOS
	vector<vector<float>> x;
	fstream arquivo;

	fileTree.open(fileTreeName, fstream::in);
	// Carrega a árvore de classificação à partir do arquivo
	if(DISPLAY_OUTS) { cout << endl; }
	Node* root = readNode();
	if(DISPLAY_OUTS) { cout << endl << endl; }
	// Imprime as características de cada nó
	//root->print();

	// Imprime a string para respresentação em latex
	//root->draw();
	if(DISPLAY_OUTS) { cout << endl; }


	// Carregar os elementos
	elements = loadElements(fileElementsName, p, K);

	// Inserir os elementos na árvore
	if(DISPLAY_OUTS && DEBUG) { cout << "elements:" << endl; }
	for(unsigned int i = 0; i < elements.size(); i++) {
		if(DISPLAY_OUTS && DEBUG) { cout << "inseriu " << i+1 << "\t"; }
		for(unsigned int j = 0; j < elements[i].size(); j++) {
			if(DISPLAY_OUTS && DEBUG) { cout << setprecision(8) << elements[i][j] << "\t"; }
		}
		if(DISPLAY_OUTS && DEBUG) { cout << endl; }
		insertElement(root, elements[i]);
	}


	// Inicializiar os atributos do problema
	if(DISPLAY_OUTS && DEBUG) { cout << "Atributos: " << p << "\tClasses: " << K << endl; }
	Problem::init(elements, p, K);
	Problem::print();


	if(DISPLAY_OUTS && DEBUG) { cout << endl << endl; }
	// Calcular a impureza da árvore
	// Isso também calcula quantos elementos de cada classe há em cada nó folha
	calcImpurity(root);

	if(DISPLAY_OUTS && DEBUG) { cout << endl << endl; }
	// Imprimir a árvore
	//root->print(1, "", 0, true);
	//root->print();

	results << endl << endl;
	root->draw(results);
	root->toLatexTikzpicture(results);
	results << endl << endl;

	vector<int> classificados(Problem::numberOfElemetsInProblem);
	listarElementosClassificados(root, classificados);
	if(DISPLAY_OUTS && DEBUG) { cout << endl; }

	if(DISPLAY_OUTS && DEBUG) { cout << "Elementos listados na variavel 'classificados' (" << &classificados << "): " << classificados.size() << " elementos" << endl; }


	// Salvar o arquivo de predições (predict.csv)
	if(DISPLAY_OUTS && DEBUG) { cout << "Abriu o arquivo " << filePredictName << "?: " << (filePredict.is_open() ? "Sim" : "Não") << endl; }
	filePredict.open(filePredictName, fstream::out);
	if(DISPLAY_OUTS && DEBUG) { cout << "Abriu o arquivo " << filePredictName << "?: " << (filePredict.is_open() ? "Sim" : "Não") << endl; }


	// Printando taxas de acertos
	int numAcertos = 0;
	int numErros = 0;
	for(unsigned int i = 0; i < classificados.size(); i++) {
		// Problem::PAttributes+1 é o índice onde está a classe na qual o elemento foi classificado
		//if(DISPLAY_OUTS && DEBUG) { cout << "[" << i+1 << "]\tclasse: " << classificados[i] << endl; }
		filePredict << classificados[i] << endl;

		//if(DISPLAY_OUTS && DEBUG) { cout << "(int)elements[i][p]: " << (int)elements[i][p+1] << " | " << "classificados[i]: " << classificados[i] << endl; }
		if((int)elements[i][p+1] == classificados[i]) {
			numAcertos++;
		}
		else {
			numErros++;
		}
	}

	if(int(numAcertos + numErros) != classificados.size()) {
		if(DISPLAY_OUTS && DEBUG) { cout << "numAcertos+numErros != classificados.size()" << endl; }
	}

	if(DISPLAY_OUTS && DEBUG) { cout << "Acertos: " << numAcertos << "\t" << ((float)numAcertos/(float)classificados.size())*100 << "%" << endl; }
	if(DISPLAY_OUTS && DEBUG) { cout << "Erros: " << numErros << "\t" << ((float)numErros/(float)classificados.size())*100 << "%" << endl; }



	if(DISPLAY_OUTS && DEBUG) { cout << endl << endl; }
	root->preOrderComIndices(results);
	if(DISPLAY_OUTS && DEBUG) {
		printTreeInLevels(root);
	}

	if(DISPLAY_OUTS && DEBUG) {
		cout << endl << endl;
		cout << "Summary: " << endl;
		summary(root);
		cout << endl << endl;
	}





	// DADOS DA EXECUÇÃO COMPLETA
	auto endTime = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	// Extract hours, minutes, seconds, and remaining milliseconds
	auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
	duration -= hours;

	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
	duration -= minutes;

	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	duration -= seconds;

	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

	int horas = int(hours.count());
	int minutos = int(minutes.count());
	int segundos = int(seconds.count());
	int milisegundos = int(duration.count());
	string spentTimeStr = to_string(horas)+"h"+to_string(minutos)+"min"+to_string(segundos)+"s"+to_string(milisegundos)+"ms";



	int nTotal = elements.size();
	int nAcertos = root->nAcertos();
	int nErros = nTotal-root->nAcertos();



	// Resultados
	results << endl << "=== PREDICT =========================================" << endl << endl;

	results << "N. elementos teste:\t" << nTotal << endl;
	results << "N. galhos:\t\t" << root->getBranches().size() << endl;
	results << "N. folhas:\t\t" << root->getLeafs().size() << endl;
	results << "Altura real:\t\t" << root->height() << endl;
	results << std::fixed << setprecision(3);
	results << "Total acertos:\t\t" << nAcertos << "\t(" << ((float) nAcertos/(float)nTotal)*100 << ")%" << endl;
	results << "Total erros:\t\t" << nErros << "\t(" << ((float)nErros/(float)nTotal)*100 << ")%" << endl;
	results << "Tempo gasto:\t\t" << spentTimeStr << endl;

	results << endl << "---------------------------------------" << endl << endl;

	results << "Parametros de entrada:" << endl;
	results << "CSV:\t\t\t" << fileElementsName << endl;
	results << "N. elementos:\t\t" << nTotal << endl;
	results << "Atributos:\t\t" << p << endl;
	results << "Classes:\t\t" << K << " (" << classesInLine << ")" << endl;
	results << "Arq. config. árvore:\t" << fileTreeName << endl;
	results << "ID execucao:\t\t" << idExec << endl;

	results << endl << "=====================================================" << endl << endl;


	// Salvando saídas no arquivo
	string directoryPath = "outputs";
	string outputsFile = to_string(idExec)
			+"_n"+to_string(x.size())
			+"_p"+to_string(p)
			+"_K"+to_string(K)
			+"_h"+to_string(root->height())
			+".txt";

	// Check and create the directory
	if (createDirectory(directoryPath)) {
		string filePath = directoryPath + "_" + outputsFile;
		string fileContent = results.str();
		// Create the file
		createFile(filePath, fileContent);
	}


	if(DISPLAY_OUTS) {
		cout << results.str() << endl;
	}



	if(DISPLAY_OUTS && DEBUG) { cout << endl; }
	if(DISPLAY_OUTS && DEBUG) { cout << "terminou" << endl; }

	filePredict.close();
	fileElements.close();
	fileTree.close();

	return 1;
}



















