#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

bool criaAresta(GRAFO *graf, int vi, int vf, PESO p)
{ // vai de vi a vf
  if (!graf)
    return (false); // validações se o grafo existe
  if ((vf < 0) || (vf >= graf->vertices))
    return (false); // validações se os valores não são neg
  if ((vi < 0) || (vf >= graf->vertices))
    return (false); // ou maiores que o numero de vértice do grafo

  ADJACENCIA *novo = criaAdj(vf, p); // crio adjacencia com o vértice final e o peso
  // retirei a volta pois iria contar dobrado
  // ADJACENCIA *volta = criaAdj(vi,p); // criei a volta pois trata-se de uma grafo não direcionado
  // coloco a adjacencia na lista do vértice inicial
  // ajustei para ordenar em crescente
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
  // volta->prox = graf->adj[vf].cab;
  // o campo prox da adjacencia vai receber a cabeça da lista
  // graf->adj[vi].cab=novo;
  // graf->adj[vf].cab=volta; // e a cabeça da lista passa a ser o novo elemento
  graf->arestas++; // atualizo o numero de aresta no grafo
  return (true);
}

void imprime(GRAFO *gr, int *estrutura)
{
  // validações se o grafo existe

  printf("Vertices: %d.\nArestas: %d. \nLista de adjacencia\n", gr->vertices, gr->arestas); // imprime numero de vértice e arestas

  for (int i = 0; i < gr->vertices; i++)
  {
    if (estrutura[i] == vermelho)
    {
      printf("(Vermelho) - ");
    }
    else if (estrutura[i] == azul)
    {
      printf("(Azul) - ");
    }
    else
    {
      printf("Não está em nenhum subconjunto - ");
    }
    printf("vertice %d: ", i + 1);   // Imprimo em qual aresta estou
    ADJACENCIA *ad = gr->adj[i].cab; // chamo a cabeça da lista de adjacencia desta aresta
    while (ad)
    {                                                // enquanto as adjacencias não forem nula
      printf("v%d(%d) ", ad->vertice + 1, ad->peso); // imprimo a adjacencia e seu peso
      ad = ad->prox;                                 // passo para proxima adjacencia
    }
    printf("\n");
  }
}

void confereestrutura(int *estrutura, int i, ADJACENCIA *aux, GRAFO *gr, int *PesoMaxIntAzul, int *PesoMaxIntVer, int *pesoMaxExt)
{
  if (estrutura[i] == azul)
  {
    if (estrutura[aux->vertice] == azul)
    {
      (*PesoMaxIntAzul) += aux->peso;
    }
    else if (estrutura[aux->vertice] == vermelho)
    {
      (*pesoMaxExt) += aux->peso;
    }
  }
  else if (estrutura[i] == vermelho)
  {
    if (estrutura[aux->vertice] == vermelho)
    {
      (*PesoMaxIntVer) += aux->peso;
    }
    else if (estrutura[aux->vertice] == azul)
    {
      (*pesoMaxExt) += aux->peso;
    }
  }
}

void CorteMaximo(GRAFO *gr, int *estrutura)
{
  int pesoMaxExt = 0, PesoMaxIntVer = 0, PesoMaxIntAzul = 0;
  if (!estrutura)
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
          confereestrutura(estrutura, i, aux, gr, &PesoMaxIntAzul, &PesoMaxIntVer, &pesoMaxExt);
          aux = aux->prox;
        }
        confereestrutura(estrutura, i, aux, gr, &PesoMaxIntAzul, &PesoMaxIntVer, &pesoMaxExt);
      }
    }
  }
  printf("\nCorte Maximo: %d.\nCorte Interno entre as arestas no subconjunto vermelho: %d\nCorte interno entre as arestas do subconjunto azul: %d\n", pesoMaxExt, PesoMaxIntVer, PesoMaxIntAzul);
  // return pesoMax;
}

void main()
{
  FILE *arq;
  char *result;
  int i;
  arq = fopen("Exemplo.txt", "rt");

  // Se houver erro na abertura
  if (arq == NULL)
  {
    printf("Problemas na abertura do arquivo\n");
    return;
  }
  i = 1;
  GRAFO *graf;
  int orig, dest, peso;
  int qtdvert, qtdaresta;
  while (!feof(arq))
  {
    // Lê uma linha (inclusive com o '\n')
    // Se foi possível ler
    if (i == 1)
    {
      result = fscanf(arq, "%d%d", &qtdvert, &qtdaresta);
      graf = criaGrafo(qtdvert);
    }
    else
    {
      result = fscanf(arq, "%d%d%d", &orig, &dest, &peso);
      criaAresta(graf, orig, dest, peso);
    }
    i++;
  }
  int *estrutura = (int *)malloc(graf->vertices * sizeof(int));
  // aqui ainda tenho que tratar para fazer as alternancias
  estrutura[0] = azul;
  estrutura[1] = vermelho;
  estrutura[2] = vermelho;
  estrutura[3] = azul;
  imprime(graf, estrutura);
  CorteMaximo(graf, estrutura);
  fclose(arq);
}
