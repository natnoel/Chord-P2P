#include "Node.h"
#include <fstream>

class Chord
{
public:
	Chord();
	~Chord();

	Node *chordNode;
	unsigned int k;

	void init(int);
	void addPeer(int);
	void removePeer(int);
	Node* find(int,int);
	int hash(string);
	void insert(int, string);
	void print(int);
	void read(char []);
	void garbageCollection();
	int computeFingerTableSize(int);
};

Chord::Chord()
{
	k = 0;
	chordNode = NULL;
}

Chord::~Chord()
{
	if (chordNode != NULL)
	{
		garbageCollection();
	}
}

void Chord::init(int n)
{
	if (chordNode != NULL)
	{
		garbageCollection();
	}

	k = computeFingerTableSize(n);

	cout<<"Tan Shi Terng Leon"<<endl
		<<"4000602"<<endl;

	Node *firstNode = new Node(0, k);
	firstNode->join(NULL);
	chordNode = firstNode;
}

int Chord::computeFingerTableSize(int n)
{
	int i = 0;
	while ((1<<i) < n)
		i++;

	return i;
}

void Chord::addPeer(int id)
{
	Node *n = new Node(id, k);
	n->join(chordNode);
	cout<<"PEER "<<id<<" inserted"<<endl;
}

void Chord::removePeer(int id)
{
	Node *peer = chordNode->findSucc(id);
	if (peer->pred != peer) //If not the last node
	{
		if (chordNode == peer)
			chordNode = peer->pred;

		peer->leave();
		delete peer;
		cout<<"PEER "<<id<<" removed"<<endl;
	}
	else
	{
		Node* pDel = peer;
		chordNode = NULL;
		delete pDel;
		cout<<"This is the last peer. The CHORD is now empty."<<endl;
	}
}

Node* Chord::find(int id, int key)
{
	return chordNode->findSucc(id)->findSuccPrint(key);
}

int Chord::hash(string s)
{
	unsigned int key = 0;
	for (unsigned int i = 0; i < s.length(); i++)
	{
		key = ((key << 5) + key) ^ s.at(i);
	}

	key = key % (1 << k);

	return key;
}

void Chord::insert(int id, string s)
{
	int h = hash(s);
	
	cout<<"INSERTING "<<s<<" AT "<<h<<endl;
	find(id, h)->insertData(h, s);
}

void Chord::print(int key)
{
	Node *n = chordNode->findSucc(key);
	
	if (n != NULL)
	{
		n->printData();
		n->printFingerTable();
	}
	else
		cout<<"A PEER FOR "<<key<<" was not found."<<endl;
}

void Chord::read(char* fileName)
{
	fstream infile(fileName, ios::in);

	if (!infile)
	{
		cout<<"File not found!"<<endl;
		return;
	}

	string command, data;
	int id;
	
	while (getline(infile, command, ' '))
	{
		if (command.compare("addpeer") == 0)
		{
			infile>>id;
			addPeer(id);
			infile.clear();
			infile.ignore(100, '\n');
		}
		else if (command.compare("removepeer") == 0)
		{
			infile>>id;
			removePeer(id);
			infile.clear();
			infile.ignore(100, '\n');
		}
		else if (command.compare("insert") == 0)
		{
			infile>>id;
			infile.clear();
			infile.ignore(5, ' ');
			getline(infile, data);
			insert(id, data);
		}
		else if (command.compare("print") == 0)
		{
			infile>>id;
			print(id);
			infile.clear();
			infile.ignore(100, '\n');
		}
		else if (command.compare("init") == 0)
		{
			infile>>id;
			init(id);
			infile.clear();
			infile.ignore(100, '\n');
		}
	}

	infile.close();
}

void Chord::garbageCollection()
{
	//Free memory
	Node *pDel;
	chordNode->pred->finger.at(0).node = NULL;	//Set predecessor's successor to NULL for a point to terminate

	while (chordNode != NULL)	//While it hasn't reach the terminating point
	{
		pDel = chordNode;		//Garbage collection it as we would in a linked list
		chordNode = chordNode->finger.at(0).node;
		delete pDel;
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout<<"Please enter a the file name when you call the program"<<endl;
		cin.get();
		return 1;
	}

	register Chord c;
	c.read(argv[1]);

	return 0;
}