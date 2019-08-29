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
template <class T>ostream &operator<<(ostream &o,const vector<T>&v)
{o<<"[";for(int i=0;i<(int)v.size();i++)o<<(i>0?", ":"")<<v[i];o<<"]\n";return o;}
clock_t start = clock();

#ifdef LOCAL
  constexpr bool DEBUG = false;
  constexpr bool INFO = true;
  constexpr bool ANSWER = false;
  const double TIME_LIMIT = 9.98;
  mt19937 mt = random_engine();
#else
  constexpr bool DEBUG = false;
  constexpr bool INFO = false;
  constexpr bool ANSWER = true;
  const double TIME_LIMIT = 9.98;
  random_device seed;
  mt19937 mt(seed());
#endif

int rand(int min, int max){
  if(min > max) return min;
  return mt() % (max-min+1) + min;
}

bool chance(int percent){
  return rand(0,99) < percent;
}
bool chance(int r, int max){
  return rand(0,max-1) < r;
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

vector<Operation> toOperations(vector<vector<int>>& ans) {
  vector<Operation> opes;
  for(int i=0; i<M; i++){
    int t1 = 0, prev = -1;
    vector<int>& machine = ans[i];
    for(int j=0; j<len(machine); j++){
      int n = machine[j];
      Order& order = orders[n];
      Bom& bom = boms[order.i];
      Operation ope;
      int s = 1;
      for(auto& b : bom.infos){
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
      opes.push_back(ope);
    }
  }
  return opes;
}

ll eval(vector<vector<int>>& ans){
  ll V1 = 0, V2 = 0;
  for(int i=0; i<M; i++){
    int t1 = 0, prev = -1;
    vector<int>& machine = ans[i];
    for(int j=0; j<len(machine); j++){
      int n = machine[j];
      Order& order = orders[n];
      Bom& bom = boms[order.i];
      Operation ope;
      int s = bom.infos[0].second;
      for(auto& b : bom.infos){
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

      V1 += ope.t2 - ope.t1;
      if (ope.t3 <= order.d) {
        V2 += order.pr;
      } else {
        ll delay = ope.t3 - order.d;
        V2 += order.pr - ceil((double)order.pr * delay / order.a);
      }
    }
  }
  ll P = P_MAX - V1 + V2;
  return P;
}

ll eval(vector<vector<int>>& ans, int m, int n = -1){
  ll V1 = 0, V2 = 0;
  for(int i=0; i<M; i++){
    if(i != m && i != n) continue;
    int t1 = 0, prev = -1;
    vector<int>& machine = ans[i];
    for(int j=0; j<len(machine); j++){
      int n = machine[j];
      Order& order = orders[n];
      Bom& bom = boms[order.i];
      Operation ope;
      int s = bom.infos[0].second;
      for(auto& b : bom.infos){
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

      V1 += ope.t2 - ope.t1;
      if (ope.t3 <= order.d) {
        V2 += order.pr;
      } else {
        ll delay = ope.t3 - order.d;
        V2 += order.pr - ceil((double)order.pr * delay / order.a);
      }
    }
  }
  ll P = - V1 + V2;
  return P;
}

void move_order(vector<vector<int>>& ans, int m, int from, int n, int to){
  int tmp = ans[m][from];
  ans[m].erase(ans[m].begin()+from);
  if(to == len(ans[n])) {
    ans[n].push_back(tmp);
  }else{
    ans[n].insert(ans[n].begin()+to, tmp);
  }
}

vector<vector<int>> neighbor(vector<vector<int>> ans, bool flg = true){
  int m = rand(0,M-1);
  int from = rand(0,len(ans[m])-1);
  int n = m;
  if(flg ? true : chance(80)) {
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

tuple<int,int,int,int,ll> neighborR(vector<vector<int>>& ans, bool flg = true){
  int m = 0;
  if(flg){
    m = rand(0,M-1);
    while(len(ans[m]) == 0) m = rand(0,M-1);
  }else{
    ll min = LLONG_MAX;
    for(int i=0; i<M; i++){
      ll tmp = eval(ans,i);
      if(tmp < min){
        min = tmp; m = i;
      }
    }
  }
  int from = rand(0,len(ans[m])-1);
  int n = m;
  vector<Pii> bominfo = boms[orders[ans[m][from]].i].infos;
  n = bominfo[rand(0,len(bominfo)-1)].first;

  int to = rand(0,len(ans[n])-1);
  if(DEBUG) cout << m << " " << n << " " << from << " " << to << endl;
  if(DEBUG) cout << ans[m] << ans[n];

  ll prevScore = eval(ans, m, m == n ? -1 : n);

  move_order(ans, m, from, n, to);

  ll newScore = eval(ans, m, m == n ? -1 : n);
  return make_tuple(m, from, n, to , newScore - prevScore);
}

vector<vector<int>> shuffle(vector<vector<int>>& ans, int i){
  int m = rand(0,M-1);
  int from = i; // rand(0,len(ans[m])-1);
  int n = rand(0,M-1);;
  int to = rand(0,len(ans[n])-1);
  swap(ans[m][from],ans[n][to]);
  return ans;
}

class simulated_annealing{
  //varibale
  int temp = 1000000;
  double time = 0.0;
  using format = vector<vector<int>>;
  format bestAns, ans;
  ll bestScore = -1e8, score = -1e8;
  int cnt = 0;

  //function
  bool accept(){
    return chance(temp/3 + 100000, 1000000);
  }
  bool accept(int _score, int _newScore){
    return chance( (int)(max(0.0, -(time/TIME_LIMIT)*60.0 + 50.0)*10000 + 50000 - max(0, (_score - _newScore))*2.3), 1000000);
  }
  void change_temp(){
    //if(temp>0) temp -= 2;
  }

  //constructor
  public:
  simulated_annealing() {}
  simulated_annealing(format _ans) :
    ans(_ans) {}
  
  format exec(double timeLimit, int type = 0){
    bool flg = true;
    while((time = get_time()) < timeLimit){
      //if(flg && get_time() > 9.0) flg = false;
      for(int i=0; i<100; i++){
        int m, n, from, to; ll diff;
        tie(m, from, n, to, diff) = neighborR(ans, flg);
        ll newScore = score + diff;
        if(DEBUG) cout << "newScore : " << newScore << " Score : " <<  score << endl;

        if(accept(score, newScore) || newScore > score){
          score = newScore;
          if(newScore > bestScore){
            bestAns = ans; bestScore = newScore;
          }
        }else{
          move_order(ans, n, to, m, from);
        }
        change_temp();
        cnt++;
      }
    }
    if(INFO) cout << cnt << "\t";
    return bestAns;
  }
};

class beam_search{
  //varibale
  int width = 10;
  using format = vector<vector<int>>;
  multimap<ll,format> beams;
  ll bestScore = 0, score = 0;
  int cnt = 0;

  //constructor
  public:
  beam_search() {}
  beam_search(format _ans){
    beams.insert(make_pair(eval(_ans), _ans));
  }
  
  vector<vector<int>> exec(double timeLimit){
    while(get_time() < timeLimit){
      multimap<ll, format> newBeams = beams;
      for(auto beam : beams){
        format newAns = neighbor(beam.second);
        ll newScore = eval(newAns);
        if(DEBUG) cout << "newScore : " << newScore << " Score : " <<  score << endl;

        newBeams.insert(make_pair(newScore, newAns));
        cnt++;
      }
      while((int)newBeams.size() > width){
        newBeams.erase(newBeams.begin());
      }
    }
    if(INFO) cout << cnt << endl;
    return (*beams.rbegin()).second;
  }
};

void solve() {
  vector<Order> _orders = orders;
  stable_sort(all(_orders), [](const Order& o1, const Order& o2) { return o1.d < o2.d; });
  for(auto bom : boms){
    sort(all(bom.infos), [](const Pii& info1, const Pii& info2){ return info1.second > info2.second; });
  }

  vector<Pii> prev(M, Pii(0,-1)); //t3, i
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

  simulated_annealing SA(ans);
  ans = SA.exec(TIME_LIMIT);

  if(INFO) cout << eval(ans) << endl;
  operations = toOperations(ans);
}

int main() {
  readProblem();
  solve();
  if(ANSWER) writeSolution();
  return 0;
}
