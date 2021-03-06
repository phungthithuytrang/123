#include<iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string>
#include<deque>
#include<fstream>
#include<vector>
#include <sstream>
#include <iterator>
#include <numeric>
#include <algorithm>
#include<windows.h>
#include <cstdio>
#include <chrono>
#include <unordered_map>
using namespace std;
using namespace std::chrono;

struct word
{
	int belong;
	string value;
	bool match = false;
	bool isstopwword = false;
	void Init(string name, int num)
	{
		value = name;
		belong = num;
	}

	void Print()
	{

		if (match == true)
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 10);
			cout << value << " ";
		}
		else
		{
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 12);
			cout << value << " ";
		}
	}
	void Reset()
	{
		match = false;
	}
};
word* NewWord(string name, int num)
{
	word* temp = new word;
	temp->Init(name, num);
	return temp;
}
struct sentence
{
	int index = 0;
	vector<word> key;
	int count = 0;
	bool end = false;
	bool checked = false;
	void AddWord(word* x)
	{
		key.push_back(*x);
		count++;
	}
	void Print()
	{
		cout << "   ";
		for (word i : key)
		{
			i.Print();
		}
	}
	void Reset()
	{
		for (word i : key)
		{
			i.Reset();
			i.match = false;
		}
	}

};


struct Bword
{
	vector<int> belong;
	string value;

	void Init(string name, int num)
	{
		value = name;
		belong.push_back(num);
	}
};
Bword* NewBWord(word in)
{
	Bword* temp = new Bword;
	temp->Init(in.value, in.belong);
	return temp;
}
class BTreeNode
{
	int t;	 // Minimum degree (defines the range for number of keys)
	BTreeNode** C; // An array of child pointers
	int n;	 // Current number of keys
	bool leaf; // Is true when node is leaf. Otherwise false
public:
	Bword* keys;
	BTreeNode(int _t, bool _leaf); // Constructor

	// A utility function to insert a new key in the subtree rooted with
	// this node. The assumption is, the node must be non-full when this
	// function is called
	void insertNonFull(word k);

	// A utility function to split the child y of this node. i is index of y in
	// child array C[]. The Child y must be full when this function is called
	void splitChild(int i, BTreeNode* y);
	void traverse();
	// A function to search a key in the subtree rooted with this node.
	BTreeNode* search(string k, int& index);
	friend class BTree;
};
class BTree
{
	BTreeNode* root; // Pointer to root node
	int t; // Minimum degree
public:
	// Constructor (Initializes tree as empty)
	BTree(int _t)
	{
		root = NULL; t = _t;
	}
	// function to traverse the tree
	void traverse()
	{
		if (root != NULL) root->traverse();
	}
	// function to search a key in this tree
	BTreeNode* search(string k, int& index)
	{
		return (root == NULL) ? NULL : root->search(k, index);
	}
	// The main function that inserts a new key in this B-Tree
	void insert(word k);
};
// Constructor for BTreeNode class
BTreeNode::BTreeNode(int t1, bool leaf1)
{
	// Copy the given minimum degree and leaf property
	t = t1;
	leaf = leaf1;

	// Allocate memory for maximum number of possible keys
	// and child pointers
	keys = new Bword[2 * t - 1];
	C = new BTreeNode * [2 * t];
	// Initialize the number of keys as 0
	n = 0;
}

// Function to traverse all nodes in a subtree rooted with this node
void BTreeNode::traverse()
{
	// There are n keys and n+1 children, traverse through n keys
	// and first n children
	int i;
	for (i = 0; i < n; i++)
	{
		// If this is not leaf, then before printing key[i],
		// traverse the subtree rooted with child C[i].
		if (leaf == false)
			C[i]->traverse();

		cout << endl << keys[i].value;
		for (int j : keys[i].belong)
			cout << " " << j;
	}

	// Print the subtree rooted with last child
	if (leaf == false)
		C[i]->traverse();
}

// Function to search key k in subtree rooted with this node
BTreeNode* BTreeNode::search(string k, int& index)
{
	// Find the first key greater than or equal to k
	int i = 0;
	while (i < n)
	{
		if (k.compare(keys[i].value) > 0)
			i++;
		else
			break;
	}
	if (i < n)
		// If the found key is equal to k, return this node
		if (k.compare(keys[i].value) == 0)
		{
			index = i;
			return this;
		}
	// If key is not found here and this is a leaf node
	if (leaf == true)
		return NULL;
	// Go to the appropriate child
	return C[i]->search(k, index);
}

