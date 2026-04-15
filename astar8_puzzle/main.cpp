
// -------------------------------------------------------------------------
//  Resolvedor de 8-Puzzle usando o algoritmo A* (A estrela)
// -------------------------------------------------------------------------

// Descrição:
// Este programa resolve instâncias do 8-Puzzle utilizando o 
// algoritmo A* com heurística de distância de Manhattan. 
// A prioridade dos nós segue a regra: menor f → menor h → LIFO.

// Observações:
// - Cada nó representa um estado do puzzle.
// - A função heurística estima a distância até o objetivo.
// - A openSet armazena os nós a explorar (priority_queue).
// - A closedSet impede revisitar estados já expandidos.
// - Movimentos seguem a ordem: cima, esquerda, direita, baixo.

// Como compilar e executar:
//    make clean
//    make
//    ./main  -astar 0 6 1 7 4 2 3 8 5, 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5
//    ./main -astar instances.txt
// -------------------------------------------------------------------------


#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <memory>
#include <fstream>

using namespace std;
using namespace chrono;

// ===================== CONSTANTES =====================

// Tamanho do puzzle (8-puzzle → 9 posições)
constexpr int PUZZLE_SIZE = 9;
constexpr int N = 3;
const vector<int> GOAL_STATE = {0,1,2,3,4,5,6,7,8};

// ===================== ESTRUTURA DE RESULTADO =====================
struct Resultado {
    int nos_expandidos;
    int comprimento_solucao;
    double tempo;    
    double heuristica_media; 
    int heuristica_inicial;
};

// Função para salvar resultados no console ou CSV
void salvarResultado(const Resultado& r, ostream& out) {
    if (r.nos_expandidos < 0) {
        out << "-,-,-,-,-\n";
        return;
    }

    // Salva estado de formatação do stream para restaurar depois
    ios::fmtflags oldFlags = out.flags();
    streamsize oldPrec = out.precision();

    out << r.nos_expandidos << ","
        << r.comprimento_solucao << ","
        << fixed << setprecision(10) << r.tempo << ","
        << fixed << setprecision(5) << r.heuristica_media << ","
        << r.heuristica_inicial
        << "\n";

    out.flags(oldFlags);
    out.precision(oldPrec);
}

// ===================== ESTRUTURA DE NÓ =====================
struct Node {
    vector<int> state;        // Estado atual do puzzle
    int g, h, f;              // g = custo do caminho, h = heurística, f = g + h
    int zeroPos;              // Posição do espaço vazio (0)
    long long ordemInsercao;  // Para desempate LIFO na prioridade
    shared_ptr<Node> parent;  // Ponteiro para o nó pai (reconstrução do caminho)

    Node(const vector<int>& st, int g_, int h_, int zeroPos_, long long ts, shared_ptr<Node> p = nullptr)
        : state(st), g(g_), h(h_), f(g_ + h_), zeroPos(zeroPos_), ordemInsercao(ts), parent(move(p)) {}
};

// estimativa de memória ocupada por cada nó (struct + vector + dados)
constexpr size_t BYTES_POR_NODE = sizeof(Node) + sizeof(vector<int>) + PUZZLE_SIZE * sizeof(int);

// ===================== HEURÍSTICA =====================

// Armazena a posição final de cada peça (linha, coluna)
vector<pair<int,int>> goalPos(PUZZLE_SIZE);

// Inicializa posições do objetivo (para cálculo da heurística)
void inicializarPosicaoObjetivo() {
    for (int i = 0; i < PUZZLE_SIZE; i++)
        goalPos[GOAL_STATE[i]] = {i / N, i % N};
}

// Distância de Manhattan de uma peça até sua posição final
int distanciaManhattan(int idx, int val) {
    if (val == 0) return 0;
    int x1 = idx % N, y1 = idx / N;
    int x2 = goalPos[val].second, y2 = goalPos[val].first;
    return abs(x1 - x2) + abs(y1 - y2);
}

// Soma das distâncias de Manhattan de todas as peças
int calcularHeuristica(const vector<int>& state) {
    int dist = 0;
    for (int i = 0; i < PUZZLE_SIZE; i++) dist += distanciaManhattan(i, state[i]);
    return dist;
}

