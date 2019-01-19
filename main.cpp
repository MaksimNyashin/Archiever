#include <stdio.h>
#include <iostream>
/*#ifdef MAKS
    #define QWE ""
    #define mp make_pair
    #define pb push_back
    #define sz(a) ((int)a.size())
    #define cina(a, n) {a.resize(n); for (auto &i_i: a) cin >> i_i};
    #define couta(a) {for (auto i_i: a) cout << i_i << ' '; cout << '\n';}
#endif // MAKS*/
#define PICTUR
#define toChar(ch) ((ch < N / 2)? char(ch): char(ch - N))

using namespace std;

typedef long long ll;
typedef long double ld;

const int INF = 1e9 + 9;
const ld EPS = 1e-9;
const int N = 256;
const int NN = 8;
const char* inputFile = "input.jpg";
const char* outputFile = "output.jpg";
//const char* inputFile = "input.jpeg";
//const char* outputFile = "output.jpeg";

int counter = 0;

struct T
{
    int w, id;
    T(int w, int id): w(w), id(id){}
    T(){};
};

struct Tree
{
    int l, r, p;
    Tree(): l(-1), r(-1), p(-1){}
    Tree(Tree &a, Tree &b, int idA, int idB)
    {
        l = idA;
        r = idB;
        p = -1;
        a.p = counter;
        b.p = counter;
    }
    Tree(int p): l(-1), r(-1), p(p) {}
};

FILE *f1, *f2;
FILE *fd;
ll cnt[N];//char -> number of chars in the file
T arr[N];
Tree tree[N * 2];
bool toBin[N];//helps build array of bytes for chars in dfs
int toBinPos = 0;
bool toWrite[N * 2][N];//vertex number -> array of bytes
int toWriteSize[N * 2];//vertex number -> size of array of bytes
int charsForOutput[N];//char -> vertex number
char antiCharsForOutput[N * 2];//vertex number -> char
bool traversalOrder[N * 3];//ïîðÿäîê îáõîäà
int traversalOrderCounter = 0;
string vertexOrder;

void dfs(int v)
{
    if (tree[v].l == -1)
    {
        toWriteSize[v] = toBinPos;
        vertexOrder += antiCharsForOutput[v];
        for (int i = 0; i < toBinPos; i++)
            toWrite[v][i] = toBin[i];
        traversalOrder[traversalOrderCounter] = 0;
        traversalOrderCounter++;
        return;
    }
    toBin[toBinPos] = 0;
    toBinPos++;
    if (tree[v].l != -1)
    {
        traversalOrder[traversalOrderCounter] = 1;
        traversalOrderCounter++;
        dfs(tree[v].l);
    }
    toBin[toBinPos - 1] = 1;
    if (tree[v].r != -1)
    {
        traversalOrder[traversalOrderCounter] = 1;
        traversalOrderCounter++;
        dfs(tree[v].r);
    }
    toBinPos--;
}

void writeBytes(int number)
{
    f1 = fopen(inputFile, "rb");
    fprintf(f2, "%c", toChar(number - 1));
    int write = 0;
    int bal = 0;
    for (int i = 0; i < (int)number * 3; i++)
    {
        write = (write << 1) | traversalOrder[i];
        bal++;
        if (bal == NN)
        {
            fprintf(f2, "%c", toChar(write));
            write = 0;
            bal = 0;
        }
    }
    for (int i = 0; i < number; i++)
    {
        for (int j = NN - 1; j >= 0; j--)
        {
            write = (write << 1) | bool(vertexOrder[i] & (1 << j));
            bal++;
            if (bal == NN)
            {
                fprintf(f2, "%c", toChar(write));
                write = 0;
                bal = 0;
            }
        }
    }
    unsigned char c;
    while (fscanf(f1, "%c", &c) != -1)
    {
        int cpos = charsForOutput[c];
        for (int i = 0; i < toWriteSize[cpos]; i++)
        {
            write = (write << 1) | toWrite[cpos][i];
            bal++;
            if (bal == NN)
            {
                fprintf(f2, "%c", toChar(write));
                write = 0;
                bal = 0;
            }
        }
    }
    if (bal > 0)
    {
        write <<= (NN - bal);
        fprintf(f2, "%c", toChar(write));
        fprintf(f2, "%c", toChar(bal));
    }
    else
        fprintf(f2, "%c", toChar(NN));
}

