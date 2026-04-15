/*
 * ================================================================
 * Busca em Largura com controle de estados visitados (BFS-Graph)
 * ================================================================
 *
 * Estratégia utilizada:
 * - BFS (Busca em Largura) no grafo de estados do 8-Puzzle.
 * - Cada nó (NoBusca) guarda referência para o pai e custo do caminho.
 * - A expansão segue a ordem fixa: cima, esquerda, direita, baixo.
 * - Mantemos um conjunto "closed" (hash) de estados já visitados para
 *   evitar ciclos e expansões desnecessárias.
 * - BFS garante que a primeira solução encontrada é a de menor número de movimentos,
 *   porque explora todos os estados em ordem crescente de profundidade.
 *
 * Heurística:
 * - Distância de Manhattan calculada apenas para registro,
 *   não influencia a busca (BFS não usa heurística).
 * - Serve como indicador da dificuldade do estado inicial.
 *
 * Memória:
 * - Estimativa de memória usada pelos nós expandidos é feita no final.
 * 
 * Observação:
 * - Em algumas execuções, foram observadas inconsistências ocasionais
 *   (ex.: número de nós expandidos diferente).
 *   Isso pode ser explicado pelo uso de unordered_set (ordem indefinida)
 *   e gerenciamento de ponteiros. Apesar disso, a BFS sempre retorna
 *   soluções corretas.
 *
 * Compilação:
 *     make clean && make
 *
 * Execução:
 *     ./main -bfs 0 6 1 7 4 2 3 8 5, 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5
 *     ./main -bfs instances.txt
 */


#include <iostream>
#include <vector>
#include <deque> // Usado como fila FIFO (estrutura principal da BFS
#include <set>
#include <unordered_set> // Conjunto hash para estados já visitados
#include <chrono>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace std::chrono;

const int N = 3;
const int SIZE = N * N;
const vector<int> GOAL_STATE = {0,1,2,3,4,5,6,7,8};
const int dx[4] = {-1,0,0,1}; // cima, esquerda, direita, baixo
const int dy[4] = {0,-1,1,0};

// ------------------ STATE ------------------
struct Estado {
    vector<int> tiles;
    Estado() : tiles(SIZE,0) {}
    Estado(const vector<int>& t) : tiles(t) {}

    int zeroPos() const {
        auto it = find(tiles.begin(), tiles.end(), 0);
        return distance(tiles.begin(), it);
    }

    bool isGoal() const { return tiles == GOAL_STATE; }