// The main function that inserts a new key in this B-Tree
void BTree::insert(word k)
{
	// If tree is empty

	if (root == NULL)
	{
		// Allocate memory for root
		root = new BTreeNode(t, true);
		root->keys[0] = *NewBWord(k); // Insert key
		root->n = 1; // Update number of keys in root
	}
	else // If tree is not empty
	{
		int index;
		BTreeNode* temp = search(k.value, index);
		if (temp != NULL)
		{
			temp->keys[index].belong.push_back(k.belong);
			return;
		}
		// If root is full, then tree grows in height
		if (root->n == 2 * t - 1)
		{
			// Allocate memory for new root
			BTreeNode* s = new BTreeNode(t, false);

			// Make old root as child of new root
			s->C[0] = root;

			// Split the old root and move 1 key to the new root
			s->splitChild(0, root);

			// New root has two children now. Decide which of the
			// two children is going to have new key
			int i = 0;
			if (s->keys[0].value.compare(k.value) < 0)
				i++;

			s->C[i]->insertNonFull(k);

			// Change root
			root = s;
		}
		else // If root is not full, call insertNonFull for root
			root->insertNonFull(k);
	}
}

// A utility function to insert a new key in this node
// The assumption is, the node must be non-full when this
// function is called
void BTreeNode::insertNonFull(word k)
{
	// Initialize index as index of rightmost element
	int i = n - 1;

	// If this is a leaf node
	if (leaf == true)
	{
		// The following loop does two things
		// a) Finds the location of new key to be inserted
		// b) Moves all greater keys to one place ahead
		while (i >= 0 && keys[i].value.compare(k.value) > 0)
		{
			keys[i + 1] = keys[i];
			i--;
		}
		// Insert the new key at found location
		keys[i + 1] = *NewBWord(k);
		n = n + 1;
	}
	else // If this node is not leaf
	{
		// Find the child which is going to have the new key
		while (i >= 0 && keys[i].value.compare(k.value) > 0)
			i--;

		// See if the found child is full
		if (C[i + 1]->n == 2 * t - 1)
		{
			// If the child is full, then split it
			splitChild(i + 1, C[i + 1]);

			// After split, the middle key of C[i] goes up and
			// C[i] is splitted into two. See which of the two
			// is going to have the new key
			if (keys[i + 1].value.compare(k.value) < 0)
				i++;
		}
		C[i + 1]->insertNonFull(k);
	}
}

// A utility function to split the child y of this node
// Note that y must be full when this function is called
void BTreeNode::splitChild(int i, BTreeNode* y)
{
	// Create a new node which is going to store (t-1) keys
	// of y
	BTreeNode* z = new BTreeNode(y->t, y->leaf);
	z->n = t - 1;
	// Copy the last (t-1) keys of y to z
	for (int j = 0; j < t - 1; j++)
		z->keys[j] = y->keys[j + t];
	// Copy the last t children of y to z
	if (y->leaf == false)
	{
		for (int j = 0; j < t; j++)
			z->C[j] = y->C[j + t];
	}
	// Reduce the number of keys in y
	y->n = t - 1;
	// Since this node is going to have a new child,
	// create space of new child
	for (int j = n; j >= i + 1; j--)
		C[j + 1] = C[j];
	// Link the new child to this node
	C[i + 1] = z;
	// A key of y will move to this node. Find the location of
	// new key and move all greater keys one space ahead
	for (int j = n - 1; j >= i; j--)
		keys[j + 1] = keys[j];
	// Copy the middle key of y to this node
	keys[i] = y->keys[t - 1];
	// Increment count of keys in this node
	n = n + 1;
}
string* loadwords(const string& filename)
{
	string* arr = new string[76];
	ifstream file(filename);
	string data;
	int i = 0;
	while (getline(file, data))
	{
		arr[i++] = data;
	}
	file.close();
	return arr;
}
bool lookup(string* words, const string& word)
{
	return binary_search(words, words + 76, word);
}
string* dictionary = loadwords("stopword.txt");
struct text
{
	BTree* tree = new BTree(3);
	vector<sentence> sen;
	int count = 0;
	int meaning = 0;
	bool issour;
	int num_couple = 0;
	void AddSentence(sentence*& s)
	{
		int index;
		s->index = count;
		sen.push_back(*s);
		if (issour == true)
			for (word i : s->key)
			{
				if (i.isstopwword == false)
				{
					tree->insert(i);
					meaning++;
				}
			}
		count++;
	}
	void Print()
	{
		for (sentence i : sen)
		{
			cout << endl << endl;
			i.Print();
		}
	}
};

