#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define false 0
#define true 1
#define azul 2
#define vermelho 3

typedef int bool;
typedef int PESO;

typedef struct adjacencia
{
  int vertice;             // vertice de destino
  PESO peso;               // peso associado a aresta que leva ao vertice de destino
  struct adjacencia *prox; // O próximo elemento da lista de adjacencias
} ADJACENCIA;

typedef struct vertice
{
  /* Dados armazenados vão aqui */
  ADJACENCIA *cab; // possui apenas a cabeça da lista de adjacencia
} VERTICE;

typedef struct grafo
{
  int vertices; // numero de vertice total do grafo
  int arestas;  // numero de arestas totais do grafo
  VERTICE *adj; // Arranjo de vertices da estrutura
} GRAFO;

/**função para criar um GRAFO**/
GRAFO *criaGrafo(int qtdvert)
{
  int i;

  GRAFO *graf = (GRAFO *)malloc(sizeof(GRAFO)); // aloca espaço para estrtura grafo
  graf->vertices = qtdvert;                     // atualizo o numero de vertice
  graf->arestas = 0;                            // atualizo o numero de vertice
  graf->adj = (VERTICE *)malloc(qtdvert * sizeof(VERTICE));
  // Dentro da estrturua tem só o arranjo para o ponteiro de vertice, não o arranjo em si
  //  então aloco o arranjo com (v) o numero de vertice desejado
  for (i = 0; i < qtdvert; i++)
  {
    graf->adj[i].cab = NULL; // coloco NULL em todas arestas
  }
  return (graf);
}

/**função para adicionar arestas no GRAFO**/

ADJACENCIA *criaAdj(int v, int peso)
{
  ADJACENCIA *temp = (ADJACENCIA *)malloc(sizeof(ADJACENCIA)); // aloca espaço para um nó
  temp->vertice = v;                                           // vertice alvo da adjacencia
  temp->peso = peso;                                           // peso da aresta
  temp->prox = NULL;
  return (temp); // retorno endereço da adjacencia
}

bool criaAresta(GRAFO *graf, int vi, int vf, PESO p, int criaVolta)
{ // vai de vi a vf
  if (!graf)
    return (false); // validações se o grafo existe
  if ((vf < 0) || (vf >= graf->vertices))
    return (false); // validações se os valores não são neg
  if ((vi < 0) || (vf >= graf->vertices))
    return (false); // ou maiores que o numero de vértice do grafo

  ADJACENCIA *novo = criaAdj(vf, p); // crio adjacencia com o vértice final e o peso
  // retirei a volta pois iria contar dobrado
  // coloco a adjacencia na lista do vértice inicial
  // ajustei para ordenar em crescente
  // o grafo ta sendo criado como se fosse direcionado, por questões de leitura mais a frente.
  // pois iremos ler os primeiros vértices e suas adjacencias
  if (graf->adj[vi].cab == NULL)
  {
    graf->adj[vi].cab = novo;
  }
  else
  {
    ADJACENCIA *aux = graf->adj[vi].cab;
    while (aux->prox != NULL)
    {
      aux = aux->prox;
    }
    aux->prox = novo;
  }
  // novo->prox = graf->adj[vi].cab;
  if(criaVolta == 1){
      ADJACENCIA *volta = criaAdj(vi, p); // criei a volta pois trata-se de uma grafo não direcionado
      if (graf->adj[vf].cab == NULL)
      {
        graf->adj[vf].cab = volta;
      }
      else
      {
        ADJACENCIA *aux = graf->adj[vf].cab;
        while (aux->prox != NULL)
        {
          aux = aux->prox;
        }
        aux->prox = volta;
      }
  }
  // volta->prox = graf->adj[vf].cab;
  // o campo prox da adjacencia vai receber a cabeça da lista
  // graf->adj[vi].cab=novo;
  // graf->adj[vf].cab=volta; // e a cabeça da lista passa a ser o novo elemento
  graf->arestas++; // atualizo o numero de aresta no grafo
  return (true);
}

