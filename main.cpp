#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <iterator>

using namespace std;

struct City {
    int index;
    int vicinanza;
};
struct CompareHeight {
    bool operator() (City const& p1, City const& p2) {
        return p1.vicinanza > p2.vicinanza;
    }
};

struct retta {
    int m;
    int q;
    bool cattiva;
};

float intersezione(float m1, float q1, float m2, float q2){
    return abs((q2-q1)/(m1-m2));
}

int tmp_m;
int tmp_q;
int k;
int C;

City c;

int bfs_m;
int bfs_q;
int dij_m;
int dij_q;
bool bfs_cattiva = false;
bool dij_cattiva = false;

void fun_k_finder(
        map<int, vector<pair<int, int>>> &grafo,
        vector<int> &m,
        vector<int> &q,
        vector<bool> &cattivi,
        vector<bool> &cattivi_in,
        priority_queue<City, vector<City>, CompareHeight> &coda,
        map<int, retta> &k_found,
        int &last_dij_m,
        int &last_dij_q) {

    // calcolo dijkstra sul k maggiore di k_finder
    auto last_k = k_found.rbegin();
    k = last_k->first;
    coda.emplace( City {0, 0});
    m.assign(m.size(), -1);
    q.assign(q.size(), 0);
    m[0] = 0;

    for (int i = 0; i < cattivi_in.size(); ++i)
        if (cattivi_in[i]) cattivi[i] = true;

    while (!coda.empty()) { // dijkstra con propagazione della cattiveria
        c = coda.top();
        for (auto j : grafo[c.index]) {
            tmp_m = m[c.index] + 1;
            tmp_q = c.vicinanza + j.second + k;

            if (m[j.first] == -1 || q[j.first] > tmp_q) { // mai visitato o visitato con q superiore
                m[j.first] = tmp_m;
                q[j.first] = tmp_q;
                cattivi[j.first] = cattivi_in[j.first] || cattivi[c.index];
                coda.emplace( City{j.first, tmp_q} );
            } else if (q[j.first] == tmp_q && m[j.first] >= tmp_m) {
                m[j.first] = tmp_m;
                q[j.first] = tmp_q; // inutile se uguale
                if (m[j.first] == tmp_m) {
                    cattivi[j.first] = cattivi[j.first] || cattivi[c.index];
                } else {
                    cattivi[j.first] = cattivi_in[j.first] || cattivi[c.index];
                }
                coda.emplace( City{j.first, tmp_q} );
            }
        }
        coda.pop();
    }

    // normalizzo q
    q[C - 1] = q[C - 1] - m[C - 1] * k;

    // se trovo retta con stessi valori della precedente dijkstra --> esco
    if (m[C - 1] == last_dij_m && q[C - 1] == last_dij_q || m[C - 1] == bfs_m && q[C - 1] == bfs_q) return;

    // ho trovato un nuovo k
    // aggiorno m e q nel k appena superato
    k_found.erase(last_k->first);
    k_found[k] = {m[C - 1], q[C - 1], cattivi[cattivi.size() - 1]};

    // faccio intersezione con bfs e nuovi m e q
    k = (int)intersezione((float)m[C - 1], (float)q[C - 1], (float)bfs_m, (float)bfs_q);
    k_found[k] = {bfs_m, bfs_q, bfs_cattiva};

    // itero per cercare nuovi k
    last_dij_m = m[C - 1];
    last_dij_q = q[C - 1];

    fun_k_finder(grafo, m, q, cattivi, cattivi_in, coda, k_found, last_dij_m, last_dij_q);

}