bool isletter(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}
string pretreatment(string t)
{
	string temp;
	int n = 0;
	int len = t.length();
	for (int i = 0; i < len; i++)
	{
		if (t[i] >= 'A' && t[i] <= 'Z')
			t[i] = t[i] + 32;
		if (i == len - 1) {
			if (t[i] == '.')
				temp.push_back(t[i]);
		}
		if (isletter(t[i]) == true)
		{
			temp.push_back(t[i]);
		}
	}
	return temp;
}
int Getsize(const string name)
{
	long l, m;
	ifstream file(name, ios::in | ios::binary);
	l = file.tellg();
	file.seekg(0, ios::end);
	m = file.tellg();
	file.close();
	cout << "Lay đc size";
	return m;
}
int LikelyIndex(int array[], text* sour)
{

	int imax = 0;
	int n = sour->count;
	for (int i = 0; i < n; i++)
	{
		if (array[i] > array[imax] || (array[i] == array[imax] && sour->sen[i].count < sour->sen[imax].count))
			imax = i;
	}

	if (array[imax] == 0)
		return -1;
	return imax;
}
struct Dif
{
	float value=-1;
};

struct Hash
{
	int count = 0;
	int width;
	unordered_map<long, Dif> map;
	int sum = 0;
	int lastkey=-1;
	void Getnode(int x, int y, float value)
	{
		cout << "(" << x << "," << y << ")" << ":" << value << endl;
		long key = x * width + y;
		if (key != lastkey)
		{
			Dif x;
			x.value = value;
			map[key] = x;
			sum += 1 - value;
			count++;
		}
		else
		{
			if (map[lastkey].value > value)
			{
				sum += map[lastkey].value - value;
				map[lastkey].value = value;
			}
		}
		lastkey = key;
		return;
	}
	float Get(int x, int y)
	{
		long key = x * width + y;
		if (map[key].value==-1)
			if (x == 0 || y == 0)
				return x > y ? x : y;
			else
				return 1;
		else
			return map[key].value;
	}
	float avrg()
	{
		return sum;
	}
	void Print()
	{
		for (auto i : map)
			cout << "[" << i.first << "]" << ":" << i.second.value << endl;
	}
	void DeleteAbove(int row)
	{
		row--;
		long key;
		for (int i = 1; i < width; i++)
		{
			key = row * width + i;
			map.erase(key);
		}
	}
};

Hash CreateHash(int width)
{
	Hash hash;
	hash.width = width;
	return hash;
}


