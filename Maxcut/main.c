#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "operacoesGrafos.h"
#include "CorteMaximo.h"

void imprime(GRAFO *gr, int *estruturaSubC, int w)
{
  printf("Vertices: %d.\nArestas: %d. \nLista de adjacencia da combinacao %d\n", gr->vertices, gr->arestas,w); // imprime numero de v�rtice e arestas

  for (int i = 0; i < gr->vertices; i++)
  {
    printf("vertice %d ", i + 1); // Imprimo em qual aresta estou
    // e verifico em qual subconjunto ela est�, atrav�s do vetor de identifica��o
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
      printf("- (N�o est� em nenhum subconjunto): ");
    }
    ADJACENCIA *ad = gr->adj[i].cab; // chamo a cabe�a da lista de adjacencia desta aresta
    while (ad)
    {                                                // enquanto as adjacencias n�o forem nula
      printf("v%d(%d) ", ad->vertice + 1, ad->peso); // imprimo a adjacencia e seu peso
      ad = ad->prox;                                 // passo para proxima adjacencia
    }
    printf("\n");
  }
}

int ** criaPopulacao(GRAFO *gr, int totalPop, int *tamPopul, int tamCromossomo){
    int aleat;
    if(gr->vertices < 3){
        (*tamPopul) = totalPop;
    }else if(gr->vertices >= 3 && gr->vertices < 10){
        (*tamPopul) = (totalPop/4);
    }else if(gr->vertices >= 10 && gr->vertices < 20){
        (*tamPopul) = (totalPop/8);
    }else{
        (*tamPopul) = 100000;
    }
    int **populacao = (int**)malloc((*tamPopul) * sizeof(int *));
    for(int i = 0; i < (*tamPopul); i++){
        populacao[i] = (int*)malloc(tamCromossomo * sizeof(int));
    }

    //gera��o da popula��o aleatoria
    srand(time(NULL));
    int cont = 1;
    for(int i = 0; i < (*tamPopul); i++){
        for(int j = tamCromossomo-1; j >= 0; j--){
            aleat = (rand() % 10)%2;
            //printf("aleat %d = %d\n",cont,aleat);
            if(aleat == 0){
                populacao[i][j] = azul;
            }else if(aleat == 1){
                populacao[i][j] = vermelho;
            }
            cont++;
        }
    }
    return populacao;
}

void inserirNaGeral(int **populacaogeral, int tamPop, int **populacao, int tamCromossomo){
    int *indiv = (int*)malloc(tamCromossomo * sizeof(int));
    for(int i = 0; i < tamPop; i++){
        int indicapos = 0;
        int cont = 0;
        for(int j = tamCromossomo-1; j >= 0; j--){
            indiv[j] = populacao[i][j];
            if(populacao[i][j] == azul){
                indicapos += (pow(2,cont)*0);
            }else if(populacao[i][j] == vermelho){
                indicapos += (pow(2,cont)*1);
            }
            cont++;
        }

        //printf("posicao adicionada %d da populacao geral\n",indicapos);
        for(int j = tamCromossomo-1; j >= 0; j--){
            populacaogeral[indicapos][j] = indiv[j];
        }
    }
}

void mutacao(int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo){
    int cromossomoaleat = tamPop+1, genealeat = tamCromossomo+1;
    srand(geracao);
    while(cromossomoaleat > tamPop){
        cromossomoaleat = rand()%tamPop;
    }
    while(genealeat > tamCromossomo){
        genealeat = rand()%tamCromossomo;
    }
    if(geracao == 0){
        printf("\nAlterando no gene %d do cromossomo %d da populacao inicial.\n",genealeat+1,cromossomoaleat);
    }else{
        printf("\nAlterando no gene %d do cromossomo %d da geracao anterior.\n",genealeat+1,cromossomoaleat);
    }

    if(novapopulacao[cromossomoaleat][genealeat] == azul){
        printf("(azul -> vermelho)\n");
        novapopulacao[cromossomoaleat][genealeat] = vermelho;
    }else if(populacao[cromossomoaleat][genealeat] == vermelho){
        printf("(vermelho -> azul)\n");
        novapopulacao[cromossomoaleat][genealeat] = azul;
    }
}

