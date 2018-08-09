#include "Node.h"

Node::Node(void)
{
}


Node::~Node(void)
{
}

Node::Node(int id, int fingTableSize)
{
	this->id = id;
	this->m = fingTableSize;
	chordSize = 1 << m;
	pred = NULL;

	for (unsigned int i = 0; i < m; i++)
	{
		Finger f;
		f.start = (id + (1<<i)) % chordSize;
		f.node = NULL;
		finger.push_back(f);
	}
}

Node* Node::findSucc(int id)
{
	if (inBtwnRightInclusive(id, pred->id, this->id))	//If this is the node responsible
		return this;

	//Finds the predecessor if the given id
	//Returns the successor
	return findPred(id)->finger.at(0).node;
}

Node* Node::findPred(unsigned int id)
{
	if (this->id == id)	//If this node is the one we are looking for
		return pred;
	
	if (pred == this)	//If this is the only node
		return this;

	Node *n = this;	//While the given id is not between a node and its successor
	while (!inBtwnRightInclusive(id, n->id, n->finger.at(0).node->id))
	{
		n = n->closestPredFinger(id);	//Traverse to the next appropriate node
	}
	return n;
}

Node* Node::closestPredFinger(int id)
{
	//Starting from the largest finger, we search until we reach a node
	//in between this node and the given id (largest possible hop)
	for (int i = m - 1; i >= 0; i--)
	{
		if (inBtwnExclusive(finger.at(i).node->id, this->id, id))
			return finger.at(i).node;
	}

	return this;
}

bool Node::inBtwnRightInclusive(int num, int left, int right)
{
	if (left < right)
	{
		if (left < num && num <= right)
			return true;
	}
	else if (left > right)
	{
		if (left < num || num <= right)
			return true;
	}
	else
	{
		if (num == right)
			return true;
	}
	return false;
}

bool Node::inBtwnExclusive(int num, int left, int right)
{
	if (left < right)
	{
		if (left < num && num < right)
			return true;
	}
	else if (left > right)
	{
		if (left < num || num < right)
			return true;
	}

	return false;
}

void Node::printFingerTable()
{
	cout<<"FINGER TABLE OF NODE "<<id<<":"<<endl;
	for (unsigned int i = 0; i < m; i++)
	{
		cout<<finger.at(i).node->id<<" ";
	}
	cout<<endl;
}

//A new peer joins the system
bool Node::join(Node* n)
{
	if (n != NULL)	//If node is not the only node
	{
		//Find the successor;
		Node* succ = n->findSucc(id);

		if (id == succ->id)	//If peer already exist
			return false;

		//Establish the successor and predecessor just like a doubly linked list
		finger.at(0).node = succ;
		pred = finger.at(0).node->pred;
		finger.at(0).node->pred = this;	//Updates the successor's predecessor
		pred->finger.at(0).node = this;	//Updates the predecessor's successor

		initFingerTable(n);				//Initializes its finger table
		if (!succ->data.empty())
			takeOverData(finger.at(0).node);//Takes over any data it is responsible for from its successor
		updateOthers();					//Updates other nodes' finger tables
	}
	else			//If node is the only node
	{
		for (unsigned int i = 0; i < m; i++)	//Initializes all fingers to itself
			finger.at(i).node = this;
		pred = this;				//Initializes its predecessor to itself
	}

	return true;
}

//Initializes all the fingers besides the first one (the successor)
void Node::initFingerTable(Node* n)
{
	for (unsigned int i = 0; i < m - 1; i++)
	{
		//If the start range of the next finger is less than the previous finger node
		if (inBtwnLeftInclusive(finger.at(i + 1).start, id, finger.at(i).node->id))
			finger.at(i + 1).node = finger.at(i).node;	//Use the same finger node
		else	//Or else we find its successor O(log n)
			finger.at(i + 1).node = n->findSucc(finger.at(i + 1).start);
	}
}

