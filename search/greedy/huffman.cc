/*
 * huffman.cc
 * Copyright (C) 2014 Liu Xinyu (liuxinyu95@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * D.A. Huffman, "A Method for the Construction of Minimum-Redundancy Codes",
 * Proceedings of the I.R.E., September 1952, pp 1098�C1102.
 */

#include <algorithm> /* builtin swap etc. */
#include <vector> /* array of huffman trees. */
#include <string> /* to store variable-length coding/decoding result. */
#include <map>    /* to store code table. */
#include <cstdio>

using namespace std;

/* Definition of Huffman tree node. */
struct Node {
    int w;
    char c;
    Node *left, *right;
    bool operator<(const Node& a) { return w < a.w; }
};

typedef vector<Node*> Nodes;
typedef map<char, string> CodeTab;

int isleaf(Node* a) { return (!a->left) && (!a->right); }

Node* leaf(char c, int w) {
    Node* n = new Node();
    n->w = w;
    n->c = c;
    n->left = n->right = NULL;
    return n;
}

void release(Node* t) {
    if (t) {
        release(t->left);
        release(t->right);
        delete t;
    }
}

Node* merge(Node* a, Node* b) {
    Node* n = new Node();
    n->w = a->w + b->w;
    n->left = a;
    n->right = b;
    return n;
}

void swap(Nodes& ts, int i, int j, int k) {
    swap(ts[i], ts[ts[j] < ts[k] ? k : j]);
}

/*
 * Method 1, Build the Huffman tree by repeatedly extracting the 2
 * trees with the smallest weight.
 */
Node* huffman(vector<Node*> ts) {
    int n;
    while((n = ts.size()) > 1) {
        for (int i = n - 3; i >= 0; --i)
            if (ts[i] < min(ts[n-1], ts[n-1]))
                swap(ts, i, n-1, n-2);
        ts[n-2] = merge(ts[n-1], ts[n-2]);
        ts.pop_back();
    }
    return ts.front();
}

/* Build the code table from a Huffman tree by traversing */
void codetab(Node* t, string bits, CodeTab& codes) {
    if (isleaf(t))
        codes[t->c] = bits;
    else {
        codetab(t->left, bits + "0", codes);
        codetab(t->right, bits + "1", codes);
    }
}

CodeTab codetable(Node* t) {
    CodeTab codes;
    codetab(t, "", codes);
    return codes;
}

/* Encode text with the code table. */
string encode(CodeTab codes, const char* w) {
    string bits;
    while (*w)
        bits += codes[*w++];
    return bits;
}

/* Decode with a Huffman tree. */
string decode(Node* root, const char* bits) {
    string w;
    while (*bits) {
        Node* t = root;
        while (!isleaf(t))
            t = '0' == *bits++ ? t->left : t->right;
        w += t->c;
    }
    return w;
}

/*
 * Auxiliary function
 * Count the occurrence of every character to build the histogram of a text
 */
map<char, int> freq(const char* w) {
    map<char, int> hist;
    while (*w) ++hist[*w++];
    return hist;
}

/* Turn a symbol-weight histogram into an array of huffman tree leaves. */
vector<Node*> nodes(const map<char, int>& hist) {
    vector<Node*> ns;
    for (map<char, int>::const_iterator it = hist.begin(); it != hist.end(); ++it)
        ns.push_back(leaf(it->first, it->second));
    return ns;
}

void print_tr(Node* t, char end='\n') {
    if (t) {
        printf("(%c:%d ", t->c, t->w);
        print_tr(t->left, 0);
        print_tr(t->right, 0);
        printf(")%c", end);
    }
}

void test() {
    const char* w = "hello, wired world";
    Node* tr = huffman(nodes(freq(w)));
    print_tr(tr);
    string cs = encode(codetable(tr), w);
    printf("code: %s\n", cs.c_str());
    printf("text: %s\n", decode(tr, cs.c_str()).c_str());
    release(tr);
}

int main(int, char**) { test(); }