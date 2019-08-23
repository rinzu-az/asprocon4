#include <iostream>
#include <algorithm>
#include <bitset>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <cstring>
#include <utility>
#include <vector>
#include <complex>
#include <valarray>
#include <fstream>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <numeric>
#include <climits>
#include <random>

#define all(X) (X).begin(),(X).end()
#define len(X) ((int)(X).size())
#define fi first
#define sc second
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int,int> Pii;
typedef pair<ll,ll> Pll;

struct Order {
  int r;
  int i;
  int e;
  int d;
  int q;
  int pr;
  int a;
  Order (int _r, int _i, int _e, int _d, int _q, int _pr, int _a) :
    r(_r), i(_i), e(_e), d(_d), q(_q), pr(_pr), a(_a) {}
};

struct Bom {
  vector<int> mList;
  vector<int> sList;
};

struct Operation {
  int r;
  int m;
  int t1;
  int t2;
  int t3;
};

int M;
int I;
int R;
int BL;
int CL;
vector<Order> orders;
vector<Bom> boms;
vector<Operation> operations;
vector<map<Pii, int>> setupTimes;

void readProblem() {
  string _str;
  cin >> _str >> M >> I >> R >> BL >> CL;
  boms.resize(I);
  for (int n = 0; n < BL; ++n) {
    int i, m, s;
    cin >> _str >> i >> m >> s;
    boms[i].mList.push_back(m);
    boms[i].sList.push_back(s);
  }
  setupTimes.resize(M);
  for (int n = 0; n < CL; ++n) {
    int m, i_pre, i_nxt, t;
    cin >> _str >> m >> i_pre >> i_nxt >> t;
    setupTimes[m][Pii(i_pre, i_nxt)] = t;
  }
  for (int n = 0; n < R; ++n) {
    int r, i, e, d, q, pr, a;
    cin >> _str >> r >> i >> e >> d >> q >> pr >> a;
    orders.push_back(Order(r, i, e, d, q, pr, a));
  }
}

void writeSolution() {
  for (int n = 0; n < R; ++n) {
    Operation& ope = operations[n];
    cout << ope.r << '\t' << ope.m << '\t' << ope.t1 << '\t' << ope.t2 << '\t' << ope.t3 << endl;
  }
}

void solve() {
  stable_sort(all(orders), [](const Order& o1, const Order& o2) { return o1.e < o2.e; });

  vector<int> mToPreviousT3(M);
  vector<int> mToPreviousI(M, -1);

  operations.resize(R);
  for (int n = 0; n < R; ++n) {
    Order& order = orders[n];
    Bom& bom = boms[order.i];
    Operation& ope = operations[n];
    int m = bom.mList[0];
    int s = bom.sList[0];
    ope.r = order.r;
    ope.m = m;
    if (mToPreviousI[m] < 0) {
      ope.t1 = order.e;
      ope.t2 = ope.t1;
    } else {
      ope.t1 = max(order.e, mToPreviousT3[m]);
      ope.t2 = ope.t1 + setupTimes[m][Pii(mToPreviousI[m], order.i)];
    }
    ope.t3 = ope.t2 + order.q * s;
    mToPreviousI[m] = order.i;
    mToPreviousT3[m] = ope.t3;
  }
}

int main() {
  readProblem();
  solve();
  writeSolution();
  return 0;
}
