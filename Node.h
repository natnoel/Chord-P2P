#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Node;

struct Finger
{
	int start;
	Node* node;
};

struct Data
{
	int key;
	string content;
};

class Node
{
public:
	Node(void);
	~Node(void);
	Node(int, int);

	unsigned int id;
	unsigned int m;
	unsigned int chordSize;
	Node* pred;
	vector<Finger> finger;
	vector<Data> data;

	Node* findSucc(int);
	Node* findPred(unsigned int);
	Node* closestPredFinger(int);

	bool join(Node*);
	void initFingerTable(Node*);
	void updateOthers();
	void updateFingerTable(Node*, int);

	void leave();
	void updateOthersDel();
	void updateFingerTableDel(Node*, int);

	void insertData(int, string);
	void takeOverData(Node*);
	void handOverData(Node*);

	Node* findSuccPrint(int);
	Node* findPredPrint(unsigned int);

	bool inBtwnRightInclusive(int, int, int);
	bool inBtwnExclusive(int, int, int);
	bool inBtwnLeftInclusive(int, int, int);

	void printData();
	void printFingerTable();
};

