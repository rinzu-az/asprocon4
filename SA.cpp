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
  constexpr bool DEBUG = false;
  const double TIME_LIMIT = 9.8;
  mt19937 mt = random_engine();
#else
  constexpr bool DEBUG = false;
  const double TIME_LIMIT = 9.8;
  random_device seed;
  mt19937 mt(seed());
#endif

int rand(int min, int max){
  return mt() % (max-min+1) + min;
}

bool chance(int percent){
  return rand(0,99) < percent;
}

double get_time(){
  return (double)(clock() - start) / CLOCKS_PER_SEC;
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
  vector<Pii> infos; //m, s
};

struct Operation {
  int r, m, t1, t2, t3;
  Operation (){}
  Operation (int _r, int _m, int _t1, int _t2, int _t3) :
    r(_r), m(_m), t1(_t1), t2(_t2), t3(_t3) {}
};


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

vector<Operation> toOperations(vector<vector<int>> ans) {
  vector<Operation> opes(R);
  for(int i=0; i<M; i++){
    int t1 = 0, prev = -1;
    vector<int> machine = ans[i];
    for(int j=0; j<len(machine); j++){
      int n = machine[j];
      Order& order = orders[n];
      Bom& bom = boms[order.i];
      Operation& ope = opes[order.r];
      int s = 1;
      for(auto b : bom.infos){
        if(b.first == i){
          s = b.second; break;
        }
      }
      ope.r = order.r;
      ope.m = i;
      if (prev < 0) {
        ope.t1 = order.e;
        ope.t2 = ope.t1;
      } else {
        ope.t1 = max(order.e, t1);
        ope.t2 = ope.t1 + setupTimes[i][Pii(prev, order.i)];
      }
      ope.t3 = ope.t2 + order.q * s;
      t1 = ope.t3; prev = order.i;
    }
  }
  return opes;
}

vector<vector<int>> neighbor(vector<vector<int>> ans, bool flg){
  int m = rand(0,M-1);
  int from = rand(0,len(ans[m])-1);
  int n = m;
  if(flg ? true : chance(100)) {
    vector<Pii> bominfo = boms[orders[ans[m][from]].i].infos;
    n = bominfo[rand(0,len(bominfo)-1)].first;
  }
  int to = rand(0,len(ans[n])-1);
  int tmp = ans[m][from];
  ans[m].erase(ans[m].begin()+from);
  ans[n].insert(ans[n].begin()+to,tmp);
  if(DEBUG) cout << m << " " << from << " " << to << endl;
  return ans;
}