void toArchive()
{
    f1 = fopen(inputFile, "rb");
    f2 = fopen("input.rar", "wb");
    unsigned char c;
	while (fscanf(f1, "%c", &c) != -1)
        cnt[c]++;

    for (int i = 0; i < N; i++)
    {
        if (cnt[i] == 0)
            continue;
        arr[counter] = T(cnt[i], counter);
        tree[counter] = Tree();
        charsForOutput[i] = counter;
        antiCharsForOutput[counter] = i;
        counter++;
    }
    int counter0 = counter;
    int counterDec = counter;
    for (int i = 1; i < counter0; i++)
    {
        int m1 = 0, m2 = 1;
        if (arr[m1].w > arr[m2].w)
        {
            int m = m1;
            m1 = m2;
            m2 = m;
        }
        for (int j = 2; j < counterDec; j++)
        {
            if (arr[j].w < arr[m1].w)
            {
                m2 = m1;
                m1 = j;
            }
            else if (arr[j].w < arr[m2].w)
                m2 = j;
        }
        tree[counter] = Tree(tree[arr[m1].id], tree[arr[m2].id], arr[m1].id, arr[m2].id);
        arr[m1] = T(arr[m1].w + arr[m2].w, counter);
        T nt = arr[m2];
        arr[m2] = arr[counterDec - 1];
        arr[counterDec - 1] = nt;
        counterDec--;
        counter++;
    }
    dfs(arr[0].id);
    writeBytes(counter0);
    fclose(f1);
    fclose(f2);
}


Tree faTree[N * 2];
int faCharsList[N];//number char -> vertex number
int faCharsListCounter = 0;
int faCharsListChars[N];//number char -> char
int faAntiCharsList[N * 2];//vertex number -> number char

void fromArhive()
{
    f1 = fopen("input.rar", "rb");
    f2 = fopen(outputFile, "wb");
    unsigned char nn;
    fscanf(f1, "%c", &nn);
    int n = nn + 1;
    unsigned char c;
    int curVertex = 0;
    faTree[0] = Tree();
    int vertexCounter = 1;
    int rest = 0;
    for (int i = 0; i < 3 * (int)n; i += 8)
    {
        fscanf(f1, "%c", &c);
        for (int j = NN - 1; j >= max(0, NN - (3 * n - i)); j--)
        {
            rest = j - 1;
            bool ch = c & (1 << j);
            if (!ch)
            {
                if (curVertex == 0)
                    continue;
                faCharsList[faCharsListCounter] = curVertex;
                faAntiCharsList[curVertex] = faCharsListCounter;
                faCharsListCounter++;
                curVertex = faTree[curVertex].p;
                while (curVertex != 0 && faTree[curVertex].r != -1)
                    curVertex = faTree[curVertex].p;
            }
            else
            {
                c ^= (1 << j);
                if (faTree[curVertex].l == -1)
                    faTree[curVertex].l = vertexCounter;
                else
                    faTree[curVertex].r = vertexCounter;
                faTree[vertexCounter] = Tree(curVertex);
                curVertex = vertexCounter;
                vertexCounter++;
                if (vertexCounter == 0)
                    fprintf(fd, "%d %d\n", i, j);
            }
        }
    }
    int restc = c;
    int bal = rest + 1;
    for (int i = 0; i < n; i++)
    {
        fscanf(f1, "%c", &c);
        for (int j = NN - 1; j >= 0; j--)
        {
            restc = (restc << 1) | bool(c & (1 << j));
            bal++;
            if (bal == NN)
            {
                rest = j - 1;
                faCharsListChars[i] = restc;
                restc = 0;
                bal = 0;
            }
        }
    }
    int pos = 0;
    unsigned char c1, c2;
    fscanf(f1, "%c", &c1);
    int toFor = 0;
    bool ret = 0;
    if (fscanf(f1, "%c", &c2) == -1)
        toFor = NN - c1, ret = 1;
    for (int i = rest; i >= toFor; i--)
    {
        if (restc & (1 << i))
            pos = faTree[pos].r;
        else
            pos = faTree[pos].l;
        if (faTree[pos].l == -1)
        {
            fprintf(f2, "%c", toChar(faCharsListChars[faAntiCharsList[pos]]));
            pos = 0;
        }
    }
    if (ret)
        return;
    while (fscanf(f1, "%c", &c) != -1)
    {
        for (int i = NN - 1; i >= 0; i--)
        {
            if (c1 & (1 << i))
                pos = faTree[pos].r;
            else
                pos = faTree[pos].l;
            if (faTree[pos].l == -1)
            {
                fprintf(f2, "%c", toChar(faCharsListChars[faAntiCharsList[pos]]));
                pos = 0;
            }
        }
        c1 = c2;
        c2 = c;
    }
    for (int i = 1; i <= c2; i++)
    {
        if (c1 & (1 << (NN - i)))
            pos = faTree[pos].r;
        else
            pos = faTree[pos].l;
        if (faTree[pos].l == -1)
        {
            fprintf(f2, "%c", toChar(faCharsListChars[faAntiCharsList[pos]]));
            pos = 0;
        }
    }
    fclose(f1);
    fclose(f2);
}

int main()
{
    toArchive();
    fromArhive();
    return 0;
}