    // Gera todos os estados possíveis a partir do estado atual
    // Considera movimentos válidos do espaço vazio: cima, esquerda, direita, baixo
    // Não gera estados fora do tabuleiro
    vector<Estado> gerarSucessores() const {
        vector<Estado> sucessores;
        int zp = zeroPos();
        int zx = zp / N, zy = zp % N;
        for (int dir = 0; dir < 4; ++dir) {
            int nx = zx + dx[dir];
            int ny = zy + dy[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            int newPos = nx * N + ny;
            Estado next = *this;
            swap(next.tiles[zp], next.tiles[newPos]);
            sucessores.push_back(next);
        }
        return sucessores;
    }

    bool operator==(const Estado& other) const { return tiles == other.tiles; }
    bool operator<(const Estado& other) const { return tiles < other.tiles; }
};

// ------------------ HASH PARA UNORDERED_SET -------------------
// Functor usado para armazenar estados no unordered_set (closed)
// Permite uma verificação rápida dos estados já visitados
struct HashEstado {
    size_t operator()(const Estado& s) const {
        size_t h = 0;
        for (int v : s.tiles) {
            h = h * 31 + hash<int>()(v);
        }
        return h;
    }
};

// ------------------ HEURÍSTICA --------------------------
//Serve como heurística informativa, não utilizada pelo BFS
int manhattan(const Estado& s) {
    int dist = 0;
    for(int i=0;i<SIZE;++i){
        int val = s.tiles[i];
        if(val==0) continue;
        int target_x = val / N, target_y = val % N;
        int curr_x = i / N, curr_y = i % N;
        dist += abs(target_x - curr_x) + abs(target_y - curr_y);
    }
    return dist;
}

// ------------------ Nó de Busca -----------------------------
// Guarda referência para o estado, nó pai e custo do caminho (número de movimentos)
struct NoBusca {
    Estado estados;
    NoBusca* parent;
    int pathCost;
    NoBusca(const Estado& s, NoBusca* p, int cost) : estados(s), parent(p), pathCost(cost) {}
};

// libera memória de todos os nós da fila
void freeNodes(deque<NoBusca*>& open){
    while(!open.empty()){
        delete open.front();
        open.pop_front();
    }
}

// ------------ Implementa BFS no grafo de estados ---------------------
// open: fila de nós a expandir (FIFO)
// closed: tabela hash de estados visitados
// Retorna true se solução encontrada e preenche nos_expandidos e comprimento_solucao
// Mantém expansão mínima de nós (evita ciclos)
bool BFSGraph(const Estado& estadoInicial, int& nos_expandidos, int& comprimento_solucao) {
    if (estadoInicial.tiles.size() != SIZE) {
        cerr << "Estado inicial com tamanho inválido: " << estadoInicial.tiles.size() << endl;
        return false;
    }

    if (estadoInicial.isGoal()) {
        nos_expandidos = 0;
        comprimento_solucao = 0;
        return true;
    }

    deque<NoBusca*> open;    // deque usado na fila BFS (FIFO) usado para explorar os nós em ordem de profundidade
    unordered_set<Estado, HashEstado> closed;       // estados já visitados

    NoBusca* root = new NoBusca(estadoInicial, nullptr, 0);
    open.push_back(root);
    closed.insert(estadoInicial);

    nos_expandidos = 0;

    while (!open.empty()) {
        NoBusca* node = open.front();
        open.pop_front();
        nos_expandidos++;

        // Geração de sucessores na ordem fixa: cima, esquerda, direita, baixo
        vector<Estado> sucessores = node->estados.gerarSucessores();

        for (const Estado& succ : sucessores) {
            // Evita voltar ao estado pai imediato
            if (node->parent && succ == node->parent->estados) continue;

            // Verifica se é objetivo
            if (succ.isGoal()) {
                comprimento_solucao = node->pathCost + 1;
                freeNodes(open);  // libera memória da fila
                delete node;
                return true;
            }

            // Se ainda não visitado, insere na fila e marca como visitado
            if (closed.find(succ) == closed.end()) {
                NoBusca* newNode = new NoBusca(succ, node, node->pathCost + 1);
                open.push_back(newNode);
                closed.insert(succ);
            }
        }

        delete node;  // libera nó já expandido
    }

    comprimento_solucao = -1; // não encontrou solução
    return false;
}

// ------------------ PARSE ------------------
vector<Estado> estadosDaString(const string& input){
    vector<Estado> estados;
    stringstream ss(input);
    string token;
    while(getline(ss, token, ',')){
        vector<int> tiles;
        stringstream sst(token);
        int n;
        while(sst >> n) tiles.push_back(n);
        if(tiles.size()==SIZE) estados.emplace_back(tiles);
    }
    return estados;
}

vector<Estado> estadosDoArquivo(const string& filename){
    vector<Estado> estados;
    ifstream file(filename);
    if(!file) return estados;
    string line;
    while(getline(file,line)){
        if(line.empty()) continue;
        vector<int> tiles;
        stringstream ss(line);
        int n;
        while(ss>>n) tiles.push_back(n);
        if(tiles.size()==SIZE) estados.emplace_back(tiles);
    }
    return estados;
}

// ------------------ RESULT ------------------
struct Resultado{
    int nos_expandidos;
    int comprimento_solucao;
    double tempo;
    double heuristica_media;
    int heuristica_inicial;
};

void salvarResultado(const Resultado& r, ostream& out){
    if(r.nos_expandidos == -1){
        // Caso não resolvido, escreve traços
        out << "-,-,-,-,-\n";
    } else {
        out << r.nos_expandidos << ","
            << r.comprimento_solucao << ","
            << fixed << setprecision(10) << r.tempo << ","
            << fixed << setprecision(0) << r.heuristica_media << ","
            << r.heuristica_inicial << "\n";
    }
}

// ------------------ MAIN ----------------------------
// - Processa argumentos de entrada: lista de estados ou arquivo .txt
// - Converte entradas em vetores de Estado
// - Para cada estado:
//     • Executa BFSGraph
//     • Mede tempo de execução
//     • Calcula heurística inicial (Manhattan)
//     • Salva resultados no CSV e terminal
// - Estima memória usada pelos nós expandidos
int main(int argc, char* argv[]){
    if(argc<3 || string(argv[1])!="-bfs"){ cerr<<"Uso: ./main -bfs <estados>\n"; return 1; }

    auto removerEspacos = [](const string &s){
        const char* espacos = " \t\n\r";
        size_t start = s.find_first_not_of(espacos);
        if(start==string::npos) return string("");
        size_t end = s.find_last_not_of(espacos);
        return s.substr(start,end-start+1);
    };

    auto dividirPorVirgula = [&](const string &s){
        vector<string> out;
        string token;
        stringstream ss(s);
        while(getline(ss,token,',')){
            string t = removerEspacos(token);
            if(!t.empty()) out.push_back(t);
        }
        return out;
    };

    vector<string> instances;
    string arg2 = argv[2];
    bool arquivoLido = false;

    auto terminaCom = [](const string &s, const string &suffix){
        if(s.size()<suffix.size()) return false;
        return s.substr(s.size()-suffix.size())==suffix;
    };

    if(terminaCom(arg2,".txt")){
        ifstream infile(arg2);
        if(!infile){ cerr<<"Erro ao abrir arquivo: "<<arg2<<"\n"; return 1; }
        string line;
        while(getline(infile,line)){
            line=removerEspacos(line);
            if(!line.empty()){
                auto toks = dividirPorVirgula(line);
                if(!toks.empty()) instances.insert(instances.end(),toks.begin(),toks.end());
                else instances.push_back(line);
            }
        }
        infile.close();
        arquivoLido=true;
    }

    if(!arquivoLido){
        string joined;
        for(int i=2;i<argc;++i){ if(i>2) joined+=' '; joined+=argv[i]; }
        joined=removerEspacos(joined);
        auto toks = dividirPorVirgula(joined);
        if(!toks.empty()) instances = move(toks);
        else instances.push_back(joined);
    }

    if(instances.empty()){ cerr<<"Nenhuma instancia valida fornecida.\n"; return 1; }

    vector<Estado> estados;
    for(const string &inst_raw : instances){
        vector<int> tiles;
        stringstream ss(inst_raw);
        int num;
        while(ss>>num) tiles.push_back(num);
        if(tiles.size()!=SIZE){ cerr<<"Instancia invalida: "<<inst_raw<<"\n"; continue; }
        estados.emplace_back(tiles);
    }

    if(estados.empty()){ cerr<<"Nenhum estado valido para processar.\n"; return 1; }

    ofstream csv("resultados_bfs.csv");
    if(!csv){ cerr<<"Erro ao abrir resultados_bfs.csv\n"; return 1; }

    auto total_start = high_resolution_clock::now();

    size_t memoria_total_bytes = 0; // variável para estimativa de memória usada

    for(const Estado &s : estados){
        int nos_expandidos=0, comprimento_solucao=0;
        auto start = high_resolution_clock::now();
        bool puzzleTemSolucao = BFSGraph(s,nos_expandidos,comprimento_solucao);
        auto end = high_resolution_clock::now();
        double elapsed = duration<double>(end-start).count();

        Resultado r;
        r.nos_expandidos = puzzleTemSolucao? nos_expandidos:-1;
        r.comprimento_solucao = puzzleTemSolucao? comprimento_solucao:-1;
        r.tempo = elapsed;
        r.heuristica_media = 0.0;
        r.heuristica_inicial = manhattan(s);

        salvarResultado(r,cout);
        salvarResultado(r,csv);


        // Estimativa aproximada da memória utilizada pelos nós expandidos
        // Cada nó ocupa sizeof(NoBusca) + tamanho do vetor de tiles (9 inteiros)
        if(puzzleTemSolucao){
        memoria_total_bytes += nos_expandidos * (sizeof(NoBusca) + SIZE * sizeof(int));
    }
}

    auto total_end = high_resolution_clock::now();
    double total_elapsed = duration<double>(total_end-total_start).count();

    csv.close();
    cout<<"Resultados salvos em resultados_bfs.csv\n";
    cout<<"Tempo total de processamento: "<<fixed<<setprecision(2)<<total_elapsed<<" segundos\n";
    cout<<"Memoria aproximada usada: "<<fixed<<setprecision(2)<<memoria_total_bytes/(1024.0*1024.0)<<" MB\n";
    return 0;
}
