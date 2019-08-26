#include "bits/stdc++.h"
#include "random_device.hpp"

using namespace std;
template <class T>ostream &operator<<(ostream &o,const vector<T>&v)
{o<<"[";for(int i=0;i<(int)v.size();i++)o<<(i>0?", ":"")<<v[i];o<<"]\n";return o;}
template <class T, class U>ostream &operator<<(ostream &o, const pair<T, U>&p)
{o<<"{"<<p.first<<", "<<p.second<<"}";return o;}
template <class T, class U>ostream &operator<<(ostream &o, const map<T, U>&obj)
{o<<"{";for(auto itr=obj.begin();itr!=obj.end();++itr)o<<(itr!=obj.begin()?", ":"")<<*itr;o<<"}";return o;}
void print() {}
template <class H, class... T>void print(H&& h, T&&... t)
{cout << h << (sizeof...(t)==0 ? "\n" : " "); print(forward<T>(t)...);}


using seed_v_t = std::array<cpprefjp::random_device::result_type, sizeof(std::mt19937)/sizeof(cpprefjp::random_device::result_type)>;
seed_v_t create_seed_v()
{
  cpprefjp::random_device rnd;
  seed_v_t sed_v;
  std::generate(sed_v.begin(), sed_v.end(), std::ref(rnd));
  return sed_v;
}
std::mt19937 create_random_engine()
{
  const auto sed_v = create_seed_v();
  std::seed_seq seq(sed_v.begin(), sed_v.end());
  return std::mt19937(seq);
}
std::mt19937& random_engine()
{
  static thread_local std::mt19937 engine = create_random_engine();
  return engine;
}