void connect(
        map<int, vector<pair<int, int>>> &grafo,
        vector<int> &m,
        vector<int> &q,
        vector<bool> &cattivi,
        vector<bool> &cattivi_in,
        priority_queue<City, vector<City>, CompareHeight> &coda,
        map<int, retta> &k_found,
        map<int, retta>::iterator &A,
        map<int, retta>::iterator &B,
        int &kOut) {

    if ( kOut == -3 ) {
        k = (int)intersezione((float)A->second.m, (float)A->second.q, (float)B->second.m, (float)B->second.q);

        if (k == B->first) {
            // se corrisponde è già connesso
            if (!A->second.cattiva) {
                kOut = k;
            }
        } else {
            coda.emplace( City {0, 0});
            m.assign(m.size(), -1);
            q.assign(q.size(), 0);
            m[0] = 0;

            for (int i = 0; i < cattivi_in.size(); ++i) {
                if (cattivi_in[i]) {
                    cattivi[i] = true;
                }
            }

            while (!coda.empty()) {
                c = coda.top();
                for (auto [adj, peso] : grafo[c.index]) {
                    tmp_m = m[c.index] + 1;
                    tmp_q = c.vicinanza + peso + k;

                    if (m[adj] == -1 || q[adj] > tmp_q) {
                        m[adj] = tmp_m;
                        q[adj] = tmp_q;
                        cattivi[adj] = cattivi_in[adj] || cattivi[c.index];
                        coda.emplace( City{adj, tmp_q} );
                    } else if (q[adj] == tmp_q && m[adj] >= tmp_m) {
                        m[adj] = tmp_m;
                        q[adj] = tmp_q;
                        if (m[adj] == tmp_m) {
                            cattivi[adj] = cattivi[adj] || cattivi[c.index];
                        } else {
                            cattivi[adj] = cattivi_in[adj] || cattivi[c.index];
                        }
                        coda.emplace( City{adj, tmp_q} );
                    }
                }
                coda.pop();
            }

            // normalizzo q
            q[C - 1] = q[C - 1] - m[C - 1] * k;

            map<int, retta>::iterator X;
            if (m[C - 1] == A->second.m || m[C - 1] == B->second.m){
                k_found[k] = {m[C - 1], q[C - 1], cattivi[C - 1]};
                X = k_found.find(k);
                k_found.erase(B->first);
                if (!X->second.cattiva) kOut = k;
            } else {
                k_found[k] = {m[C - 1], q[C - 1], cattivi[cattivi.size() - 1]};
                X = k_found.find(k);
                connect(grafo, m, q, cattivi, cattivi_in, coda, k_found, X, B, kOut);
                connect(grafo, m, q, cattivi, cattivi_in, coda, k_found, A, X, kOut);
            }
        }
    }
}

