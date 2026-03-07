#include <bits/stdc++.h>
using namespace std;

#ifdef ONPC
#include "debug_utils.h"
#else
#define dbg(...)
#endif

//#define int long long
bool testcases = false;

void solve(int testcase) {
  string line;

    while(getline(cin, line)) {
        cout << line << endl;
    }

}


signed main() {
  ios::sync_with_stdio(0);
  cin.tie(0);

  int t = 1, testcase = 1;
  if(testcases) cin >> t;
  while (t--) {
    solve(testcase); testcase++;
  }
  return 0;
}
