#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;
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

    }
    void Reset()
    {
        match = false;
    }
};
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
word* NewWord(string name, int num)
{
    word* temp = new word;
    temp->Init(name, num);
    return temp;
}
struct trie
{
    typedef map<string, trie*> next_t;

    // The set with all the letters which this node is prefix
    next_t next;

    // If word is equal to "" is because there is no word in the
    //  dictionary which ends here.
    sentence sen;

    trie() : next(map<string, trie*>()) {}

    void insert(sentence w)
    {
        //w = string("$") + w;
        auto t1 = w.key.insert(w.key.begin(), *NewWord("$", w.index));
        int sz = w.count;

        trie* n = this;
        for (int i = 0; i < sz; ++i) {
            if (n->next.find(w.key[i].value) == n->next.end()) {
                n->next[w.key[i].value] = new trie();
            }

            n = n->next[w.key[i].value];
        }

        n->sen = w;
    }
};

// The tree
trie tree;

// The minimum cost of a given word to be changed to a word of the dictionary
int min_cost;

//
void search_impl(trie* tree, string ch, vector<int> last_row, const sentence& sen)
{
    int sz = last_row.size();

    vector<int> current_row(sz);
    current_row[0] = last_row[0] + 1;

    // Calculate the min cost of insertion, deletion, match or substution
    int insert_or_del, replace;
    for (int i = 1; i < sz; ++i) {
        insert_or_del = min(current_row[i - 1] + 1, last_row[i] + 1);
        replace = (sen.key[i - 1].value.compare(ch)==0) ? last_row[i - 1] : (last_row[i - 1] + 1);

        current_row[i] = min(insert_or_del, replace);
    }

    // When we find a cost that is less than the min_cost, is because
    // it is the minimum until the current row, so we update
   // if ((current_row[sz - 1] < min_cost) && (tree->sen.count != 0)) {
     //   min_cost = current_row[sz - 1];
    //}
      min_cost = current_row[sz - 1];

    // If there is an element wich is smaller than the current minimum cost,
    //  we can have another cost smaller than the current minimum cost
    if (*min_element(current_row.begin(), current_row.end()) < min_cost) {
        for (trie::next_t::iterator it = tree->next.begin(); it != tree->next.end(); ++it) {
            search_impl(it->second, it->first, current_row, sen);
        }
    }
}

int search(sentence w)
{
   // word = string("$") + word;
    auto t1 = w.key.insert(w.key.begin(), *NewWord("$", w.index));
    int sz = w.count;
    min_cost = 100000;

    vector<int> current_row(sz + 1);

    // Naive DP initialization
    for (int i = 0; i < sz; ++i) current_row[i] = i;
    current_row[sz] = sz;



    // For each letter in the root map wich matches with a
    //  letter in word, we must call the search
    for (int i = 0; i < sz; ++i) {
        if (tree.next.find(w.key[i].value) != tree.next.end()) {
            search_impl(tree.next[w.key[i].value], w.key[i].value, current_row, w);
        }
    }

    return min_cost;
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

    return A[len1][len2];
}
int main()
{
    sentence t,y;
    word *x[20];
    x[0] = NewWord("I", 0);
    x[1] = NewWord("have", 0);
    x[2] = NewWord("a", 0);
    x[3] = NewWord("new", 0);
    x[4] = NewWord("car", 0);
    x[5] = NewWord("type", 0);
    x[6] = NewWord("I", 0);
    
    x[6] = NewWord("I", 0);
    x[7] = NewWord("have", 0);
    x[8] = NewWord("a", 0);
    x[9] = NewWord("old", 0);
    x[10] = NewWord("car", 0);
    x[11] = NewWord("bike", 0);
    x[12] = NewWord("bikes", 0);
    x[13] = NewWord("cars", 0);

    for (int i = 0; i < 8; i++)
        t.AddWord(x[i]);

    for (int i = 6; i < 14; i++)
        y.AddWord(x[i]);


    cout<< FindSimilarity(t, y)<<endl;
    tree.insert(t);
    int kq = search(y);
    cout << kq;

}