//Updates all other nodes with fingers possibly changed by its joining
void Node::updateOthers()
{
	unsigned int updateId;
	for (unsigned int i = 0; i < m; i++)	//For each finger
	{
		//Gets the id of the first position + 1 (travelling backwards)
		//where the finger might contain the new node should it be a node (peer)
		updateId = (id - (1<<i) + 1) % chordSize;

		//Gets the predecessor of the id
		//(the first node travelling backwards where the finger position might possibly be affected)
		findPred(updateId)->updateFingerTable(this, i);	//Checks if it needs updating and updates it if it needs
	}
}

//Checks a finger position (i) of the node if it needs updating because of a new node (s)
void Node::updateFingerTable(Node* s, int i)
{
	//If the new node id is between the start if the finger and the current node of the finger
	if (inBtwnLeftInclusive(s->id, finger.at(i).start, finger.at(i).node->id))
	{
		finger.at(i).node = s;
		pred->updateFingerTable(s, i);
	}
}

void Node::leave()
{
	if (!finger.at(0).node->data.empty())
		handOverData(finger.at(0).node);	//Hands over all data to its successor
	finger.at(0).node->pred = pred;		//Updates its successor's predecessor
	pred->finger.at(0).node = finger.at(0).node;	//Updates its predecessor's successor
	updateOthersDel();	//Updates all possible node's fingers that are affected
}

//Updates all other nodes with fingers possibly changed by its leaving
void Node::updateOthersDel()
{
	unsigned int updateId;
	for (unsigned int i = 0; i < m; i++)
	{
		//Gets the first position + 1 travelling backwards where its ith finger
		//is possibly the leaving node should it be a node
		updateId = (id - (1<<i) + 1) % chordSize;

		//Gets the predecessor of that id which is the first node travelling
		//backwards where it ith position might possibly contain the leaving node
		findPred(updateId)->updateFingerTableDel(this, i);	//Updates the ith finger of the node
	}
}

//Checks if the ith finger contains the leaving node
//If it does, changes the ith finger to the leaving node's predecessor
//Recursive call to the predecessor until the ith position no longer contains the leaving node
void Node::updateFingerTableDel(Node *s, int i)
{
	if (finger.at(i).node->id == s->id)	//If it contains the leaving node
	{
		finger.at(i).node = s->finger.at(0).node;	//Set node to leaving node's successor
		pred->updateFingerTableDel(s, i);	//Recursive call to its predecessor
	}
}

bool Node::inBtwnLeftInclusive(int num, int left, int right)
{
	if (left < right)
	{
		if (left <= num && num < right)
			return true;
	}
	else if (left > right)
	{
		if (left <= num || num < right)
			return true;
	}
	else
	{
		if (num == left)
			return true;
	}

	return false;
}

//Inserts data into the node
void Node::insertData(int key, string content)
{
	Data d;
	d.key = key;
	d.content = content;

	data.push_back(d);
}

//Takes over data from its successor
void Node::takeOverData(Node *succ)
{
	for (vector<Data>::iterator it = succ->data.begin(); it != succ->data.end(); it++)
	{
		if (inBtwnRightInclusive(it->key, pred->id, id))
		{
			data.push_back(*it);
			succ->data.erase(it);
		}
	}
}

//Hands over data to its successor
void Node::handOverData(Node *succ)
{
	for (vector<Data>::iterator it = data.begin(); it != data.end(); it++)
	{
		succ->data.push_back(*it);
		data.erase(it);
	}
}

//Same as findSucc but with printing
Node* Node::findSuccPrint(int key)
{
	if (inBtwnRightInclusive(key, pred->id, this->id))
		return this;

	Node *n = findPredPrint(key);
	cout<<">"<<n->finger.at(0).node->id<<endl;
	return n->finger.at(0).node;
}

//Same as findPred but with printing
Node* Node::findPredPrint(unsigned int id)
{
	if (this->id == id)
		return pred;

	if (pred == this)
		return this;

	Node *n = this;
	cout<<this->id;
	while (!inBtwnRightInclusive(id, n->id, n->finger.at(0).node->id)) {
		n = n->closestPredFinger(id);
		cout<<">"<<n->id;
	}

	return n;
}

void Node::printData()
{
	cout<<"DATA AT INDEX NODE "<<id<<":"<<endl;
	if (!data.empty())
	{
		for (vector<Data>::iterator it = data.begin(); it != data.end(); it++)
			cout<<it->content<<endl;
	}
}
