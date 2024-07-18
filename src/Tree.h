/*
 * Tree.cpp
 *
 *  Created on: 1 de fev. de 2022
 *      Author: jader
 */


#ifndef TREE_H_
#define TREE_H_

#include<iostream>
#include "Node.h"

using namespace std;

class Tree {
public:
	Node* root;
	// Construtores
	Tree();
	Tree(Node* root);
	// Método destrutor
	virtual ~Tree();
};

#endif /* TREE_H_ */

