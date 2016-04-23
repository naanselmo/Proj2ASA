# Proj2ASA
Introdução breve
----------------------
Modelamos o problema com um grafo dirigido e pesado, representado por uma lista de adjacências,
onde um vértice correspondia a uma localidade que por sua vez poderia ou não ter uma filial associada. Se tal acontecesse
o grafo teria essa localidade não só no array de vértices, mas também no array de filiais para futura consulta.

O nosso algoritmo é baseado no algoritmo de Johnson.
Tal como o algoritmo de Johnson usamos Bellman-Ford e Dijkstra, para calcular as distâncias de todas as filiais a todas
as localidades, e com essa informação escolher uma localidade cujo custo total seja o menor,
ou seja o ponto de encontro pedido pelo problema.

Descrição da solução
----------------------
A nossa solução como já foi referido baseia-se no algoritmo de Johnson, e como tal passa pelos seguintes passos:

1. Começamos por adicionar um vértice S ao grafo que tem uma aresta para todos os outros vértices com custo 0.
A adição deste vértice nada altera o grafo original visto que nenhum vértice consegue chegar a S.

2. Depois de adicionado, executamos o algoritmo de Bellman-Ford em S, obtendo as distancias de S a todos os outros vértices
do grafo.

3. Iteramos por todos os vértices guardando a distância de cada um (calculada pelo algoritmo de Bellman-Ford)
num campo com nome h para futura consulta.

4. Repesamos as arestas todas do grafo com base no campo h de cada vértice, ou seja, para cada aresta (u, v) do grafo,
calculamos o novo peso da aresta através da seguinte formula: _peso\_novo(u, v) = peso(u, v) + h(u) - h(v)_.
Esta repesagem com base no campo h de todos os vértices permite remover todos os pesos negativos preservando
os caminhos de menor custo do grafo.

5. Depois da repesagem, removemos o vértice S e todas as arestas do mesmo.

6. Calculamos então o custo total de todas as filiais a todas as localidades. Para tal temos um array com o mesmo número
de elementos que o número de localidades, de modo a cada localidade esteja associado a um custo total.
Para calcular o custo total associado a cada localidade, iteramos todas as filiais, e para cada filial f, executamos
o algoritmo de Dijkstra e iteramos todas as localidades, e para cada localidade l adicionamos ao seu custo total a distancia de f a l.
Esta distância adicionada, não é a distancia devolvida pelo algoritmo de Dijkstra, visto que esta é invalida no grafo original devido à repesagem de arestas,
mas sim a distância no grafo original que é possível ser calculada através da seguinte formula: _d\_original(l) = d\_com\_repesagem(l) + h(l) - d\_com\_repesagem(f)_.

7. Com o array, podemos iterar todos os elementos do mesmo ver qual a localidade que tem um menor custo total associado.
A localidade com o menor custo total associado é o nosso ponto de encontro. Se todas as localidades tiverem como custo
total associado o valor infinito, então é porque não foi possível encontrar um ponto de encontro e o nosso algoritmo para.
Caso tenhamos sucedido na procura de um ponto de encontro então o nosso algoritmo continua.

8. Como ultimo passo, transpomos o grafo e corremos Dijkstra no ponto de encontro calculado no passo anterior.
Imprimimos então as distancias originais do ponto de encontro a cada filial através da formula referida em 6,
que são as mesmas de cada filial ao ponto de encontro no grafo original.

Análise teórica
----------------------
Na nossa análise teórica, passaremos a tratar V como numero de vértices/localidades do grafo, E como numero de arestas
e F como número de filiais.

Antes de anunciarmos as complexidades temporais de cada passo, deixamos a complexidade temporal de cada algoritmo usado:

* Bellman-Ford - O(V.E) no pior caso.
* Dijkstra - O(E + V.log(V)) implementado com priority queues em C++.
* Transpor grafo - O(V + E) transpondo um grafo representado por uma lista de adjacências.

As complexidades temporais para cada passo são as seguintes:

* Em 1. temos como complexidade temporal O(V) visto que percorremos todos os vértices para adicionar uma aresta de S para os mesmos.
* Em 2. temos como complexidade temporal no pior caso O(V.E).
* Em 3. temos como complexidade temporal O(V) visto que iteramos todos os vértices para guardar a distancia de cada um em h.
* Em 4. temos como complexidade temporal O(E) visto que iteramos todas as arestas do grafo para as repesarmos. Note que cada repesagem tem como complexidade temporal O(1).
* Em 5. temos como complexidade temporal O(1) visto que remover o vértice S apenas executamos 2 instruções.
* Em 6. temos como complexidade temporal O(F((E + V.log(V)) + V)) visto que para cada filial executamos o algoritmo de Dijkstra e
logo de seguida iteramos cada localidade, para adicionar a distancia sem repesagem ao array de custo total. Note que anular a repesagem tem como complexidade temporal O(1).
* Em 7. temos como complexidade temporal O(V) visto que iteramos todas as localidades para ver qual tem o menor custo total associado.
* Em 8. temos como complexidade temporal O(V+E + (E + V.log(V)) + F) visto que transpomos o grafo seguido de uma execução do algoritmo de Dijkstra
e de uma iteração de todos os filiais de modo a imprimir as distâncias ao ponto de encontro sem a repesagem. Note que anular a repesagem tem como complexidade temporal O(1).

Logo temos como complexidade total O(F.E + F.V.log(V)).

Análise experimental
----------------------

Referências
----------------------
<https://www.youtube.com/watch?v=TV2Z6nbo1ic>

<https://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm>

<https://en.wikipedia.org/wiki/Johnson's_algorithm>

<http://stackoverflow.com/questions/2974470/efficiency-of-the-stl-priority-queue>