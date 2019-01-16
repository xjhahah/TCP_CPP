#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int main()
{
  unordered_map<string,string> dict;
  dict.insert(make_pair("hello","你好"));
  dict.insert(make_pair("bit","比特"));
  dict.insert(make_pair("XUST","西安科技大学"));
  dict.insert(make_pair("tiger","熊"));

  cout<<dict["XUST"]<<endl;
  for(auto it = dict.begin();it!=dict.end() ;++it)
  {
    cout<< it->first << " : "<< it->second <<endl;
  }
  return 0;
}
