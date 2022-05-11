#ifndef CORTEMAXIMO_H_INCLUDED
#define CORTEMAXIMO_H_INCLUDED

#define azul 2
#define vermelho 3

void preencheMatriz(GRAFO *gr, int **matCombinacoes, int totalPop)
{
  for(int i = 0; i < totalPop; i++){
    int aux_num = i;
    for(int aux = gr->vertices-1; aux >= 0; aux--){
        if(aux_num%2 == 0){
            matCombinacoes[i][aux] = azul;
        }else if(aux_num%2 == 1){
            matCombinacoes[i][aux] = vermelho;
        }
        aux_num = aux_num/2;
    }
  }
}

void imprimirMatriz(GRAFO *gr, int **matCombinacoes, int totalPop, int *valoresCortes)
{
  for (int i = 0; i < totalPop; i++)
  {
    printf("Cromossomo %d - ", i);
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
        printf(" ");
      }
      printf("] ");
    }
    printf(" - valor de corte deste cromossomo: %d\n",valoresCortes[i]);
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
  //printf("\nCorte Maximo: %d.\nCorte Interno entre as arestas no subconjunto vermelho: %d\nCorte interno entre as arestas do subconjunto azul: %d\n\n", pesoMaxExt, PesoMaxIntVer, PesoMaxIntAzul);
  if(pesoMaxExt > (*cortemax)){
    (*cortemax) = pesoMaxExt;
    (*subconjCorteMax) = subconjuntodavez;
  }
}

int corteDoCromossomo(GRAFO *gr, int *estruturaSubConj){
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
  return pesoMaxExt;
}

#endif // CORTEMAXIMO_H_INCLUDED
