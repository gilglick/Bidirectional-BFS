#include "Node.h"

Node::Node(int r, int c, Node* pr)
{
    row = r;
    col = c;
    parent = pr;
}

int Node::GetRow()
{
    return row;
}

int Node::GetCol()
{
    return col;
}

Node* Node::GetParent()
{
    return parent;
}