void imprime(GRAFO *gr, int *estruturaSubC, int w)
{
  printf("Vertices: %d.\nArestas: %d. \nLista de adjacencia da combinacao %d\n", gr->vertices, gr->arestas,w+1); // imprime numero de vértice e arestas

  for (int i = 0; i < gr->vertices; i++)
  {
    printf("vertice %d ", i + 1); // Imprimo em qual aresta estou
    // e verifico em qual subconjunto ela está, através do vetor de identificação
    if (estruturaSubC[i] == vermelho)
    {
      printf("- (Vermelho): ");
    }
    else if (estruturaSubC[i] == azul)
    {
      printf("- (Azul): ");
    }
    else
    {
      printf("- (Não está em nenhum subconjunto): ");
    }
    ADJACENCIA *ad = gr->adj[i].cab; // chamo a cabeça da lista de adjacencia desta aresta
    while (ad)
    {                                                // enquanto as adjacencias não forem nula
      printf("v%d(%d) ", ad->vertice + 1, ad->peso); // imprimo a adjacencia e seu peso
      ad = ad->prox;                                 // passo para proxima adjacencia
    }
    printf("\n");
  }
}

void preencheMatriz(GRAFO *gr, int **matCombinacoes, int elev)
{
  for(int i = 0;i<elev; i++){
    int aux_num = i;
    for(int aux = gr->vertices-1; aux >= 0; aux--){
        matCombinacoes[i][aux] = aux_num%2;
        aux_num = aux_num/2;
    }
  }
  for (int i = 0; i < elev; i++)
  {
    for (int j = 0; j < gr->vertices; j++)
    {
      if(matCombinacoes[i][j] == 0){
        matCombinacoes[i][j] = azul;
      }else if(matCombinacoes[i][j] == 1){
        matCombinacoes[i][j] = vermelho;
      }
    }
  }
}

void imprimirMatriz(GRAFO *gr, int **matCombinacoes, int elev)
{
  for (int i = 0; i < elev; i++)
  {
    printf("Combinacao %d - ", i+1);
    for (int j = 0; j < gr->vertices; j++)
    {
      printf("[");
      if (matCombinacoes[i][j] == vermelho)
      {
        printf("vermelho");
      }
      else if (matCombinacoes[i][j] == azul)
      {
        printf("azul");
      }
      else
      {
        printf("Não esta definido.");
      }
      printf("] ");
    }
    printf("\n");
  }
}

void confereestrutura(int *estruturaSubConj, int i, ADJACENCIA *aux, GRAFO *gr, int *PesoMaxIntAzul, int *PesoMaxIntVer, int *pesoMaxExt)
{
  if (estruturaSubConj[i] == azul)
  {
    if (estruturaSubConj[aux->vertice] == azul)
    {
      (*PesoMaxIntAzul) += aux->peso;
    }
    else if (estruturaSubConj[aux->vertice] == vermelho)
    {
      (*pesoMaxExt) += aux->peso;
    }
  }
  else if (estruturaSubConj[i] == vermelho)
  {
    if (estruturaSubConj[aux->vertice] == vermelho)
    {
      (*PesoMaxIntVer) += aux->peso;
    }
    else if (estruturaSubConj[aux->vertice] == azul)
    {
      (*pesoMaxExt) += aux->peso;
    }
  }
  // aqui estou tratando a duplicidade da aresta que está sendo representada uma única vez, e fazendo com que seja lido apenas uma vez, no caso na ordem crescente.
  //gr->adj[aux->vertice].cab = gr->adj[aux->vertice].cab->prox;
}