int main() {
    ifstream in ("input.txt");
    ofstream out ("output.txt");

    map<int, vector<pair<int, int>>> grafo;

    int S,a,b,w,M;
    in >> C >> S;
    for (int i = 0; i < S; ++i) {
        in >> a >> b >> w;
        grafo[a].push_back({b, w});
        grafo[b].push_back({a,w});
    }

    vector<bool> cattivi_input(C, false);
    vector<bool> cattivi(C, false);
    in >> M;
    for (int i = 0; i < M; ++i) {
        in >> a;
        cattivi_input[a] = true;
        cattivi[a] = true;
    }

    //--------------------------------- fine input ---------------------------------

    priority_queue<City, vector<City>, CompareHeight> coda;
    vector<int> m(C, -1);
    vector<int> q(C, -1);
    vector<int> provenienza (C);

    //--------------------------------- dijkstra ---------------------------------
    m[0] = 0;
    q[0] = 0;
    coda.emplace( City {0, 0});

    while (!coda.empty()) {
        c = coda.top();
        for (auto [adj, cost] : grafo[c.index]) {
            tmp_m = m[c.index] + 1;
            tmp_q = c.vicinanza + cost;

            if (m[adj] == -1 || q[adj] > tmp_q) {
                m[adj] = tmp_m;
                q[adj] = tmp_q;
                cattivi[adj] = cattivi_input[adj] || cattivi[c.index];
                provenienza[adj] = c.index;
                coda.emplace( City{adj, tmp_q} );
            } else if (q[adj] == tmp_q && m[adj] >= tmp_m) {
                m[adj] = tmp_m;
                q[adj] = tmp_q;
                if (m[adj] == tmp_m) {
                    cattivi[adj] = cattivi[adj] || cattivi[c.index];
                } else {
                    cattivi[adj] = cattivi_input[adj] || cattivi[c.index];
                }
                provenienza[adj] = c.index;
                coda.emplace( City{adj, tmp_q} );
            }
        }
        coda.pop();
    }

    dij_m = m[C - 1];
    dij_q = q[C - 1];
    dij_cattiva = cattivi[C - 1];

    vector<int> min_path;
    int temp = C - 1;
    while (temp != 0) {
        min_path.push_back(temp);
        temp = provenienza[temp];
    }
    min_path.push_back(0);

    //--------------------------------- bfs ---------------------------------
    queue<int> coda2;
    coda2.push(0);
    m.assign(C, -1);
    q.assign(C, 0);
    m[0] = 0;

    cattivi.assign(C, false);
    for (int i = 0; i < C; ++i)
        if (cattivi_input[i]) cattivi[i] = true;

    int c;
    while (!coda2.empty()) {
        c = coda2.front();
        for (auto [adj, peso] : grafo[c]) {
            tmp_m = m[c] + 1;
            tmp_q = q[c] + peso;

            if (m[adj] == -1 || m[adj] > tmp_m) {
                m[adj] = tmp_m;
                q[adj] = tmp_q;
                cattivi[adj] = cattivi_input[adj] || cattivi[c];
                coda2.push(adj);
            } else if (m[adj] == tmp_m && q[adj] >= tmp_q) {
                q[adj] = tmp_q;
                if (q[adj] == tmp_q) {
                    cattivi[adj] = cattivi[adj] || cattivi[c];
                } else {
                    cattivi[adj] = cattivi_input[adj] || cattivi[c];
                }
                coda2.push(adj);
            }
        }
        coda2.pop();
    }

    //ottengo il cammino più breve e se incontra un cattivo, in caso -1
    bfs_m = m[C - 1];
    bfs_q = q[C - 1];
    bfs_cattiva = cattivi[C - 1];

    //------------------------ dijkstra - cammini candidati --------------------------

    vector<bool> visited (C);
    map<int, retta> k_found;

    int kOut = -3;

    if ( M == 0) {
        cout << "M = 0\n";
        kOut = -1;
    } else if (M == C-2) {
        cout << "M = C-2\n";
        kOut = -2;
    } else {
        // se sono stessa retta e una è cattiva --> -2
        if (dij_m == bfs_m && (dij_cattiva || bfs_cattiva)) {
            kOut = -2;
        } else if (!bfs_cattiva) {
            kOut = -1;
        } else {
            k_found[0] = {dij_m, dij_q, dij_cattiva};
            k_found[(int)intersezione((float)dij_m, (float)dij_q, (float)bfs_m, (float)bfs_q)] = {bfs_m, bfs_q, bfs_cattiva};

            int last_dij_m = dij_m;
            int last_dij_q = dij_q;
            // partendo da ultimo k cerco se c'è una dijkstra migliore che mi troverà nuovi k, maggiori dei precedenti
            // mi fermo quando non trovo nuovi k
            fun_k_finder(grafo, m, q, cattivi, cattivi_input, coda, k_found, last_dij_m, last_dij_q);

            //-------------------------- ricerca k ---------------------------------

            // prelevo penultima chiave della map
            auto A = prev(k_found.end());
            auto B = k_found.end();
            //finche non sono alla prima chiave della map
            // or
            // finche non trovo kOut
            do {
                A = prev(A, 1);
                B = prev(B, 1);
                connect(grafo, m, q, cattivi, cattivi_input, coda, k_found, A, B, kOut);
            } while (A->first != 0 && kOut == -3);
        }
    }

    if (kOut == -3) kOut = -2;

    out << kOut << endl;
    out << dij_m +1 << endl;

    for (int i = dij_m ; i >= 0 ; i--)
        out << min_path[i] << " ";

    return 0;
}
