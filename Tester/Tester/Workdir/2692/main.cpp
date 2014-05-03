#include <fstream>
#include <string>
#include <algorithm>
 
using namespace std;
 
int kol = 0, col[10000];
string mp[10000];
 
struct g
{
    int e, b, l;
};
 
int fd(string s)
{
    for (int i = 0; i <= kol; ++i)
        if (s == mp[i])
            return i;
    mp[kol++] = s;
    return kol - 1;
}
bool cmp(g a, g b)
{
    return a.l < b.l;
}
 
void unite(int a, int b)
{
    for (int i = 0; i <= kol; ++i)
        if (col[i] == b)
            col[i] = a;
}
 
int main ()
{
    ifstream cin("input.txt");
    ofstream cout ("output.txt");
    g reb[10000];
    int n, k, dl = 0;
    cin >> n;
    string s, ip;
    for (int i = 0; i < n; ++i)
    {
        cin >> s;
        reb[i].b = fd(s);
        cin >> s;
        reb[i].e = fd(s);
        cin >> k;
        reb[i].l = k;
    }
    sort(reb, reb + n, &cmp);
    for (int i = 0; i < kol; ++i)
        col[i] = i;
    for (int i = 0; i < n; ++i)
        if (col[reb[i].b] != col[reb[i].e])
        {
            unite(col[reb[i].b], col[reb[i].e]);
            dl += reb[i].l;
        }
    for (int i = 1; i < kol; ++i)
        if (col[i] != col[i - 1])
        {
            cout << "-1";
            return 0;
        }
    cout << dl;
}