// ===================== HASH PARA unordered_set =====================

// Permite usar vector<int> em unordered_set para verificar estados já visitados
struct HashVector {
    size_t operator()(const vector<int>& v) const {
        size_t hash = 0;
        for (auto &i : v) hash = hash * 31 + i;
        return hash;
    }
};

// ===================== GERAÇÃO DE SUCESSORES =====================

// Gera todos os movimentos válidos
// Evita voltar para o estado anterior
vector<vector<int>> gerar_sucessores(const vector<int>& state, int zeroPos, int parentZeroPos) {
    vector<vector<int>> successors;
    vector<int> dirs = {-N, -1, 1, N}; // cima, esquerda, direita, baixo
    for (int dir : dirs) {
        int newPos = zeroPos + dir;
        // Verifica limites do tabuleiro
        if (newPos < 0 || newPos >= PUZZLE_SIZE) continue;
        if (dir == -1 && zeroPos % N == 0) continue;
        if (dir == 1 && zeroPos % N == N-1) continue;
        // Evita gerar estado pai
        if (newPos == parentZeroPos) continue;

        vector<int> newState = state;
        swap(newState[zeroPos], newState[newPos]);
        successors.push_back(newState);
    }
    return successors;
}

// ===================== PRIORIDADE NA OPENSET =====================

// A* usa prioridade: menor f → menor h → LIFO
struct CompareNode {
    bool operator()(const shared_ptr<Node>& a, const shared_ptr<Node>& b) const {
        if (a->f != b->f) return a->f > b->f; // menor f primeiro
        if (a->h != b->h) return a->h > b->h; // menor h desempata
        return a->ordemInsercao < b->ordemInsercao;   // LIFO desempata finalmente
    }
};

// ===================== ALGORITMO A* =====================
Resultado resolverAStar(const vector<int>& startState) {
    Resultado r{};
    int nodesExpanded = 0;    
    long long heuristicCalls = 0;
    double heuristicSum = 0;
    long long ordemInsercao_Cont = 0;

    int zeroPos = find(startState.begin(), startState.end(), 0) - startState.begin();
    int h0 = calcularHeuristica(startState); // Heurística inicial

    unordered_set<vector<int>, HashVector> closedSet; 
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, CompareNode> openSet;

    // Insere o nó inicial na openSet
    openSet.push(make_shared<Node>(startState, 0, h0, zeroPos, ordemInsercao_Cont++));

    auto startTime = high_resolution_clock::now();


    // Expansão dos nós da openSet seguindo prioridade f → h → LIFO
    while (!openSet.empty()) {
        auto current = openSet.top(); 
        openSet.pop();

        if (closedSet.count(current->state)) continue; // Marca como visitado
        closedSet.insert(current->state);
        nodesExpanded++;

        // Verifica se chegou ao objetivo
        if (current->state == GOAL_STATE) {
            auto endTime = high_resolution_clock::now();
            chrono::duration<double> elapsed = endTime - startTime;

            // Retorna resultados
            r.nos_expandidos = nodesExpanded - 1; // não conta o nó objetivo
            r.comprimento_solucao = current->g;
            r.tempo = elapsed.count();
            r.heuristica_media = heuristicCalls ? heuristicSum / heuristicCalls : 0;
            r.heuristica_inicial = h0;
            return r;
        }

        // Geração de sucessores válidos
        vector<vector<int>> successors = gerar_sucessores(
            current->state, current->zeroPos, 
            current->parent ? current->parent->zeroPos : -1
        );

        for (auto &succ : successors) {
            if (closedSet.count(succ)) continue; // Ignora estados visitados

            int newZeroPos = find(succ.begin(), succ.end(), 0) - succ.begin();
            int hSucc = calcularHeuristica(succ);        // Calcula heurística do sucessor

            // Atualiza contadores de heurística
            heuristicCalls++;
            heuristicSum += hSucc;

            // Adiciona sucessor na openSet com prioridade
            openSet.push(make_shared<Node>(
                succ, current->g + 1, hSucc, newZeroPos, ordemInsercao_Cont++, current
            ));
        }
    }

    // Caso não encontre solução
    r.nos_expandidos = -1;
    r.comprimento_solucao = -1;
    r.tempo = -1;
    r.heuristica_media = -1;
    r.heuristica_inicial = h0;
    return r;
}

