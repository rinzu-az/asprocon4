#define LOCAL
#include "bits/stdc++.h"
#ifdef LOCAL
  #include "header.cpp"
#endif

using namespace std;
typedef long long ll;
typedef pair<int,int> P;
const int INF = 1e9;
const ll LINF = LLONG_MAX;
const int NO_NODE = -101;

clock_t start = clock();

#ifdef LOCAL
  constexpr bool DEBUG = true;
  mt19937 mt = random_engine();
#else
  constexpr bool DEBUG = false;
  random_device seed;
  mt19937 mt(seed());
#endif

vector<vector<int>> distMatrix;
vector<pair<double,double>> parkInfo;
vector<pair<double,int>> destInfo;
vector<int> reverseParking;
vector<vector<int>> parkingFrom;
vector<vector<int>> destGroup(4); //制限時間ごとに配達先をまとめる 0:120分 1:240分 2:なし 3:All
map<string,int> intID;
map<int,string> stringID;
int parkCnt,destCnt,nodeCnt;


int addID(string key){
  int val = intID.size();
  intID[key] = val;
  stringID[val] = key;
  return val;
}

bool isParking(int id){
  return id < parkCnt;
}

void print(vector<int> route){
  cout << stringID[route[0]];
  for(int i=1; i<route.size(); i++){
    cout << (isParking(route[i]) ? "\n" : " ");
    cout << stringID[route[i]];
  }
  cout << endl;
}

bool allDelivered(vector<int> route){
  vector<bool> must(nodeCnt,false);
  for(int i=0; i<2; i++){
    for(auto v : destGroup[i]){
      must[v] = true;
    }
  }
  for(auto u : route) must[u] = false;
  for(auto f : must) if(f) return false;
  return true;
}

bool validate(vector<int>& route){
  //連続する駐車場(の左)を消す
  int i=0;
  while(i < route.size()-1){
    if(isParking(route[i]) && isParking(route[i+1])){
      route.erase(route.begin()+i);
    }else{
      i++;
    }
  }
  if(isParking(route[route.size()-1])) route.erase(route.end()-1);

  if(!allDelivered(route)) return false;

  auto tmp = route;
  sort(tmp.begin(), tmp.end());
  for(int i=0; i<tmp.size()-1; i++){
    if(!isParking(tmp[i]) && tmp[i] == tmp[i+1]) return false;
  }
  return true;
}

int simulate(vector<int> route, int query = 0){
  double time = 0; int cnt = 0, score = 0;
  int lp = route[0];
  int dist = 0;
  if(!isParking(route[0])) return -1;

  for(int i=1; i<route.size(); i++){
    int prev = route[i-1], cur = route[i];

    if(isParking(route[i])){
      if(distMatrix[lp][cur] == -1) return -5;
      time += (double)distMatrix[prev][lp] / 100 + (double)distMatrix[lp][cur] / 250;
      if(lp != cur) {
        time += parkInfo[lp].first;
        time += parkInfo[cur].second;
      }
      dist += distMatrix[prev][lp];

      lp = cur; cnt = 0;
    }else{
      time += (double)distMatrix[prev][cur] / 100 + destInfo[cur].first;
      cnt++; score += 10000;
      dist += distMatrix[prev][cur];
      int r = destInfo[cur].second;
      if(r != 0 && time > r) return -2;
      if(cnt > 10) return -3;
    }
    if(time > 240) return -4;
  }
  if(query == 0){
    return score + (240*60 - (int)(time*60));
  }else if(query == 1){
    return time*60;
  }else if(query == 2){
    return dist;
  }
}

int chooseNode(int from, vector<int> target, vector<bool>& visited, int sec, int type){
  int ans = -1;
  double minVal = 10000.0, minute = sec/60.0;

  for(auto v : target){
    if(visited[v]) continue;
    double r = destInfo[v].second, val = distMatrix[from][v];

    if(r != 0) {
      double rest = (r-minute)/r;
      if(rest < 0) return -1;
      if(type == 1) val -= r==240 ? 114514 : 810893;
      if(type == 2) val *= sqrt(rest);
      if(type == 3) val *= rest;
      if(type == 4) val *= rest*rest;
      if(type == 5) val *= rest*rest*rest;
    }

    if(val < minVal){ minVal = val; ans = v; }
  }

  if(ans != -1) visited[ans] = true;
  return ans;
}

