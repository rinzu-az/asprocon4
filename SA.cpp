#define LOCAL
#include "bits/stdc++.h"
#ifdef LOCAL
  #include "header.cpp"
#endif

using namespace std;
typedef long long ll;
typedef pair<int,int> P;
typedef unsigned long long ull;
typedef pair<int,int> Pii;
typedef pair<ll,ll> Pll;
#define all(X) (X).begin(),(X).end()
#define len(X) ((int)(X).size())
#define fi first
#define sc second
const int MAX_TIME = 86400000;
const ll P_MAX = 1e10;
clock_t start = clock();

#ifdef LOCAL
  constexpr bool DEBUG = true;
  mt19937 mt = random_engine();
#else
  constexpr bool DEBUG = false;
  random_device seed;
  mt19937 mt(seed());
#endif

int rand(int min, int max){
  return mt() % (max-min+1) + min;
}

bool chance(int percent){
  return rand(0,99) < percent;
}

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
  vector<Pii> infos;
};

struct Operation {
  int r, m, t1, t2, t3;
  Operation (int _r, int _m, int _t1, int _t2, int _t3) :
    r(_r), m(_m), t1(_t1), t2(_t2), t3(_t3) {}
};

ll eval(vector<Operation> operations){
    for (int n = 0; n < R; ++n) {
      Operation ope = operations[n];
      int r,m,t1,t2,t3;
      tie(r, m, t1, t2, t3) = make_tuple(ope.r, ope.m, ope.t1, ope.t2, ope.t3);

      if (r < 0 || r >= R) {
        cerr << "r is out of range" << endl;
        return -1;
      }
      if (m < 0 || m >= M) {
        cerr << "m is out of range" << endl;
        return -1;
      }
      if (t1 < 0 || t1 > MAX_TIME) {
        cerr << "t1 is out of range" << endl;
        return -1;
      }
      if (t2 < 0 || t2 > MAX_TIME) {
        cerr << "t2 is out of range" << endl;
        return -1;
      }
      if (t3 <= 0 || t3 > MAX_TIME) {
        cerr << "t3 is out of range" << endl;
        return -1;
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
      for (int j = 0; j < len(bom.infos); ++j) {
        if (bom.infos[j].first == m) {
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
      int manuT = order.q * bom.infos[mIndex].second;
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
    return P;
}

int M, I, R, BL, CL;
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
    boms[i].infos.push_back(Pii(m,s));
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
  for(auto bom : boms){
    sort(all(bom.infos), [](const Pii& info1, const Pii& info2){ return info1.second > info2.second; });
  }
  vector<int> mToPreviousT3(M);
  vector<int> mToPreviousI(M, -1);

  operations.resize(R);
  for (int n = 0; n < R; ++n) {
    Order& order = orders[n];
    Bom& bom = boms[order.i];
    Operation& ope = operations[n];
    int m, s;
    tie(m,s) = bom.infos[0];
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

template<class T>
class simulated_annealing{
  //varibale
  int temp = 100000;
  T ans;
  int score;

  //constructor
  simulated_annealing(){}
  
  //function
  bool accept(){
    return chance(10);
  }
  void change_temp(){
    temp *= 0.995;
  }
  
  public:
  T exec(){
    for(int i=0; i<10000; i++){
      T newAns = ans.neighbor();
      int newScore = newAns.eval();
      if(accept() || newScore > score){
        tie(ans,score) = make_pair(newAns, newScore);
      }
      change_temp();
    }
    return ans;
  }
};

int main() {
  readProblem();
  solve();
  writeSolution();
  return 0;
}
