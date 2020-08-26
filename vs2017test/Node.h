#pragma once
class Node
{
private:
	int row, col;
	Node* parent;
public:
	Node(int r, int c, Node* pr);
	int GetRow();
	int GetCol();
	Node* GetParent();
};