// ===================== FUNÇÃO PRINCIPAL =====================
int main(int argc, char* argv[]) {
    // Verifica argumentos
    if (argc < 3 || string(argv[1]) != "-astar") {
        cerr << "Uso: ./main -astar <instancias...> OU <arquivo.txt>\n";
        return 1;
    }

    inicializarPosicaoObjetivo(); // Inicializa posições do objetivo

    // Função auxiliar para remover espaços
    auto removerEspacosString = [](const string &s) {
        const char* ws = " \t\n\r";
        size_t start = s.find_first_not_of(ws);
        if (start == string::npos) return string("");
        size_t end = s.find_last_not_of(ws);
        return s.substr(start, end - start + 1);
    };

    // Divide uma string por vírgula
    auto dividirPorVirgula = [&](const string &s) {
        vector<string> out;
        string token;
        stringstream ss(s);
        while (getline(ss, token, ',')) {
            string t = removerEspacosString(token);
            if (!t.empty()) out.push_back(t);
        }
        return out;
    };

    auto terminaCom = [](const string &s, const string &suffix) -> bool {
        if (s.size() < suffix.size()) return false;
        return s.substr(s.size() - suffix.size()) == suffix;
    };

    vector<string> instances;
    string arg2 = argv[2];
    bool file_read = false;

    // Leitura de arquivo .txt
    if (terminaCom(arg2, ".txt")) {
        ifstream infile(arg2);
        if (!infile) { cerr << "Erro ao abrir arquivo: " << arg2 << "\n"; return 1; }
        string line;
        while (getline(infile, line)) {
            line = removerEspacosString(line);
            if (!line.empty()) {
                auto toks = dividirPorVirgula(line);
                if (!toks.empty()) instances.insert(instances.end(), toks.begin(), toks.end());
                else instances.push_back(line);
            }
        }
        infile.close();
        file_read = true;
    }

    // Caso entrada seja pela linha de comando
    if (!file_read) {
        string joined;
        for (int i = 2; i < argc; ++i) {
            if (i > 2) joined += ' ';
            joined += argv[i];
        }
        joined = removerEspacosString(joined);
        auto toks = dividirPorVirgula(joined);
        if (!toks.empty()) instances = move(toks);
        else instances.push_back(joined);
    }

    if (instances.empty()) { cerr << "Nenhuma instancia valida fornecida.\n"; return 1; }

    // Converte strings para estados do puzzle
    vector<vector<int>> states;
    for (const string &inst_raw : instances) {
        vector<int> tiles;
        stringstream ss(inst_raw);
        int n;
        while (ss >> n) tiles.push_back(n);
        if (tiles.size() != PUZZLE_SIZE) { cerr << "Instancia invalida: " << inst_raw << "\n"; continue; }
        states.push_back(tiles);
    }

    if (states.empty()) { cerr << "Nenhum estado valido para processar.\n"; return 1; }

    ofstream csv("resultadosAstar.csv");
    if (!csv) { cerr << "Erro ao abrir resultadosAstar.csv\n"; return 1; }

    auto total_start = high_resolution_clock::now();
    size_t memoria_total_bytes = 0; // acumulador da memória

    

    // Resolve cada estado inicial
    for (const auto &s : states) {
        //auto start = high_resolution_clock::now();
        Resultado r = resolverAStar(s);
        //auto end = high_resolution_clock::now();
        //r.tempo = duration<double>(end - start).count();
        salvarResultado(r, cout);
        salvarResultado(r, csv);

        // estimativa de memória mais realista
        if (r.nos_expandidos > 0){
            memoria_total_bytes += r.nos_expandidos * BYTES_POR_NODE;
        }
    }

    auto total_end = high_resolution_clock::now();
    double total_elapsed = duration<double>(total_end - total_start).count();

    csv.close();
    cout << "Resultados salvos em resultadosAstar.csv\n";
    cout << "Tempo total de processamento: " << fixed << setprecision(2) << total_elapsed << " segundos\n";
    cout << "Memoria aproximada usada: " 
         << memoria_total_bytes / (1024.0 * 1024.0) << " MB\n";
    return 0;
}