void CorteMaximo(GRAFO *gr, int *estruturaSubConj, int *cortemax, int *subconjCorteMax, int subconjuntodavez)
{
  int pesoMaxExt = 0, PesoMaxIntVer = 0, PesoMaxIntAzul = 0;
  if (!estruturaSubConj)
  {
    printf("Sem memoria!\n");
    exit(1);
  }
  else
  {
    for (int i = 0; i < gr->vertices; i++)
    {
      if (gr->adj[i].cab != NULL)
      {
        ADJACENCIA *aux = gr->adj[i].cab;
        while (aux->prox != NULL)
        {
          confereestrutura(estruturaSubConj, i, aux, gr, &PesoMaxIntAzul, &PesoMaxIntVer, &pesoMaxExt);
          // gr->adj[aux->vertice].cab = gr->adj[aux->vertice].cab->prox;
          aux = aux->prox;
        }
        confereestrutura(estruturaSubConj, i, aux, gr, &PesoMaxIntAzul, &PesoMaxIntVer, &pesoMaxExt);
        // gr->adj[aux->vertice].cab = gr->adj[aux->vertice].cab->prox;
      }
    }
  }
  printf("\nCorte Maximo: %d.\nCorte Interno entre as arestas no subconjunto vermelho: %d\nCorte interno entre as arestas do subconjunto azul: %d\n\n", pesoMaxExt, PesoMaxIntVer, PesoMaxIntAzul);
  if(pesoMaxExt >= (*cortemax)){
    (*cortemax) = pesoMaxExt;
    (*subconjCorteMax) = subconjuntodavez;
  }
}

void main()
{
  FILE *arq;
  char *result;
  int linha;
  arq = fopen("Exemplo.txt", "rt");

  // Se houver erro na abertura
  if (arq == NULL)
  {
    printf("Problemas na abertura do arquivo\n");
    return;
  }
  linha = 1;
  GRAFO *graf, *grafreserva;
  //utilizei o graf reserva para construir as arestas de ida e volta, como se não fosse direcionado
  // o graf será para apresentar apenas as arestas uma única vez, e em ordem crescente
  int orig, dest, peso;
  int qtdvert, qtdaresta;
  int cortemax = 0, subconjuntoCorteMax = -1;
  while (!feof(arq))
  {
    // Lê uma linha (inclusive com o '\n')
    // Se foi possível ler
    if (linha == 1)
    {
      result = fscanf(arq, "%d%d", &qtdvert, &qtdaresta);
      graf = criaGrafo(qtdvert);
      grafreserva = criaGrafo(qtdvert);
    }
    else
    {
      result = fscanf(arq, "%d%d%d", &orig, &dest, &peso);
      criaAresta(graf, orig, dest, peso,0);
      criaAresta(grafreserva, orig, dest, peso,1);
    }
    linha++;
  }
  int *subconjuntos = (int *)malloc(graf->vertices * sizeof(int));
  // aqui ainda tenho que tratar para fazer as alternancias
  int elev = pow(2, graf->vertices);
  int **combinacoes = (int **)malloc(elev * sizeof(int *));
  for (int i = 0; i < elev; i++)
  {
    combinacoes[i] = (int *)malloc(graf->vertices * sizeof(int));
  }

  preencheMatriz(graf, combinacoes, elev);
  imprimirMatriz(graf, combinacoes, elev);
  printf("\n");

  for (int w = 0; w < elev; w++)
  {
    printf("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    for (int c = 0; c < graf->vertices; c++)
    {
      subconjuntos[c] = combinacoes[w][c];
    }
    imprime(grafreserva, subconjuntos,w);
    CorteMaximo(graf, subconjuntos, &cortemax, &subconjuntoCorteMax, w);
  }

  printf("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
  printf("\nO Corte-Maximo foi obtido no subconjunto de combinacao %d, e com valor de: %d\n", subconjuntoCorteMax+1, cortemax);
  imprime(grafreserva,combinacoes[subconjuntoCorteMax],subconjuntoCorteMax);
  /*subconjuntos[0] = vermelho;
  subconjuntos[1] = azul;
  subconjuntos[2] = azul;
  subconjuntos[3] = vermelho;
  imprime(graf, subconjuntos);
  CorteMaximo(graf, subconjuntos);*/
  fclose(arq);
}
