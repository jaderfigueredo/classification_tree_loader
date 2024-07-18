/*
 * No.cpp
 *
 *  Created on: 23 de jan. de 2022
 *      Author: jader
 */
#include "Node.h"
#include "Problem.h"

extern int DISPLAY_OUTS;
extern int DEBUG;
#define BRANCH "galho"
#define LEAF "folha"

extern vector<string> problemClasses;


// Retorna o índice do filho esquerdo
extern int getLeftChild(int t);
// Retorna o índice do filho direito
extern int getRightChild(int t);

extern string getTabs(int n = 1, string _char = "\t");
extern int getLevelFromIndex(int index);

using namespace std;

// Construtor
Node::Node(Dataset* dataset) {
	this->left = NULL;
	this->right = NULL;
	this->dataset = dataset;
}

// Destrutor
Node::~Node() {
	delete this->dataset;
	delete this->left;
	delete this->right;
}


unsigned int Node::print(unsigned int i, string side, unsigned int parent, bool imprimirElementos, int hight) {

	if(DISPLAY_OUTS) {
		// Imprimir os dados do nó atual
		cout << endl;
		cout << "Node: " << i << "\t\tFilho " << side << " do " << (parent == 0 ? "--" : to_string(parent));
		if(this->dataset == NULL) {
			cout << " não possui dataset." << endl;
			return i;
		}
	}

	// Calcular a altura da árvore
	// A altura só aumenta, mas ela não pode ser acumulativa
	if(hight > Problem::hight) {
		Problem::hight = hight;
	}

	// Se for para imprimir os elementos, mas este não for um nó folha, então não tentar imprimir seus elementos
	bool imprimirElementosDaFolha = (imprimirElementos && this->left == NULL && this->right == NULL);

	// Imprimir os detalhes do nó.
	this->dataset->print("", false, imprimirElementosDaFolha);

	vector<Dataset*> datasets(2);
	datasets[0] = this->left != NULL ? this->left->dataset : NULL;
	datasets[1] = this->right != NULL ? this->right->dataset : NULL;
	string tab = (this->left == NULL && this->right == NULL) ? "\t" : "";
	if(DISPLAY_OUTS) { cout << endl; }
	if(DISPLAY_OUTS) { cout << "Left:  " << this->left << tab << "\tdataset: " << datasets[0] << endl; }
	if(DISPLAY_OUTS) { cout << "Right: " << this->right << tab << "\tdataset: " << datasets[1]  << endl; }

	parent = i;

	// Imprimir os dados do nós filho esquerdo
	if(this->left != NULL) {
		i = this->left->print(++i, "esquerdo", parent, imprimirElementos, ++hight);
		Problem::hight--;
	}

	// Imprimir os dados do nós filho direito
	if(this->right != NULL) {
		i = this->right->print(++i, "direito", parent, imprimirElementos, ++hight);
		Problem::hight--;
	}

	if(this->left == NULL && this->right == NULL) {
		Problem::numberOfLeafs++;
	}
	else {
		Problem::numberOfBranches++;
	}

	if(DISPLAY_OUTS) { cout << endl; }
	return i;
}

void Node::toSplit() {
	// Se for um nó vazio, não há o que fazer
	if(this->dataset == NULL) {
		return;
	}

	vector<Dataset*> subsets = this->dataset->findBetterSplitProfessor();

	this->left = (subsets[0] != NULL) ? new Node(subsets[0]) : NULL;

	this->right = (subsets[1] != NULL) ? new Node(subsets[1]) : NULL;
}

void Node::toSplitAll() {
	this->toSplit();

	if(this->left != NULL) {
		this->left->toSplitAll();
	}

	if(this->right != NULL) {
		this->right->toSplitAll();
	}
}


