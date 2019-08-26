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

int rand(int min, int max){
  return mt() % (max-min+1) + min;
}

bool chance(int percent){
  return rand(0,99) < percent;
}

template<class T>
class simulated_annealing{
  //varibale
  int temp = 100000;
  pair<T,int> ans;

  //constructor
  simulated_annealing(){}
  
  //function
  int eval(){
    return 0;
  }

  bool accept(){
    return chance(10);
  }

  T neighbor(T ans){
    return ans;
  }
  
  public:
  T exec(){
    for(int i=0; i<10000; i++){
      
    }
    return;
  }
};

int main(){
  return 0;
}