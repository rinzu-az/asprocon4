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
  ll pr;
  int a;
  Order (int _r, int _i, int _e, int _d, int _q, ll _pr, int _a) :
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
  Operation (int _r, int _m, int _t1, int _t2, int _t3) :
    r(_r), m(_m), t1(_t1), t2(_t2), t3(_t3) {}
};

int M;
int I;
int R;
int BL;
int CL;
const int MAX_TIME = 86400000;
const ll P_MAX = 1e10;
vector<Order> orders;
vector<Bom> boms;
vector<Operation> operations;
vector<map<Pii, int>> setupTimes;

int main(int argc, char const* argv[]) {
  try {
    ifstream in(argv[1]);
    ifstream out(argv[2]);

    string _str;
    in >> _str >> M >> I >> R >> BL >> CL;
    boms.resize(I);
    for (int n = 0; n < BL; ++n) {
      int i, m, s;
      in >> _str >> i >> m >> s;
      boms[i].mList.push_back(m);
      boms[i].sList.push_back(s);
    }
    setupTimes.resize(M);
    for (int n = 0; n < CL; ++n) {
      int m, i_pre, i_nxt, t;
      in >> _str >> m >> i_pre >> i_nxt >> t;
      setupTimes[m][Pii(i_pre, i_nxt)] = t;
    }
    for (int n = 0; n < R; ++n) {
      int r, i, e, d, q, pr, a;
      in >> _str >> r >> i >> e >> d >> q >> pr >> a;
      orders.push_back(Order(r, i, e, d, q, pr, a));
    }
    sort(all(orders), [](const Order& o1, const Order& o2) { return o1.r < o2.r; });

    for (int n = 0; n < R; ++n) {
      int r, m, t1, t2, t3;
      out >> r >> m >> t1 >> t2 >> t3;
      if (out.eof()) {
        cerr << "output file is too small" << endl;
        return 1;
      }
      if (r < 0 || r >= R) {
        cerr << "r is out of range" << endl;
        return 1;
      }
      if (m < 0 || m >= M) {
        cerr << "m is out of range" << endl;
        return 1;
      }
      if (t1 < 0 || t1 > MAX_TIME) {
        cerr << "t1 is out of range" << endl;
        return 1;
      }
      if (t2 < 0 || t2 > MAX_TIME) {
        cerr << "t2 is out of range" << endl;
        return 1;
      }
      if (t3 <= 0 || t3 > MAX_TIME) {
        cerr << "t3 is out of range" << endl;
        return 1;
      }
      operations.push_back(Operation(r, m, t1, t2, t3));
    }

    set<int> orderSet;
    for (int n = 0; n < R; ++n) {
      if (orderSet.count(operations[n].r) == 1) {
        cerr << "分割禁止 : Split operation detected" << endl;
        return 1;
      }
      orderSet.insert(operations[n].r);
    }

    sort(all(operations), [](const Operation& o1, const Operation& o2) {
          if (o1.m != o2.m) {
            return o1.m < o2.m;
          } else {
            return o1.t1 < o2.t1;
          }
        });


    vector<int> mToPreviousT3(M, 0);
    vector<int> mToPreviousI(M, -1);

    // 作業ごとの制約チェック
    for (int n = 0; n < R; ++n) {
      Operation& ope = operations[n];
      Order& order = orders[ope.r];
      Bom& bom = boms[order.i];
      int m = ope.m;
      int e = (mToPreviousI[m] < 0) ? order.e : max(order.e, mToPreviousT3[m]);
      bool canAssign = false;
      int mIndex = 0;
      for (int j = 0; j < len(bom.mList); ++j) {
        if (bom.mList[j] == m) {
          canAssign = true;
          mIndex = j;
          break;
        }
      }
      if (!canAssign) {
        cerr << "BOM違反 : BOM violation" << endl;
        return 1;
      }
      if (e > ope.t1) {
        cerr << "製造開始時刻違反 : Scheduling start time violation" << endl;
        return 1;
      }
      if (ope.t1 > ope.t2 || ope.t2 > ope.t3) {
        cerr << "段取り開始時刻、製造開始時刻と製造終了時刻順番違反: t1 <= t2 && t2 <= t3" << endl;
        return 1;
      }
      if (mToPreviousI[m] >= 0) {
        int setupT = setupTimes[m][Pii(mToPreviousI[m], order.i)];
        if ((ope.t2 - ope.t1) != setupT) {
          cerr << "段取り時間間違い: Invalid Setup Time" << endl;
          return 1;
        }
      } else {
        if ((ope.t2 - ope.t1) != 0) {
          cerr << "段取り時間間違い: Invalid Setup Time" << endl;
          return 1;
        }
      }
      int manuT = order.q * bom.sList[mIndex];
      if ((ope.t3 - ope.t2) != manuT) {
        cerr << "製造時間間違い : Manufacturing time error" << endl;
        return 1;
      }

      mToPreviousI[m] = order.i;
      mToPreviousT3[m] = ope.t3;
    } // operation loop end


    // 評価
    ll V1 = 0, V2 = 0;
    for (int n = 0; n < R; ++n) {
      Operation& ope = operations[n];
      Order& order = orders[ope.r];
      V1 += ope.t2 - ope.t1;
      if (ope.t3 <= order.d) {
        V2 += order.pr;
      } else {
        ll delay = ope.t3 - order.d;
        V2 += order.pr - ceil((double)order.pr * delay / order.a);
      }
    }

    ll P = P_MAX - V1 + V2;

    printf("IMOJUDGE<<<%lld>>>\n", max(0ll, P));
    return 0;
  } catch (char* str) {
    cerr << "error: " << str << endl;
    return 1;
  }
  return 1;
}
