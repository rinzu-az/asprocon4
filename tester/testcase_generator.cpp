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
#include <chrono>

#define all(X) (X).begin(),(X).end()
#define len(X) ((int)(X).size())
#define fi first
#define sc second
using namespace std;
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int,int> Pii;
typedef pair<ll,ll> Pll;

const string doc = "Test Case Generator\n"
                   "usage: ./a.out [OPTION]\n"
                   "       各オプションを指定しなかった場合の初期値はソースコードを参照してください\n"
                   "options:\n"
                   "  --help ヘルプを読む\n"
                   "  -M     設備数\n"
                   "  -I     品目数\n"
                   "  -R     オーダ数\n"
                   "  -S     製造スピードの最大値\n"
                   "  -ST    段取り時間の最大値\n"
                   "  -Q     オーダの製造数量の最大値\n"
                   "  -E     納期時間の最大値\n"
                   "  -Dmin  (納期時間-最早開始時間)の最小値\n"
                   "  -Dmax  (納期時間-最早開始時間)の最大値\n"
                   "  -A     納期遅れ許容時間の最大値\n"
                   "  -PR    オーダの粗利金額の最大値\n"
                   "  -SU    品目間の段取り時間の生成方法(0: ランダム、1: 対称的)\n"
                   "example:\n"
                   "  ./a.out -M 1 -I 3 -R 100 -S 10000 -ST 20000 -Q 1 -E 691200 -Dmin 86400 -Dmax 691200 -A 86400 -PR 20000 -SU 0";
                   
struct Bom {
  int i;         /* 品目番号 */
  int pr;        /* 品目単価 */
  int p;         /* 割り当て可能設備数 */
  vector<int> m; /* 割り当て可能設備 */
  vector<int> s; /* 製造スピード */
  double mean_s; /* 製造スピードの平均値 */
};


struct Order {
  int r;        /* オーダ番号 */
  int i;        /* 品目番号 */
  int e;        /* 最早開始時間 */
  int d;        /* 納期時間 */
  int q;        /* 製造数量 */
  int pr;       /* オーダの粗利金額 */
  int a;        /* 納期遅れ許容時間 */
};

const int SECOND_A_DAY = 86400;


// parameters
int M    = 1;
int I    = 2;
int R    = 10;
int S    = 3;
int ST   = 1000;
int Q    = 1;
int E    = 3*SECOND_A_DAY;
int Dmin = 1;
int Dmax = SECOND_A_DAY;
int A    = SECOND_A_DAY;
int PR   = 1;
int SU   = 0;

// internal parameters
double manuTimeToPeriodRatio = 2.0;

// range [mi, ma]
int rand(int mi, int ma) {
  static chrono::system_clock::duration dtn = 
    chrono::system_clock::now().time_since_epoch();
  static mt19937 mt(dtn.count()%UINT_MAX);
  double t = (double(mt()))/UINT_MAX * (ma-mi) + mi;
  return min(max(int(round(t)), mi), ma);
}

bool setParameters(vector<string>& params) {
  for (int i = 0; i < len(params); ++i) {
    if (params[i] == "--help") {
      cout << doc << endl;
      return false;
    }
  }
  if (len(params)%2 != 0) {
    cerr << "error: invalid option" << endl;
    return false;
  }
  for (int i = 0; i < len(params); i += 2) {
    if (params[i][0] != '-') {
      cerr << "error: invalid option " << params[i] << endl;
      return false;
    }
    
    int t = 0;
    try {
      t = stoi(params[i+1]);
    } catch (...) {
      cerr << "error: invalid option " << params[i+1] << endl;
      return false;
    }

    string p = params[i].substr(1);

    if (p == "M")         { M = t; }
    else if (p == "I")    { I = t; }
    else if (p == "R")    { R = t; }
    else if (p == "S")    { S = t; }
    else if (p == "ST")   { ST = t; }
    else if (p == "Q")    { Q = t; }
    else if (p == "E")    { E = t; }
    else if (p == "Dmin") { Dmin = t; }
    else if (p == "Dmax") { Dmax = t; } 
    else if (p == "A")    { A = t; }
    else if (p == "PR")   { PR = t; }
    else if (p == "SU")   { SU = t; }
    else { 
      cerr << "error: invalid option " << params[i] << endl; 
      return false;
    }
  }
  return true;
}

