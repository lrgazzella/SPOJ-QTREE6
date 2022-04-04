#include <bits/stdc++.h>

using namespace std;
const int N = 100001;


// chains = numero di catene in uno specifico momento
// chain[u] = numero della catena di cui il nodo u fa parte
int chain[N], chains, n, q, a, b;
int id[N], pos[N], par[N], blackarr[N], whitearr[N], ptr;
int weight[N], head[N], heavy[N], c[N]; // nota che ci possono essere tante catene quanti sono i nodi
vector<int> adj[N];
// std::set is a container that contains an ordered set of objects. 
// The ordering of the objects is determined by the second argument. 
// In questo caso stiamo quindi ordinando dal più grande al più piccolo (grazie a greater)
// tree[0][i] = i-esima catena dell'albero relativo ai neri. 
set<int, greater<int> > tree[2][N]; 
set<int, greater<int> >::iterator it;

struct SegmentTree {
	#define M ((L + R) >> 1)
	#define P (p << 1)
	int st[N << 2], lazy[N << 2];

	void push(int p, int L, int R) { 
		if (lazy[p] != 0) {
			st[p] += lazy[p];
			if (L != R) {
				lazy[P] += lazy[p];
				lazy[P + 1] += lazy[p];
			}
			lazy[p] = 0;
		}
	}
	void pull(int p) { // Aggiorna il valore del padre con i valori che prende dai due figli
		st[p] = st[P] + st[P + 1];
	}
	void build(int arr[], int p = 1, int L = 0, int R = n - 1) {
		lazy[p] = 0;
		if (L == R) {
			st[p] = arr[L];
			return;
		}
		build(arr, P, L, M);
		build(arr, P + 1, M + 1, R);
		pull(p);
	}
	int query(int a, int p = 1, int L = 0, int R = n - 1) {
        int b = a;
		push(p, L, R); // Controllo se devo propagare un valore del nodo corrente (Questo perchè potrebbe servire nell'if sotto)
		
		if (a < L || R < a) // se l'elemento che stiamo cercando non è incluso nel range => non contiamo nulla, ritorniamo 0
			return 0;
		if (a <= L && R <= a) {
			return st[p]; 
        }
		int q = query(a, P, L, M) + query(a, P + 1, M + 1, R); 
		pull(p); // Le query sopra potrebbero aggiornare i valori dei figli. Ecco perchè devo fare la pull.
        return q;
	}
	
	void updateUp(int u, int an, int v) { 
		while (chain[u] != chain[an]) {
			update(pos[head[chain[u]]], pos[u], v);
			u = par[head[chain[u]]];
		}
		update(pos[an], pos[u], v);
	}
	void update(int a, int b, int v, int p = 1, int L = 0, int R = n - 1) { 
		if (b < a) swap(a, b);
		push(p, L, R);
		if (b < L || R < a) return; 
		if (a <= L && R <= b) {
			lazy[p] += v;
			push(p, L, R);
			return;
		}
		update(a, b, v, P, L, M);
		update(a, b, v, P + 1, M + 1, R);
		pull(p);
	}
    void print_st(){
        for(int i=0; i<30; i++)
            cout << st[i] << " |";
		cout << "\n";
    }

} black, white;

// Calcola: array weight, array heavy, array par
void dfsHeavy(int u) {
	// Nodo corrente ha peso 1. Nota che il peso dei nodi sarà quello con cui verrà inizializzato il basearray del ST.
	// Il basearray contiene, per ogni nodo, la dimensione del sottoalbero nero/bianco.
	// Notare però che all'inizio tutti i nodi sono neri, quindi il basearray sarà semplicemente il numero di figli
	// Weight rappresenta la dimensione del sottoalbero (incluso se stesso)	
	weight[u] = 1; 
	heavy[u] = -1; // heavy[u] = 1 sse u è un heavy node
	for (int i = 0; i < adj[u].size(); ++i) {
		int v = adj[u][i];
		if (par[u] == v) continue;
		par[v] = u;
		dfsHeavy(v);
		weight[u] += weight[v];
		if (heavy[u] == -1 || weight[heavy[u]] < weight[v])
			heavy[u] = v;
	}
}