float FindSimilarity(sentence& s1, sentence& s2)
{
	sentence s3 = s1;
	sentence s4 = s2;
	int count = 0;
	auto t1 = s3.key.insert(s3.key.begin(), *NewWord("", s3.index));
	auto t2 = s4.key.insert(s4.key.begin(), *NewWord("", s4.index));

	int len1 = s3.count;
	int len2 = s4.count;
	int** A = new int* [len1];
	for (int i = 0; i <= len1; i++)
		A[i] = new int[len2];
	int maxlen = (len1 > len2) ? len1 : len2;
	int minlen = (len2 > len1) ? len1 : len2;
	for (int i = 0; i <= len1; i++)
		A[i][0] = i;
	for (int j = 0; j <= len2; j++)
		A[0][j] = j;
	int cost;
	int min1;
	int i = 0, j = 0;
	for (int i = 1; i <= len1; i++)
		for (int j = 1; j <= len2; j++)
		{
			if (s3.key[i].value.compare(s4.key[j].value) == 0)
			{
				if (s1.key[i - 1].match == false)
					count++;
				s1.key[i - 1].match = true;
				s2.key[j - 1].match = true;
				cost = 0;
			}
			else
			{
				cost = 1;
			}
			min1 = ((A[i - 1][j] + 1) < (A[i][j - 1] + 1) ? A[i - 1][j] + 1 : A[i][j - 1] + 1);
			A[i][j] = min1 < (A[i - 1][j - 1] + cost) ? min1 : (A[i - 1][j - 1] + cost);
		}

	float kq1 = 1 - A[len1][len2] / (float)maxlen;
	float kq2 = count / (float)maxlen;
	float kq = (kq1 + kq2) / 2;

	if (kq < 0.3)
	{
		s1.Reset();
		s2.Reset();
	}

	return kq;
}
int Findthefirst(sentence s, text* sour)
{
	int found;
	int index;
	BTreeNode* temp;
	int* track = new int[sour->count];
	for (int i = 0; i < sour->count; i++)
		track[i] = 0;
	for (word i : s.key)
	{
		if (i.isstopwword == false)
		{
			temp = sour->tree->search(i.value, index);
			if (temp != NULL)
			{
				for (int j : temp->keys[index].belong)
					track[j]++;
			}
		}
	}
	found = LikelyIndex(track, sour);
	free(track);
	return found;
}
void BigLevenstein(Hash table, int N, int M)
{
	if (table.count == 0)
	{
		cout << "Do giong nhau la: " << 0;
		return;
	}
	else
	{
		float cost;
		float min1;
		float min2;
		int max = (N > M) ? N : M;
		float** leventable = new float* [N];
		for (int i = 0; i <= N; i++)
			leventable[i] = new float[M];
		for (int i = 0; i <= N; i++)
			leventable[i][0] = i;
		for (int j = 0; j <= M; j++)
			leventable[0][j] = j;
		for (int i = 1; i <= N; i++)
			for (int j = 1; j <= M; j++)
			{
				cost = table.Get(i, j);
				min1 = (leventable[i - 1][j] + 1 < leventable[i][j - 1] + 1) ? leventable[i - 1][j] + 1 : leventable[i][j - 1] + 1;
				min2 = min1 < (leventable[i - 1][j - 1] + cost) ? min1 : (leventable[i - 1][j - 1] + cost);
				leventable[i][j] = min2;
			}
		cout << endl;
		for (int i = 0; i <= 2; i++)
		{
			for (int j = 0; j <= M; j++)
				cout << leventable[i][j] << " ";
			cout << endl;
		}
		cout << "@" << min2;
		float kq = 1 - (leventable[N][M]) / (float)max;
		float kq1 = table.avrg() / (float)max;
		cout << endl << kq << " " << kq1 << endl;
		kq = (kq1 + kq) / 2;

		cout << "Do giong nhau la: " << kq;
		cout << endl;
	}
}
/*void BigLevenstein(Hash table, int N, int M)
{
	if (table.count == 0)
	{
		cout << "Do giong nhau la: " << 0;
		return;
	}
	else
	{
		Hash big=CreateHash(M);
		float cost;
		float min1;
		float min2;
		int max = (N > M) ? N : M;
	
		for (int i = 1; i <= N; i++)
		{
			for (int j = 1; j <= M; j++)
			{
				cost = table.Get(i, j);
				min1 = (big.Get(i - 1, j) + 1 < big.Get(i, j - 1) + 1) ? big.Get(i - 1, j) + 1 : big.Get(i, j - 1) + 1;
				min2 = min1 < (big.Get(i - 1, j - 1) + cost) ? min1 : (big.Get(i - 1, j - 1) + cost);
				big.Getnode(i, j, min2);
				cout << min2 << " ";
			}
			cout << endl;
			big.DeleteAbove(i);
		}

		float kq = 1 - (big.Get(N,M)) / (float)max;
		float kq1 = table.avrg() / (float)max;
		kq = (kq1 + kq) / 2;

		cout << "Do giong nhau la: " << kq;
		cout << endl;
	}
}*/

