#include <bits/stdc++.h>

#define ff first
#define ss second
#define pb push_back
#define mp make_pair
#define SQ(a) (a)*(a)
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#define mod0 998244353
#define mod1 1000000007
#define f(i,s,e) for(ll i=s;i<e;i++)
#define fall(x,v) for(auto x:v)
#define f_(i,s,e) for(ll i=s;i>=e;i--)

using namespace std;

// #include <ext/pb_ds/assoc_container.hpp>
// #include <ext/pb_ds/tree_policy.hpp>

// typedef __gnu_pbds::tree<int, __gnu_pbds::null_type, less<int>, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update> ordered_set;

typedef long long ll;
typedef vector<int> vi;
typedef vector<long long> vll;
typedef pair<long long,long long> pl;
typedef pair<int,int> pi;

ll binpow(ll a, ll b, ll m) {
    a %= m;
    ll res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a % m;
        a = a * a % m;
        b >>= 1;
    }
    return res;
}

ll gcd(ll x,ll y){
    if(y==0) return x;
    return gcd(y,x%y);
}

void solve(){
	int n,k; cin>>n>>k;
	vector<vi> adj(n);
	f(i,0,n-1){
		int u,v; cin>>u>>v;
		adj[--v].pb(--u);
		adj[u].pb(v);
	}
	
	
	auto good = [&](int m){
		int res = 0;
		auto dfs = [&](auto self,int v,int f)-> int{
			int sz = 1;
			
			fall(&u,adj[v]){
				if(u==f)continue;
				
				sz+=self(self,u,v);
			}
			
			if(sz>=m){
				res++, sz = 0;
			}
			
			return sz;	
		};
		
		dfs(dfs,0,-1);
		return (res>k ? true : false);
	};
	
	int l = 0, r = n;
	while(r-l>1){
		int mid = (l+r)/2;
		if(good(mid)) l = mid;
		else r = mid;
	}
	
	cout<<l<<'\n';
}

int main(){
    ios::sync_with_stdio(0);cin.tie(NULL);

    int t=1;
    cin>>t;
    for (int i = 0; i < t; ++i)
    {
        solve();
    }
    return 0;
}