void Node::draw(ostringstream &results, unsigned int tab, int index) {

	//Imprimindo as tabulações
	string concat = "";
	for(unsigned int i = 0; i < tab; i++) {
		concat += "   ";
	}

	bool isBranch = (this->dataset->majoritaryClass == -1);

	results << concat;
	results << "[.\\node[draw]{" << index << " ";
	if(!isBranch) {
		results << "Folha" << "\\\\" << '\t';
		results << "Elementos: " << this->dataset->dataset.size() << "\\\\" << endl;
		for(unsigned int j = 0; j < this->dataset->countElementsClasses.size()/*Problem::JClasses*/; j++) {
			results << concat << "cl[" << j << "] = " << this->dataset->countElementsClasses[j] << "\\\\" << endl;
		}
		results << concat;
		results << "Impur. " << std::setprecision(5) << this->dataset->impurity << "\\\\" << '\t';
		results << "Rotulo: " << this->dataset->majoritaryClass;
	}
	else {
		results << "Galho" << "\\\\" << '\t';
		results << "Atrib.:" << this->dataset->splitAttribute << "\\\\" << '\t';
		results << "Separ.: \\\\" << std::fixed << std::setprecision(6) << this->dataset->splitValue << "...";
	}
	results << "\t};";


	tab++;
	bool flagHasChild = false;

	if(this->left != NULL) {
		results << endl;
		this->left->draw(results, tab, getLeftChild(index));
		flagHasChild = true;
	}

	if(this->right != NULL) {
		results << endl;
		this->right->draw(results, tab, getRightChild(index));
		flagHasChild = true;
	}

	if(flagHasChild) {
		results << endl << concat << "]";
	}
	else {
		results << " ]";
	}

	// Se for o fim da execução (nó raiz), quebrar mais uma linha
	if(tab == 1) {
		results << endl;
	}
}

void Node::toLatexTikzpicture(ostringstream &results, unsigned int tab, int index, string branchNodeShape) {

	//Imprimindo as tabulações
	string concat = "";
	for(unsigned int i = 0; i < tab; i++) {
		concat += "\t";
	}

	bool isBranch = (this->dataset->majoritaryClass == -1);

	string nodeShape = isBranch ? branchNodeShape : "rect";

	//results << concat;
	// Is Root?
	if(index == 0) {
		results << "\\begin{tikzpicture}" << endl;
		results << concat << "\\node[" << nodeShape << "] (root) ";
	}
	else {
		results << concat << "child {node[" << nodeShape << "] ";
	}

	if(isBranch) {
		results << "{p[" << this->dataset->splitAttribute << "] < ";
		results << std::fixed << std::setprecision(6) << this->dataset->splitValue << "}";
	}
	else {
		results << "{k[" << this->dataset->majoritaryClass << "]}";
	}


	tab++;
	bool flagHasChild = false;
	string sentidoDirecao = (index % 2 == 1) ? "left" : "right";
	char atendeCondicao = (index % 2 == 1) ? 'S' : 'N';

	if(this->left != NULL) {
		results << endl;
		this->left->toLatexTikzpicture(results, tab, getLeftChild(index));
		flagHasChild = true;
	}

	if(this->right != NULL) {
		results << endl;
		this->right->toLatexTikzpicture(results, tab, getRightChild(index));
		flagHasChild = true;
	}

	if(flagHasChild) {
		// tab == 1 significa que é o primeiro nó da árvore, então:
		// aplica-se a definição de aresta e legenda de aresta à todos os nós que não seja o nó raiz
		if(tab != 1) {
			results << endl << concat << "edge from parent node[" << sentidoDirecao << "] {" << atendeCondicao << "}}";
		}
		else { // Aplica-se o fechamento simples apenas ao nó raiz, ou seja, não há arestas saindo do nó raiz.
			results << ";";
		}
	}
	else {
		results << " edge from parent node[" << sentidoDirecao << "] {" << atendeCondicao << "}}";
	}

	// Se for o fim da execução (nó raiz), quebrar mais uma linha
	if(tab == 1) {
		results << endl << "\\end{tikzpicture}" << endl;
		results << endl;
	}
}


void Node::printStatistics(unsigned int i) {
	if(this->dataset == NULL) {
		return;
	}

	string label = "Node " + std::to_string(i);
	this->dataset->printStatistics(label);

	if(this->left != NULL) {
		this->left->printStatistics(++i);
	}

	if(this->right != NULL) {
		this->right->printStatistics(++i);
	}
}