void Compare(text* des, text* sour)
{
	float tempvalue = 0;
	int max = (des->count > sour->count) ? des->count : sour->count;
	int count = 0;
	int temp = 0, temp1 = -1;
	int flag = 0;
	int i = 0;
	int old = -1;
	Hash table = CreateHash(sour->count);
	
	while (count < des->count)
	{
		if (des->sen[count].checked == false)
		{
			temp = Findthefirst(des->sen[count], sour);
			des->sen[count].checked = true;
		}
		else
		{
			temp1 = -1;
			count++;
			continue;
		}
		if (temp != -1)
		{
			if (temp != temp1)
			{
				tempvalue = FindSimilarity(des->sen[count], sour->sen[temp]);
				flag = 0;
				temp1 = -1;
			}
			else
			{
				count++;
				temp1 = -1;
				continue;
			}
			while (tempvalue > 0.7)
			{
				table.Getnode(count + 1, temp + 1, 1-tempvalue);
				if (count < des->count - 1 && temp < sour->count - 1)
				{
					count++;
					temp++;
				}
				else
				{
					count++;
					temp++;
					flag = 1;
					break;
				}
				tempvalue = FindSimilarity(des->sen[count], sour->sen[temp]);
				flag = 0;
			}
			if (flag == 0 && tempvalue > 0.3)
			{
				table.Getnode(count + 1, temp + 1, 1-tempvalue);
				old = tempvalue;
				temp1 = temp;
			}
			else if (flag == 0 && tempvalue <= 0.3)
				if (des->sen[count].checked == true)
					count++;
		}
		else
		{
			count++;
		}
	}
	cout << "/"<<table.count << " ";
	table.Print();
	BigLevenstein(table, des->count, sour->count);
}
void ReadLine(text*& tree, string datain, sentence*& last)
{
	sentence* t;
	t = last;
	word* c;
	int len;
	string dataout;
	datain = datain + ' ';
	std::stringstream ss(datain);
	while (getline(ss, dataout, ' '))
	{
		dataout = pretreatment(dataout);

		if (dataout.length() == 0)
		{
			continue;
		}
		len = dataout.length();

		if (dataout[len - 1] == '.')
		{
			dataout.pop_back();
			c = NewWord(dataout, tree->count);
			if (lookup(dictionary, dataout) == true)
				c->isstopwword = true;
			t->AddWord(c);
			t->end = true;
			tree->AddSentence(t);
			t = new sentence;
		}
		else
		{
			c = NewWord(dataout, tree->count);
			if (lookup(dictionary, dataout) == true)
				c->isstopwword = true;
			t->AddWord(c);
		}
	}
	last = t;
}
text* CreateTree(const string name, bool issource)
{
	text* tree = new text;
	tree->issour = issource;
	sentence* lastsen = new sentence;
	word* c;
	int len;
	string datain, dataout;
	
	ifstream file(name);
	if (!file.is_open())
	{
		cout << "Ko mo dc file";
		return nullptr;
	}
	cout << "Mo file thanh cong";
	while (getline(file, datain))
	{
		ReadLine(tree, datain, lastsen);
	}

	if (lastsen->count != 0)
	{
		tree->AddSentence(lastsen);
	}

	return tree;
}
void Getfile(string text1, string text2)
{
	ofstream outfile;
	outfile.open("out.txt", ios::out | ios::trunc);

	text* des;
	text* sour;

	int m = Getsize(text1);
	int n = Getsize(text2);

	outfile << m << "\t"<<n<<"\t";
	float x = 0;
	bool issour = true;
	if (m <= n)
	{
		auto start = high_resolution_clock::now();
		des = CreateTree(text1, !issour);
		sour = CreateTree(text2, issour);
		//des = CreateTree(text2, !issour);
		//sour = CreateTree(text1, issour);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		x = (float)duration.count() / 1000;
		outfile << x << "\t";
		cout << "Thoi gian doc du lieu: " << x <<endl;
	}
	else
	{
		auto start = high_resolution_clock::now();
		des = CreateTree(text2, !issour);
		sour = CreateTree(text1, issour);
		//des = CreateTree(text1, !issour);
		//sour = CreateTree(text2, issour);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		x = (float)duration.count() / 1000;
		outfile << x << "\t";

		cout << "Thoi gian doc du lieu: " << x << endl;

	}
	cout << des->count << "//" << sour->count;
	if (des != nullptr && sour != nullptr)
	{
		auto start = high_resolution_clock::now();
		Compare(des, sour);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		x = (float)duration.count() / 1000;
		outfile << x << endl;
		cout << "Thoi gian so sanh: " << x << endl;
		
	}
}

int main()
{
	Getfile("3text1.txt", "4text1.txt");
}
