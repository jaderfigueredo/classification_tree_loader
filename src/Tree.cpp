/*
 * Tree.cpp
 *
 *  Created on: 1 de fev. de 2022
 *      Author: jader
 */

#include "Tree.h"

using namespace std;

// Construtores
Tree::Tree(){} // @suppress("Class members should be properly initialized")
Tree::Tree(Node* root) {
	this->root = root;
}
// Destrutor
Tree::~Tree() {
	delete this->root;
}