void writeTestCase(vector<Bom>& boms, vector<map<Pii, int>>& setupTimes, vector<Order>& orders) {
  vector<string> bomlines;
  vector<string> combilines;
  for (int i = 0; i < len(boms); ++i) {
    Bom& b = boms[i];
    for (int j = 0; j < b.p; ++j) {
      bomlines.push_back("BOM\t" + to_string(b.i) + '\t' + to_string(b.m[j]) + '\t' + to_string(b.s[j]));
    }
  }
  for (int i = 0; i < len(setupTimes); ++i) {
    for (auto p : setupTimes[i]) {
      combilines.push_back("COMBI\t" + to_string(i) + '\t' + to_string(p.fi.fi) + '\t' + to_string(p.fi.sc) + '\t' + to_string(p.sc));
    }
  }
  cout << "HEADER\t" << M << '\t' << I << '\t' << R << '\t' << len(bomlines) << '\t' << len(combilines) << endl;
  for (int i = 0; i < len(bomlines); ++i) {
    cout << bomlines[i] << endl;
  }
  for (int i = 0; i < len(combilines); ++i) {
    cout << combilines[i] << endl;
  }
  for (int i = 0; i < len(orders); ++i) {
    Order& o = orders[i];
    cout << "ORDER\t" << o.r << '\t' << o.i << '\t' << o.e << '\t' << o.d << '\t' << o.q << '\t' << o.pr << '\t' << o.a << '\t' << endl;
  }
}

int main(int argc, char const* argv[]) {

  vector<string> params;
  for (int i = 1; i < argc; ++i) {
    params.push_back(argv[i]);
  }

  if (!setParameters(params)) return 1;


  // BOMを作成
  vector<Bom> boms(I);
  for (int i = 0; i < I; ++i) {
    Bom& b = boms[i];
    b.i = i;
    b.pr = rand(1, max(PR/Q, 1));
    b.p = 0;
    for (int j = 0; j < M; ++j) {
      if (rand(0, 1) == 1) {
        b.m.push_back(j);
        b.s.push_back(rand(1, S));
        b.p++;
      }
    }
    if (b.p == 0) {
      b.m.push_back(rand(0, M-1));
      b.s.push_back(rand(1, S));
      b.p++;
    }
    for (int j = 0; j < b.p; ++j) {
      b.mean_s += b.s[j];
    }
    b.mean_s /= b.p;
  }


  // 各設備で製造可能な品目のリストを作成
  vector<vector<int>> mToItems(M);
  for (int i = 0; i < len(boms); ++i) {
    Bom& b = boms[i];
    for (int j = 0; j < b.p; ++j) {
      mToItems[b.m[j]].push_back(b.i);
    }
  }


  // COMBIを作成
  vector<map<Pii, int>> setupTimes(M);
  if (SU == 0) {
    // 段取り時間をランダムに生成する場合
    for (int i = 0; i < M; ++i) {
      map<Pii, int>& pairToTime = setupTimes[i];
      vector<int>& items = mToItems[i];
      for (int j = 0; j < len(items); ++j) {
        for (int k = 0; k < len(items); ++k) {
          if (items[j] == items[k]) pairToTime[Pii(items[j], items[k])] = 0;
          else pairToTime[Pii(items[j], items[k])] = rand(1, ST);
        }
      }
    }
  } else {
    // 段取り時間に対称性が存在する場合 st(m, i_pre, i_next) = st(m, i_next, i_pre)
    for (int i = 0; i < M; ++i) {
      map<Pii, int>& pairToTime = setupTimes[i];
      vector<int>& items = mToItems[i];
      for (int j = 0; j < len(items); ++j) {
        for (int k = 0; k < len(items); ++k) {
          if (items[j] == items[k]) pairToTime[Pii(items[j], items[k])] = 0;
          else if (j < k) pairToTime[Pii(items[j], items[k])] = rand(1, ST);
          else pairToTime[Pii(items[j], items[k])] = pairToTime[Pii(items[k], items[j])];
        }
      }
    }
  }
  

  // ORDERを作成
  vector<Order> orders(R);
  for (int i = 0; i < R; ++i) {
    Order& o = orders[i];
    o.r = i;
    o.i = rand(0, I-1);
    o.q = rand(1, Q);
    o.pr = o.q*boms[o.i].pr;
    o.a = rand(1, A);

    int meanManuTime = o.q*boms[o.i].mean_s;
    int _Dmin = max(Dmin, int(meanManuTime*manuTimeToPeriodRatio));
    int period;
    if (_Dmin > Dmax) {
      period = Dmax;
    } else {
      period = rand(_Dmin, Dmax);
    }

    o.e = rand(0, E-period);
    o.d = o.e + period;
  }

  writeTestCase(boms, setupTimes, orders);

  return 0;
}