void Node::preOrderComIndices(ostringstream &results, int index) {
	string separator = " | ";
	string tabs = getTabs(getLevelFromIndex(index), "  ");

	// Imprime o identificador de início de nó e o seu índice

	results << endl << tabs << "( ";


	// Verifica e imprime o tipo do nó
	if(this->left == NULL || this->right == NULL) {
		results << LEAF << " " << index;
		// Classe
		results << separator << "class: " << this->dataset->majoritaryClass;

		int nTotal = this->dataset->dataset.size();
		int nAcertos = this->nAcertos();
		int nErros = nTotal - this->nAcertos();

		// N. Elementos
		results << separator  << nTotal << " elementos";
		results << std::fixed << std::setprecision(2);
		// Acertos	// Porcent.
		results << separator << "Acertos: " << (float)nAcertos / (float) nTotal * 100 << "%";
		// Erros	// Porcent.
		results << separator << "Erros: " << (float)nErros / (float) nTotal * 100 << "%";
		results	<< separator << "Impur: " << std::setprecision(7) << this->dataset->impurity << ")";

		// Classes
		results << std::fixed << std::setprecision(2);
		for(unsigned int j = 0; j < this->dataset->countElementsClasses.size()/*Problem::JClasses*/; j++) {
			int nElementosClasse = this->dataset->countElementsClasses[j];
			float percElementos = ((float) nElementosClasse / (float) nTotal) * 100;
			results << endl << tabs << " " << "Classe [" << j << "] = " << nElementosClasse << " elementos\t(" << percElementos << "%)";
		}
	}
	else {
		results << BRANCH << " " << index;
		results << separator << "feat: " << this->dataset->splitAttribute;
		results << separator << "Threshold: " << std::fixed << setprecision(16) << this->dataset->splitValue;

		// Passos recursivos: Chamar a impressão do filho esquerdo e filho direito
		this->left->preOrderComIndices(results, getLeftChild(index));
		this->right->preOrderComIndices(results, getRightChild(index));
		results << endl << tabs << ")";
	}

	// Imprime o caracter de fechamento
	//string closer = isLeaf ? ")" : ;
}

void Node::printContentForOverleafFormat() {
	if(DISPLAY_OUTS) {
		cout << "[.{";
		for(unsigned int j = 0; j < Problem::JClasses; j++) {
			cout << this->dataset->countElementsClasses[j] << "_" << problemClasses[j] << " | ";
		}
		cout << "Impur:" << std::setprecision(7) << this->dataset->impurity;
		cout << "}";
	}
}


vector<Node*> Node::getBranches() {
	vector<Node*> branches;

	if(this->left == NULL && this->right == NULL) {
		return branches;
	}

	if(this->left != NULL) {
		vector<Node*> branchesL = this->left->getBranches();
		branches.insert( branches.end(), branchesL.begin(), branchesL.end() );
	}

	branches.push_back(this);

	if(this->right != NULL) {
		vector<Node*> branchesR = this->right->getBranches();
		branches.insert( branches.end(), branchesR.begin(), branchesR.end() );
	}

	return branches;
}


vector<Node*> Node::getLeafs() {
	vector<Node*> leafs;

	if(this->left == NULL && this->right == NULL) {
		leafs.push_back(this);
		return leafs;
	}

	if(this->left != NULL) {
		vector<Node*> leafsL = this->left->getLeafs();
		leafs.insert( leafs.end(), leafsL.begin(), leafsL.end() );
	}

	if(this->right != NULL) {
		vector<Node*> leafsR = this->right->getLeafs();
		leafs.insert( leafs.end(), leafsR.begin(), leafsR.end() );
	}

	return leafs;
}

int Node::height() {
	if(this->left == NULL && this->right == NULL) {
		return 0;
	}

	int height = 1;
	int heightL = 0;
	int heightR = 0;

	if(this->left != NULL) {
		heightL = this->left->height();
	}

	if(this->right != NULL) {
		heightR = this->right->height();
	}

	int maior = (heightL > heightR) ? heightL : heightR;

	return height+maior;
}

int Node::nAcertos() {
	int nAcertos = this->dataset->nAcertos();

	if(this->left != NULL) {
		nAcertos += this->left->nAcertos();
	}

	if(this->right != NULL) {
		nAcertos += this->right->nAcertos();
	}

	return nAcertos;
}