int addDest(vector<int>& route, vector<int> target, vector<bool>& visited, int len, int ret, int type = 0){
  auto newRoute = route; auto newVisited = visited;
  int newNode = -1;
  for(int i=0; i<len; i++){
    int t = mt() % destCnt + parkCnt;

    if(mt()%10000 < 2000.0/(route.size()/8.0+i+1.0) && !newVisited[t]) {
      newNode = t;
      newVisited[newNode] = true;
    }else{
      newNode = chooseNode(route[route.size()-1], target, newVisited, ret, type);
    }

    if(newNode == -1) {
      return NO_NODE;
    }else{
      newRoute.push_back(newNode);
      ret = simulate(newRoute, 1);
      if(ret >= 0){
        route = newRoute; visited = newVisited;
      }else{
        return ret;
      }
    }
  }
  return ret;
}

vector<int> create(){
  vector<bool> visited(nodeCnt,false);
  vector<int> route;
  int ret=0, t=0, len;
  bool flg = true;
  int p = mt() % parkCnt;
  while(ret>=0){
    len = pow((1 + mt() % 99)/100.0, 0.08) * 11;
    if(flg && mt()%100 < 15){
      int nx = parkingFrom[p][mt() % parkingFrom[p].size()];
      p = nx; flg = false;
    }
    route.push_back(p);
    int r = mt() % 100;
    if(r < 75){ t = 0;}
    else if(r < 100) { t = 2;}
    ret = addDest(route, destGroup[3], visited, len, ret, t);
  }
  return route;
}

int main(){
  cin >> parkCnt >> destCnt;
  nodeCnt = parkCnt + destCnt;
  distMatrix = vector<vector<int>>(parkCnt+destCnt,vector<int>(parkCnt+destCnt));
  parkInfo = vector<pair<double,double>>(parkCnt);
  destInfo = vector<pair<double,int>>(parkCnt+destCnt);
  reverseParking = vector<int>(parkCnt, -1);
  parkingFrom = vector<vector<int>>(parkCnt);

  //駐車箇所入力
  string prev = "";
  for(int i=0; i<parkCnt; i++){
    string p; double in,out;
    cin >> p >> in >> out; //駐車箇所ID, 乗車時間, 降車時間
    int id = addID(p);
    parkInfo[id] = make_pair(in,out);
    if(prev.substr(0,4) == p.substr(0,4)){
      reverseParking[i] = i-1;
      reverseParking[i-1] = i;
    }
    prev = p;
  }

  //配達先入力
  for(int i=0; i<destCnt; i++){
    string d; double t; int r;
    cin >> d >> t >> r; //配達先ID, 滞在時間, 時間指定
    int id = addID(d);
    destInfo[id] = make_pair(t,r);
    int rid = r==120 ? 0 : (r==240 ? 1 : 2);
    destGroup[rid].push_back(id);
    destGroup[3].push_back(id);
  }

  //距離行列入力 : 駐車
  for(int i=0; i<parkCnt; i++){
    for(int j=0; j<parkCnt; j++){
      int d;
      cin >> d;
      distMatrix[i][j] = d;
    }
  }
  //距離行列入力 : 配達
  for(int i=0; i<destCnt; i++){
    for(int j=0; j<parkCnt+destCnt; j++){
      int d;
      cin >> d;
      distMatrix[i+parkCnt][j] = distMatrix[j][i+parkCnt] = d;
    }
  }

  //parkingFrom
  for(int i=0; i<parkCnt; i++){
    for(int j=0; j<parkCnt; j++){
      if(reverseParking[j] == -1){
        parkingFrom[i].push_back(j);
      }else if(reverseParking[j] == j+1){
        int t = distMatrix[i][j] < distMatrix[i][j+1] ? j : j+1;
        parkingFrom[i].push_back(t);
      }
    }
  }

  vector<int> ans;
  int maxScore = 0;
  vector<int> cnt;
  if(DEBUG) cnt = vector<int>(destCnt+1,0);
  while((double)(clock() - start) / CLOCKS_PER_SEC < 30){
    for(int n=0; n<10000; n++){
      vector<int> attempt;
      attempt = create();
      if(!validate(attempt)) break;

      int result = simulate(attempt);
      if(result > maxScore){
        ans = attempt;
        maxScore = result;
      }
      if(DEBUG) cnt[max(0,result/10000)]++;
    }
  }

  print(ans);
  if(DEBUG){
    cout << simulate(ans) << "\t";
    cout << simulate(ans, 2) << "\t";
    cout << validate(ans) << endl;
    cout << cnt << endl;
  }
  return 0;
}
