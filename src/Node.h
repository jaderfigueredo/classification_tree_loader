/*
 * No.h
 *
 *  Created on: 23 de jan. de 2022
 *      Author: jader
 */

#ifndef NODE_H_
#define NODE_H_

#include <iostream>
#include <vector>
#include "Dataset.h"

using namespace std;


class Node {
public:
	Dataset* dataset;
	Node* left;
	Node* right;

	// Métodos construtores
	Node(Dataset* dataset = NULL);
	// Método destrutor
	virtual ~Node();

	void toSplit();

	void toSplitAll();

	bool setChildren(vector<Dataset*> datasets);

	unsigned int print(unsigned int i = 1, string side = "", unsigned int parent = 0, bool imprimirElementos = false, int hight = 0);

	void findBetterSplit();

	void draw(ostringstream &results, unsigned int tab = 0, int index = 0);

	void toLatexTikzpicture(ostringstream &results, unsigned int tab = 0, int index = 0, string branchNodeShape = "ellip");

	void printStatistics(unsigned int i = 0);

	void preOrderComIndices(ostringstream &results, int index = 0);

	void printContentForOverleafFormat();

	vector<Node*> getBranches();

	vector<Node*> getLeafs();

	int height();

	int nAcertos();
};

#endif /* NO_H_ */
