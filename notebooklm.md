📌 1. Resumo: Best-First Search (Busca de Melhor Escolha)

A Best-First Search é uma classe de algoritmos de busca informada que escolhe sempre o próximo nó com base em uma função de avaliação:

        𝑓(𝑛)

Essa função representa uma estimativa de “quão bom” é o nó (normalmente custo até o objetivo).

🔹 Ideia principal
Mantém uma fila de prioridade (priority queue)
Sempre expande o nó com menor valor de f(n)
Usa conhecimento do problema (heurística) para guiar a busca
🔸 Principais variações
1. Greedy Best-First
        𝑓(𝑛)=ℎ(𝑛)

Usa só a heurística
Muito rápido 🚀
❌ Não garante solução ótima
2. A*

f(n)=g(n)+h(n)
g(n) = custo real até o nó
h(n) = estimativa até o objetivo

✔ Completo
✔ Ótimo (se h for admissível)

👉 Melhor escolha geral

3. A* Ponderado

f(n)=g(n)+w⋅h(n)
Mais rápido que A*
❌ Pode perder otimalidade

👉 Trade-off: velocidade vs qualidade

🧩 2. Estratégias para o 8-Puzzle
🔥 Melhor escolha: A* + boa heurística
✅ Heurísticas principais
1. Distância de Manhattan (melhor)
Soma das distâncias horizontal + vertical de cada peça
✔ Admissível
✔ Consistente
✔ Muito eficiente

👉 Expande MUITO menos nós

2. Peças fora do lugar (h1)
Conta quantas peças estão erradas
✔ Simples
❌ Menos informativa
⚡ Comparação real
h1: ~227 nós (profundidade 12)
Manhattan: ~73 nós

👉 Manhattan pode ser até 50.000x melhor que busca cega

3. Técnicas avançadas
🔹 Pattern Databases
Pré-calcula soluções de subproblemas
Muito mais preciso
🔹 Disjoint Pattern Databases
Soma heurísticas sem perder admissibilidade
🚀 Pode acelerar MUITO (até milhões de vezes em problemas maiores)

4. Combinação de heurísticas
        ℎ(𝑛)=max(ℎ1,ℎ2)
​
✔ Mantém admissibilidade
✔ Mais eficiente

🧠 3. Conceitos importantes
✔ Heurística admissível
Nunca superestima o custo real

        h(n)≤h∗(n)

👉 Garante solução ótima no A*

✔ Heurística consistente
Respeita desigualdade triangular
👉 Evita reabrir nós
✔ Dominação
Se:

        h2(n)≥h1(n)

Então:
👉 h2 é melhor (expande menos nós)

📊 4. Comparação geral de algoritmos
Algoritmo	Velocidade	Ótimo	Nós expandidos
BFS	❌ lento	✔	MUITOS
Greedy	🚀 rápido	❌	poucos
A*	⚖️ médio	✔	poucos (ótimo)
📌 5. Respostas das observações do criador
🧠 O que define um agente racional?

Um agente racional:

Escolhe ações que maximizam o desempenho esperado
Baseado em:
percepção
conhecimento
objetivos
📈 Como a IA evoluiu (regras → dados)?
Antes:
Sistemas baseados em regras lógicas
Ex: sistemas especialistas
Hoje:
Baseados em dados (Machine Learning)
Aprendem padrões automaticamente

👉 Mudança principal:
de “programar inteligência” → “aprender inteligência”

🧩 4 categorias de definição de IA

Segundo o livro (Russell & Norvig):

1. Agir como humano (Teste de Turing)
2. Pensar como humano
3. Pensar racionalmente
4. Agir racionalmente ✅ (mais usada)


🎯 Conclusão prática (para prova ou projeto)

👉 Para o 8-puzzle, use:

✔ A*
✔ Heurística Manhattan
✔ (Opcional) combinar heurísticas

👉 Se quiser mais velocidade:

A* ponderado
Pattern databases