void main()
{
    FILE *arq;
    arq = fopen("Exemplo.txt", "rt");
    GRAFO *graf, *grafreserva;
    char *result;
    int linha;

    // Se houver erro na abertura
    if (arq == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
        return;
    }
    linha = 1;

    //utilizei o graf reserva para construir as arestas de ida e volta, como se n�o fosse direcionado
    // o graf ser� para apresentar apenas as arestas uma �nica vez, e em ordem crescente
    int orig, dest, peso;
    int qtdvert, qtdaresta;

    while (!feof(arq))
    {
    // L� uma linha (inclusive com o '\n')
    // Se foi poss�vel ler
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

    int cortemax = 0, subconjuntoCorteMax = -1, MenorCorteMax = -1, subconjuntoMenorCorteMax = -1;

    int *subconjuntos = (int *)malloc(graf->vertices * sizeof(int));
    // aqui ainda tenho que tratar para fazer as alternancias
    int totalPop = pow(2, graf->vertices);

    int tamPop = 0;
    int tamCromossomo = graf->vertices;
    int numgeracoes = 10;
    float probCruzamento = 0.95, probMutacao = 0.1;
    //iniciando a popula��o
    int **populacao = criaPopulacao(graf,totalPop,&tamPop, tamCromossomo);
    imprimirMatriz(graf,populacao,tamPop);

    //int *aptidao = (int*)malloc(tamPop * sizeof(int));
    int **novapopulacao = (int**)malloc(tamPop * sizeof(int*));
    int **melhoresSolucoes = (int**)malloc(tamPop * sizeof(int*));
    for(int i = 0; i<tamPop; i++){
        novapopulacao[i] = (int*)malloc(tamCromossomo*sizeof(int));
        melhoresSolucoes[i] = (int*)malloc(tamCromossomo*sizeof(int));
    }

    int **populacaogeral = (int**)malloc(totalPop * sizeof(int*));
    for(int i = 0; i < totalPop; i++){
        populacaogeral[i] = (int*)malloc(graf->vertices * sizeof(int));
    }

    //inserir a populacao inicial na lista de combina��es/popula��o geral
    //por meio de inserir em cada posi��o ou com o auxilio de um contador para controlar quantas posi��es ja foram inseridas
    inserirNaGeral(populacaogeral,tamPop,populacao,tamCromossomo);

    printf("\nSegue a lista da populacao geral, ao adicionar a populacao inicial:\nobs: a populacao geral esta iniciando sem nenhuma alocacao.\n");
    imprimirMatriz(graf,populacaogeral,totalPop);

    for(int geracao = 0; geracao < numgeracoes; geracao++){
        printf("=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        //int cromossomoaleat = tamPop+1, genealeat = tamCromossomo+1;
        if(geracao == 0){
            for(int i = 0; i < tamPop; i++){
                for(int j = tamCromossomo-1; j >= 0; j--){
                    novapopulacao[i][j] = populacao[i][j];
                }
            }
            mutacao(novapopulacao,populacao,geracao,tamPop,tamCromossomo);
            imprimirMatriz(graf,novapopulacao,tamPop);
            inserirNaGeral(populacaogeral,tamPop,novapopulacao,tamCromossomo);

            /*for(int i = 0; i < tamPop; i++){
                CorteMaximo(graf,novapopulacao[i],&cortemax,&subconjuntoCorteMax,i);
            }*/

            printf("\nPopulacao geral apos a adicao da nova geracao:\n");
            imprimirMatriz(graf,populacaogeral,totalPop);
        }else{
            mutacao(novapopulacao,populacao,geracao,tamPop,tamCromossomo);
            imprimirMatriz(graf,novapopulacao,tamPop);
            inserirNaGeral(populacaogeral,tamPop,novapopulacao,tamCromossomo);

            /*for(int i = 0; i < tamPop; i++){
                CorteMaximo(graf,novapopulacao[i],&cortemax,&subconjuntoCorteMax,i);
            }*/

            printf("\nPopulacao geral apos a adicao da nova geracao:\n");
            imprimirMatriz(graf,populacaogeral,totalPop);

        }
    }

    for(int i = 0; i < totalPop; i++){
        printf("=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        printf("\nCalculo do corte maximo no cromossomo %d da populacao geral.",i);
        CorteMaximo(graf,populacaogeral[i],&cortemax,&subconjuntoCorteMax,i);
    }
    printf("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    printf("\nO Corte-Maximo foi obtido no cromossomo de posicao %d na populacao geral, e com valor de: %d\n", subconjuntoCorteMax, cortemax);
    imprime(grafreserva,populacaogeral[subconjuntoCorteMax],subconjuntoCorteMax);

    fclose(arq);
}