ll eval(vector<Operation> operations){
    for (int n = 0; n < R; ++n) {
      Operation ope = operations[n];
      int r,m,t1,t2,t3;
      tie(r, m, t1, t2, t3) = make_tuple(ope.r, ope.m, ope.t1, ope.t2, ope.t3);

      if (r < 0 || r >= R) {
        if(DEBUG) cerr << "r is out of range" << endl;
        return -1;
      }
      if (m < 0 || m >= M) {
        if(DEBUG) cerr << "m is out of range" << endl;
        return -1;
      }
      if (t1 < 0 || t1 > MAX_TIME) {
        if(DEBUG) cerr << "t1 is out of range" << endl;
        return -1;
      }
      if (t2 < 0 || t2 > MAX_TIME) {
        if(DEBUG) cerr << "t2 is out of range" << endl;
        return -1;
      }
      if (t3 <= 0 || t3 > MAX_TIME) {
        if(DEBUG) cerr << "t3 is out of range" << endl;
        return -1;
      }
    }

    set<int> orderSet;
    for (int n = 0; n < R; ++n) {
      if (orderSet.count(operations[n].r) == 1) {
        if(DEBUG) cerr << "分割禁止 : Split operation detected" << endl;
        return -1;
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
        if(DEBUG) cerr << "BOM違反 : BOM violation " << n << " " << ope.m << " " << ope.r << " " << ope.t1 << " " << ope.t3 << endl;
        return -1;
      }
      if (e > ope.t1) {
        if(DEBUG) cerr << "製造開始時刻違反 : Scheduling start time violation " << n  << " " << order.e << " " << e << " " << ope.r << endl;
        return -1;
      }
      if (ope.t1 > ope.t2 || ope.t2 > ope.t3) {
        if(DEBUG) cerr << "段取り開始時刻、製造開始時刻と製造終了時刻順番違反: t1 <= t2 && t2 <= t3" << endl;
        return -1;
      }
      if (mToPreviousI[m] >= 0) {
        int setupT = setupTimes[m][Pii(mToPreviousI[m], order.i)];
        if ((ope.t2 - ope.t1) != setupT) {
          if(DEBUG) cerr << "段取り時間間違い: Invalid Setup Time" << endl;
          return -1;
        }
      } else {
        if ((ope.t2 - ope.t1) != 0) {
          if(DEBUG) cerr << "段取り時間間違い: Invalid Setup Time" << endl;
          return -1;
        }
      }
      int manuT = order.q * bom.infos[mIndex].second;
      if ((ope.t3 - ope.t2) != manuT) {
        if(DEBUG) cerr << "製造時間間違い : Manufacturing time error" << endl;
        return -1;
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

class simulated_annealing{
  //varibale
  int temp = 100000;
  vector<vector<int>> ans;
  ll score = 0;
  int cnt=0;

  //function
  bool accept(){
    return chance(temp/10000000);
  }
  void change_temp(){
    temp *= 0.995;
  }

  //constructor
  public:
  simulated_annealing() {}
  simulated_annealing(vector<vector<int>> _ans) :
    ans(_ans) {}
  
  vector<vector<int>> exec(){
    bool flg = true;
    while(get_time() < TIME_LIMIT){
      if(flg && get_time() > 7.0) flg = false;
      for(int i=0; i<100; i++){
        vector<vector<int>> newAns = neighbor(ans, flg);
        ll newScore = eval(toOperations(newAns));
        if(DEBUG){
          cout << "newScore : " << newScore << " Score : " <<  score << endl;
          cnt++;
        }
        if(accept() || newScore > score){
          tie(ans,score) = make_pair(newAns, newScore);
        }
        change_temp();
      }
    }
    if(DEBUG) cout << cnt << endl;
    return ans;
  }
};

void solve() {
  vector<Order> _orders = orders;
  stable_sort(all(_orders), [](const Order& o1, const Order& o2) { return o1.d < o2.d; });
  for(auto bom : boms){
    sort(all(bom.infos), [](const Pii& info1, const Pii& info2){ return info1.second > info2.second; });
  }

  //int hoge = M==1 ? 2e7 : 0;
  vector<Pii> prev(M,Pii(0,-1)); //t3, i
  vector<vector<int>> ans(M);

  operations.resize(R);
  for (int n = 0; n < R; ++n) {
    Order& order = _orders[n];
    Bom& bom = boms[order.i];
    Operation& ope = operations[order.r];
    int m, s, mint3;
    Pii info = bom.infos[0];
    tie(m,s) = info;
    mint3 = prev[info.first].first;

    for(int j=0; j<len(bom.infos); j++){
      Pii info = bom.infos[j];
      int prevt3 = prev[info.first].first;
      if(mint3 > prevt3){
        mint3 = prevt3;
        tie(m,s) = bom.infos[j];
      }
    }
    
    ope.r = order.r;
    ope.m = m;
    if (prev[m].second < 0) {
      ope.t1 = max(order.e, prev[m].first);
      ope.t2 = ope.t1;
    } else {
      ope.t1 = max(order.e, prev[m].first);
      ope.t2 = ope.t1 + setupTimes[m][Pii(prev[m].second, order.i)];
    }
    ope.t3 = ope.t2 + order.q * s;
    prev[m] = Pii(ope.t3, order.i);
    ans[m].push_back(order.r);
  }
  eval(operations);
  eval(toOperations(ans));
  simulated_annealing SA(ans);
  ans = SA.exec();

  operations = toOperations(ans);
}

void Xsolve() {
  stable_sort(all(orders), [](const Order& o1, const Order& o2) { return o1.d < o2.d; });
  for(auto bom : boms){
    sort(all(bom.infos), [](const Pii& info1, const Pii& info2){ return info1.second > info2.second; });
  }

  int hoge = M==1 ? 2e7 : 0;
  vector<Pii> prev(M,Pii(hoge,-1)); //t3, i

  operations.resize(R);
  for (int n = 0; n < R; ++n) {
    Order& order = orders[n];
    Bom& bom = boms[order.i];
    Operation& ope = operations[order.r];
    int m, s, mint3;
    Pii info = bom.infos[0];
    tie(m,s) = info;
    mint3 = prev[info.first].first;

    for(int j=0; j<len(bom.infos); j++){
      Pii info = bom.infos[j];
      int prevt3 = prev[info.first].first;
      if(mint3 > prevt3){
        mint3 = prevt3;
        tie(m,s) = bom.infos[j];
      }
    }
    ope.r = order.r;
    ope.m = m;
    if (prev[m].second < 0) {
      ope.t1 = max(order.e, prev[m].first);
      ope.t2 = ope.t1;
    } else {
      ope.t1 = max(order.e, prev[m].first);
      ope.t2 = ope.t1 + setupTimes[m][Pii(prev[m].second, order.i)];
    }
    ope.t3 = ope.t2 + order.q * s;
    prev[m] = Pii(ope.t3, order.i);
  }
}


int main() {
  readProblem();
  solve();
  writeSolution();
  return 0;
}
