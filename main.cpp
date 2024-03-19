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

int n_dij;

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

    cout << "-------------- Faccio Dijkstra ------------- numero: " << ++n_dij << endl;

    // calcolo dijkstra sul k maggiore di k_finder
    auto last_k = k_found.rbegin();
    k = last_k->first;
    coda.emplace( City {0, 0});
    m.assign(m.size(), -1);
    q.assign(q.size(), 0);
    m[0] = 0;

    for (int i = 0; i < cattivi_in.size(); ++i) {
        if (cattivi_in[i]) cattivi[i] = true;
    }

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
    cout << "k : " << k << " m : " << m[C - 1] << " q : " << q[C - 1] << endl;
    cout << "last_k : " << last_k->first << " m : " << last_k->second.m << " q : " << last_k->second.q << endl;

    // se trovo retta con stessi valori della precedente dijkstra --> esco
    if (m[C - 1] == last_dij_m && q[C - 1] == last_dij_q || m[C - 1] == bfs_m && q[C - 1] == bfs_q) {
        return;
    }


    // ho trovato un nuovo k
    // aggiorno m e q nel k appena superato
    cout << "elimino dalla mappa last_k : " << last_k->first << endl;
    k_found.erase(last_k->first);
    cout <<"aggiungo k : " << k << " m : " << m[C - 1] << " q : " << q[C - 1] << " cattivo : " << cattivi[cattivi.size() - 1] << endl;
    k_found[k] = {m[C - 1], q[C - 1], cattivi[cattivi.size() - 1]};

    // faccio intersezione con bfs e nuovi m e q
    cout << "trovo intersezione nuovo last_k\n";
    k = (int)intersezione((float)m[C - 1], (float)q[C - 1], (float)bfs_m, (float)bfs_q);
    cout << "sposto retta bfs che parte da : " << k << " con m : " << bfs_m << " q : " << bfs_q << " cattivo : " << bfs_cattiva << endl;
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
        map<int, retta> &k_finded,
        map<int, retta>::iterator &A,
        map<int, retta>::iterator &B,
        int &kOut) {

    if ( kOut == -3 ) {
        cout << "-------------- Faccio Dijkstra ------------- numero: " << ++n_dij << endl;

        cout << "connect of A : " << A->first << " B : " << B->first << endl;
        k = (int)intersezione((float)A->second.m, (float)A->second.q, (float)B->second.m, (float)B->second.q);
        cout << "A : " << A->first << " m : " << A->second.m << " q : " << A->second.q << " cattivo : " << A->second.cattiva << endl;
        cout << "B : " << B->first << " m : " << B->second.m << " q : " << B->second.q << " cattivo : " << B->second.cattiva << endl;

        if (k == B->first) {
            // se corrisponde è già connesso
            cout << "x : " << k << endl;
            cout << "A-B already connesse\n";
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
                for (auto i : grafo[c.index]) {
                    tmp_m = m[c.index] + 1;
                    tmp_q = c.vicinanza + i.second + k;

                    if (m[i.first] == -1 || q[i.first] > tmp_q) {
                        m[i.first] = tmp_m;
                        q[i.first] = tmp_q;
                        cattivi[i.first] = cattivi_in[i.first] || cattivi[c.index];
                        coda.emplace( City{i.first, tmp_q} );
                    } else if (q[i.first] == tmp_q && m[i.first] >= tmp_m) {
                        m[i.first] = tmp_m;
                        q[i.first] = tmp_q;
                        if (m[i.first] == tmp_m) {
                            cattivi[i.first] = cattivi[i.first] || cattivi[c.index];
                        } else {
                            cattivi[i.first] = cattivi_in[i.first] || cattivi[c.index];
                        }
                        coda.emplace( City{i.first, tmp_q} );
                    }
                }
                coda.pop();
            }

            // normalizzo q
            q[C - 1] = q[C - 1] - m[C - 1] * k;
            cout << "x : " << k << " m : " << m[C - 1] << " q : " << q[C - 1] << " cattiva : " << cattivi[C - 1] << endl;

            map<int, retta>::iterator X;
            if (m[C - 1] == A->second.m || m[C - 1] == B->second.m){
                k_finded[k] = {m[C - 1], q[C - 1], cattivi[C - 1]};
                X = k_finded.find(k);
                k_finded.erase(B->first);
                if (!X->second.cattiva) {
                    cout << " provA\n";
                    kOut = k;
                }
            } else {
                k_finded[k] = {m[C - 1], q[C - 1], cattivi[cattivi.size() - 1]};
                X = k_finded.find(k);
                connect(grafo, m, q, cattivi, cattivi_in, coda, k_finded, X, B, kOut);
                connect(grafo, m, q, cattivi, cattivi_in, coda, k_finded, A, X, kOut);
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

    vector<bool> cattivi_in(C, false);
    vector<bool> cattivi(C, false);
    in >> M;
    for (int i = 0; i < M; ++i) {
        in >> a;
        cattivi_in[a] = true;
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
        for (auto i : grafo[c.index]) {
            tmp_m = m[c.index] + 1;
            tmp_q = c.vicinanza + i.second;

            if (m[i.first] == -1 || q[i.first] > tmp_q) {
                m[i.first] = tmp_m;
                q[i.first] = tmp_q;
                cattivi[i.first] = cattivi_in[i.first] || cattivi[c.index];
                provenienza[i.first] = c.index;
                coda.emplace( City{i.first, tmp_q} );
            } else if (q[i.first] == tmp_q && m[i.first] >= tmp_m) {
                m[i.first] = tmp_m;
                q[i.first] = tmp_q;
                if (m[i.first] == tmp_m) {
                    cattivi[i.first] = cattivi[i.first] || cattivi[c.index];
                } else {
                    cattivi[i.first] = cattivi_in[i.first] || cattivi[c.index];
                }
                provenienza[i.first] = c.index;
                coda.emplace( City{i.first, tmp_q} );
            }
        }
        coda.pop();
    }

    dij_m = m[C - 1];
    dij_q = q[C - 1];
    dij_cattiva = cattivi[C - 1];

    cout << "dij_m : " << dij_m << " dij_q : " << dij_q << endl;

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
    for (int i = 0; i < C; ++i) {
        if (cattivi_in[i]) {
            cattivi[i] = true;
        }
    }

    int c;
    while (!coda2.empty()) {
        c = coda2.front();
        for (auto i: grafo[c]) {
            tmp_m = m[c] + 1;
            tmp_q = q[c] + i.second;

            if (m[i.first] == -1 || m[i.first] > tmp_m) {
                m[i.first] = tmp_m;
                q[i.first] = tmp_q;
                cattivi[i.first] = cattivi_in[i.first] || cattivi[c];
                coda2.push(i.first);
            } else if (m[i.first] == tmp_m && q[i.first] >= tmp_q) {
                q[i.first] = tmp_q;
                if (q[i.first] == tmp_q) {
                    cattivi[i.first] = cattivi[i.first] || cattivi[c];
                } else {
                    cattivi[i.first] = cattivi_in[i.first] || cattivi[c];
                }
                coda2.push(i.first);
            }
//            cout << "citta : " << c << " m : " << m[i.first] << " q : " << q[i.first] << endl;
        }
        coda2.pop();
    }

    //ottengo il cammino più breve e se incontra un cattivo, in caso -1
    bfs_m = m[C - 1];
    bfs_q = q[C - 1];
    bfs_cattiva = cattivi[C - 1];
    cout << "bfs_m : " << bfs_m << " bfs_q : " << bfs_q << endl;

    //------------------------ dijkstra - cammini candidati --------------------------

    vector<bool> visited (C);
    map<int, retta> k_found;

    int kOut = -3;

    cout << "dij cattiva : " << dij_cattiva << endl;
    cout << "bfs cattiva : " << cattivi[C - 1] << endl;

    cout << endl;

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
            fun_k_finder(grafo, m, q, cattivi, cattivi_in, coda, k_found, last_dij_m, last_dij_q);


            cout << endl << "Elementi in k_finded : " << k_found.size() << endl;
            for (auto i : k_found) {
                cout << "k : " << i.first << " m : " << i.second.m << " q : " << i.second.q << " cattivo : " << i.second.cattiva << endl;
            }

            cout << "\ndijkstra-search finished\n";

            //-------------------------- rigerca k ---------------------------------
            cout << "\n\nstudio dei casi\n";

            // prelevo penultima chiave della map
            auto A = prev(k_found.end());
            auto B = k_found.end();
            //finche non sono alla prima chiave della map
            // or
            // finche non trovo kOut
            int x;
            do {
                A = prev(A, 1);
                B = prev(B, 1);
                connect(grafo, m, q, cattivi, cattivi_in, coda, k_found, A, B, kOut);
            } while (A->first != 0 && kOut == -3);

        }
    }

    if (kOut == -3) {
        kOut = -2;
    }

    out << kOut << endl;
    out << dij_m +1 << endl;

    for (int i = dij_m ; i >= 0 ; i--) {
        out << min_path[i] << " ";
    }

    cout << "-------------- fine Dijkstra ------------- numero: " << n_dij << endl;

    return 0;
}