void dfsLight(int u) {
	if (head[chains] == -1) {
		tree[0][chains].clear(); // Clear: Removes all elements from the set container (which are destroyed), leaving the container with a size of 0
		tree[1][chains].clear(); 
		head[chains] = u; // è il primo nodo che metto nella chain corrente 
	}
	chain[u] = chains; // La catena del nodo corrente è quella identificata da chains, ovvero il numero di catene correnti
	pos[u] = ptr; // pos[u] = posizione di u nell'albero
	id[ptr] = u; // id è un array che, data una posizione nell'albero, ci restituisce l'id del nodo associato
	blackarr[ptr] = weight[u]; // come detto nella dfsHeavy
	whitearr[ptr] = 0;
	tree[0][chains].insert(ptr); // Inserisce la posizione (ptr) nell'albero. Notare che la inserisce in posizione 0, ovvero quella corrispondente all'albero nero. Ovviamente lo inseriamo nella catena corrente.
	ptr++; // dato che abbiamo inserito il nodo corrente, incrementiamo la posizione che occuperà il prossimo nodo
	if (heavy[u] != -1) // se il nodo è heavy chiamiamo dfsLight in modo da finire la catena
		dfsLight(heavy[u]);
	for (int i = 0; i < adj[u].size(); ++i) { // una volta finita la catena, inizio tante catene per ogni normal child e richiamo dfsLight
		int v = adj[u][i];
		if (v == par[u] || v == heavy[u])
			continue;
		chains++;
		dfsLight(v);
	}
}

void heavyLight(int root) {
	// parent[i] = parent del nodo i
	// head[i] = primo nodo della catena numero i.
	for (int i = 0; i < n; ++i) // tutti i nodi hanno parent = -1. 
		head[i] = par[i] = -1; // Non ci sono ancora catene, mettiamo un valore fittizio come primo nodo di ogni catena
	chains = ptr = 0; // abbiamo appena iniziato a fare HLD, il numero di catene è 0
	dfsHeavy(root); 
	dfsLight(root);
	black.build(blackarr);
	white.build(whitearr);
	chains++;
}

int ancestor(int u) { // prende un nodo e scorre verso l'alto fino a prendere il primo nodo con il colore diverso da quello di u
	while (u != -1) {
		it = tree[!c[u]][chain[u]].lower_bound(pos[u]);
		if (it != tree[!c[u]][chain[u]].end())
			return id[*it + 1];
		int h = head[chain[u]];
		if (par[h] == -1 || c[h] != c[par[h]])
			return h;
		u = par[head[chain[u]]];
	}
	return id[0];
}

void toggle(int u) {
	
	SegmentTree& prev = (c[u] ? white : black); // prev è un alias per quello che torna l'if in linea
	SegmentTree& next = (c[u] ? black : white);

	tree[c[u]][chain[u]].erase(pos[u]); // il nodo u sta cambiando colore, lo tolgo dall'albero relativo al vecchio colore
	int an = ancestor(u); // primo nodo nell'albero con colore diverso da u
	int q = prev.query(pos[u]); // prende la dimensione del sottoalbero nero/bianco, compreso se stesso

	if (par[an] != -1) an = par[an]; 
	prev.update(pos[u], pos[u], -1); 

	if (par[u] != -1)
		prev.updateUp(par[u], an, -q); // come da definizione faccio -q a tutti i nodi tra il padre di u e an

	c[u] ^= 1; // xor, cambia colore al nodo

	tree[c[u]][chain[u]].insert(pos[u]); // inserisco il nodo u all'interno dell'albero relativo al nuovo colore
	an = ancestor(u); // Prendo il primo nodo nell'albero con colore diverso da quello nuovo
	q = next.query(pos[u]); // prendo il valore nel nuovo ST
	if (par[an] != -1) an = par[an];
	next.update(pos[u], pos[u], 1);
	if (par[u] != -1)
		next.updateUp(par[u], an, q + 1); // come da definizione
}

void print_arr(int *arr, int num){
	cout << "\n";
    for(int i=0; i<num; i++)
            cout << arr[i] << " |";
	cout << "\n";
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	cin >> n;
	for (int i = 1; i < n; ++i) {
		cin >> a >> b;
		a--, b--; 
		adj[a].push_back(b);
		adj[b].push_back(a);
	}
	heavyLight(0);
		
	cin >> q;
	while (q-- > 0) {
		cin >> a >> b; // a contiene l'operazione, b il nodo
		b--;
		if (a){
			toggle(b);
		}else {
			int an = ancestor(b);
			int qq = (c[b] ? white : black).query(pos[an]); // notare che prende l'ancestor
			cout << qq << "\n";
		}
	}
	cout << flush;
}
