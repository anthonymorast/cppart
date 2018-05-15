#include "print_tree.h"

void printTree(node * root, string filename)
{
	cout << "Creating tree file '" << filename << "'..." << endl;
	ofstream fout;
	fout.open(filename);

	fout << "node) split, number observations, deviance, yval\n* denotes a terminal node\n\n";
	fout << "1) root " << root->numObs << " " << root->dev << " " << root->yval << endl;

	if(root->leftNode != NULL) {
		printSubTree(root->leftNode, 2, 1, root, false, fout);
	}
	if (root->rightNode!= NULL) {
		printSubTree(root->rightNode, 3, 1, root, true, fout);
	}
}

void printSubTree(node * n, int nodeId, int myDepth, node * parent, bool right, ofstream &fout)
{
	printNode(n, nodeId, myDepth, parent, right, fout);

	if (n->leftNode != NULL) {
		printSubTree(n->leftNode, (2 * nodeId), myDepth + 1, n, false, fout);
	}
	if (n->rightNode != NULL) {
		printSubTree(n->rightNode, (2 * nodeId) + 1, myDepth + 1, n, true, fout);
	}
}

void printNode(node * n, int nodeId, int myDepth, node * parent, bool right, ofstream &fout)
{
	string tabString = "";
	for (int i = 0; i < myDepth; i++) {
		tabString += "\t";
	}

	int direction = parent->direction;
	if (right) {
		direction *= -1;
	}

	string directionStr = "";
	if (direction < 0) {
		directionStr = "<";
	}
	else {
		directionStr = ">";
	}

	if (right) {
		directionStr += "=";
	}
	else {
		directionStr += " ";
	}

	string terminalStr = "";
	if (n->leftNode == NULL && n->rightNode == NULL) {
		terminalStr += "*";
	}

	fout << tabString << nodeId << ") " << parent->varIndex << directionStr << parent->splitPoint << " " <<
		n->numObs << "  " << n->dev << " " << n->yval << " " << terminalStr << endl;
}
