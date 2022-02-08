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
  //ADJACENCIA *volta = criaAdj(vi, p); // criei a volta pois trata-se de uma grafo não direcionado
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
  /*if (graf->adj[vf].cab == NULL)
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
  }*/
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
  /*for (int i = 0; i < elev; i++)
  {
    for (int j = 0; j < gr->vertices; j++)
    {
      matCombinacoes[i][j] = 0;
    }
  }*/

  // combinação 1
  matCombinacoes[0][0] = vermelho;
  matCombinacoes[0][1] = vermelho;
  matCombinacoes[0][2] = vermelho;
  matCombinacoes[0][3] = vermelho;

  // combinação 2
  matCombinacoes[1][0] = vermelho;
  matCombinacoes[1][1] = vermelho;
  matCombinacoes[1][2] = vermelho;
  matCombinacoes[1][3] = azul;

  // combinação 3
  matCombinacoes[2][0] = vermelho;
  matCombinacoes[2][1] = vermelho;
  matCombinacoes[2][2] = azul;
  matCombinacoes[2][3] = vermelho;

  // combinação 4
  matCombinacoes[3][0] = vermelho;
  matCombinacoes[3][1] = vermelho;
  matCombinacoes[3][2] = azul;
  matCombinacoes[3][3] = azul;

  // combinação 5
  matCombinacoes[4][0] = vermelho;
  matCombinacoes[4][1] = azul;
  matCombinacoes[4][2] = vermelho;
  matCombinacoes[4][3] = vermelho;

  // combinação 6
  matCombinacoes[5][0] = vermelho;
  matCombinacoes[5][1] = azul;
  matCombinacoes[5][2] = vermelho;
  matCombinacoes[5][3] = azul;

  // combinação 7
  matCombinacoes[6][0] = vermelho;
  matCombinacoes[6][1] = azul;
  matCombinacoes[6][2] = azul;
  matCombinacoes[6][3] = vermelho;

  // combinação 8
  matCombinacoes[7][0] = vermelho;
  matCombinacoes[7][1] = azul;
  matCombinacoes[7][2] = azul;
  matCombinacoes[7][3] = azul;

  // combinação 9
  matCombinacoes[8][0] = azul;
  matCombinacoes[8][1] = vermelho;
  matCombinacoes[8][2] = vermelho;
  matCombinacoes[8][3] = vermelho;

  // combinação 10
  matCombinacoes[9][0] = azul;
  matCombinacoes[9][1] = vermelho;
  matCombinacoes[9][2] = vermelho;
  matCombinacoes[9][3] = azul;

  // combinação 11
  matCombinacoes[10][0] = azul;
  matCombinacoes[10][1] = vermelho;
  matCombinacoes[10][2] = azul;
  matCombinacoes[10][3] = vermelho;

  // combinação 12
  matCombinacoes[11][0] = azul;
  matCombinacoes[11][1] = vermelho;
  matCombinacoes[11][2] = azul;
  matCombinacoes[11][3] = azul;

  // combinação 13
  matCombinacoes[12][0] = azul;
  matCombinacoes[12][1] = azul;
  matCombinacoes[12][2] = vermelho;
  matCombinacoes[12][3] = vermelho;

  // combinação 14
  matCombinacoes[13][0] = azul;
  matCombinacoes[13][1] = azul;
  matCombinacoes[13][2] = vermelho;
  matCombinacoes[13][3] = azul;

  // combinação 15
  matCombinacoes[14][0] = azul;
  matCombinacoes[14][1] = azul;
  matCombinacoes[14][2] = azul;
  matCombinacoes[14][3] = vermelho;

  // combinação 16
  matCombinacoes[15][0] = azul;
  matCombinacoes[15][1] = azul;
  matCombinacoes[15][2] = azul;
  matCombinacoes[15][3] = azul;
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
  GRAFO *graf;
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
    }
    else
    {
      result = fscanf(arq, "%d%d%d", &orig, &dest, &peso);
      criaAresta(graf, orig, dest, peso);
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
    imprime(graf, subconjuntos,w);
    CorteMaximo(graf, subconjuntos, &cortemax, &subconjuntoCorteMax, w);
  }

  printf("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
  printf("\nO Corte-Maximo foi obtido no subconjunto de combinacao %d, e com valor de: %d\n", subconjuntoCorteMax+1, cortemax);
  imprime(graf,combinacoes[subconjuntoCorteMax],subconjuntoCorteMax);
  /*subconjuntos[0] = vermelho;
  subconjuntos[1] = azul;
  subconjuntos[2] = azul;
  subconjuntos[3] = vermelho;
  imprime(graf, subconjuntos);
  CorteMaximo(graf, subconjuntos);*/
  fclose(arq);
}
