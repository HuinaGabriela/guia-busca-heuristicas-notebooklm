# 📘 Mini Guia de Estudos – Busca em Grafos e Problema do 8-Puzzle

🎯 Contexto e Objetivos

Este projeto foi desenvolvido como parte de um desafio prático da DIO com o objetivo de explorar o uso da Inteligência Artificial como ferramenta de aprendizagem ativa.

O tema escolhido foi busca heurística em IA, com foco nos algoritmos:

Best-First Search

Greedy Search

A* (A-Star)

A* Ponderado

E sua aplicação no problema clássico do 8-puzzle.

📌 Objetivos de Estudo
Entender como heurísticas guiam algoritmos de busca
Comparar diferentes estratégias de busca informada
Analisar desempenho entre heurísticas (h1 vs h2)
Aplicar conceitos teóricos em problemas clássicos

📚 Curadoria de Fontes

As seguintes fontes foram utilizadas e analisadas no NotebookLM:

📘 Russell, S.; Norvig, P. — Artificial Intelligence: A Modern Approach

📄 Materiais sobre algoritmo A*

📄 Conteúdos sobre o problema do 8-puzzle

📄 Estudos sobre heurísticas admissíveis e consistentes

📄 Documentação sobre busca informada


🤖 Engenharia de Prompts e Aprendizado

Durante o processo, utilizei diferentes estratégias de prompts para extrair conhecimento mais preciso.

🔹 Prompt 1

Pergunta:
Explique a heurística Best-First Search.

Resultado:
Obtive uma visão geral do algoritmo e sua função de avaliação f(n).

🔹 Prompt 2

Pergunta:
Qual a melhor estratégia para resolver o 8-puzzle?

Resultado:
Identifiquei que o algoritmo A* com heurística de Manhattan é o mais eficiente.

🔹 Prompt 3

Pergunta:
Compare heurísticas h1 e h2 no 8-puzzle com dados.

Resultado:
Obtive comparações quantitativas de desempenho (número de nós expandidos).

⚠️ Dificuldades Encontradas

Respostas iniciais muito genéricas

Falta de comparação quantitativa entre heurísticas

Necessidade de refinar perguntas

✅ Estratégias de Melhoria

Uso de prompts mais específicos

Solicitação de exemplos numéricos

Comparações diretas entre algoritmos

📘 Miniguia de Estudo

📌 1. Best-First Search (Resumo)

A Best-First Search é uma classe de algoritmos de busca informada que seleciona o próximo nó com base em uma função de avaliação:

f(n)

Ela utiliza uma fila de prioridade e sempre expande o nó mais promissor.

🔍 Principais Variações

Greedy Best-First: f(n) = h(n)

Rápido, mas não garante solução ótima

A*: f(n) = g(n) + h(n)

Completo e ótimo (se h for admissível)

A* Ponderado: f(n) = g(n) + w·h(n)

Mais rápido, mas pode perder otimalidade

🧩 2. Estratégias para o 8-Puzzle

✅ Melhor abordagem:

A* com heurística de Manhattan

🔢 Heurísticas

h1 — Peças fora do lugar

Conta peças em posições erradas

Simples, porém menos eficiente

h2 — Distância de Manhattan

Soma das distâncias horizontal e vertical

Mais precisa e eficiente

📊 Comparação de Desempenho

Heurística	Nós expandidos (d=12)

h1	~227

Manhattan	~73

👉 Manhattan pode ser até 50.000x mais eficiente que busca cega

🧠 3. Conceitos Fundamentais

✔ Heurística Admissível

Nunca superestima o custo real até o objetivo.

✔ Heurística Consistente

Mantém coerência entre estados (evita retrabalho).

✔ Dominação

Se h2 ≥ h1 para todos os estados, então h2 é mais eficiente.

📖 4. Glossário

Heurística: função que estima custo até o objetivo

A*: algoritmo de busca ótima

g(n): custo real do caminho

h(n): custo estimado

f(n): função de avaliação

Admissível: nunca superestima

Consistente: evita revisitar nós

🔁 5. Prompts Reutilizáveis

Explique A* com exemplo passo a passo

Compare heurísticas admissíveis

Como melhorar desempenho no 8-puzzle?

Explique dominação entre heurísticas


🏁 Conclusão

O estudo demonstrou que o uso de heurísticas adequadas transforma problemas complexos em soluções viáveis.

A combinação do algoritmo A* com a heurística de Manhattan mostrou-se extremamente eficiente para o 8-puzzle, reduzindo drasticamente o número de nós explorados.

Além disso, a prática de engenharia de prompts evidenciou a importância de formular boas perguntas para extrair conhecimento relevante da IA.
