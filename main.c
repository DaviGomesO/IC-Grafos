#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "operacoesGrafos.h"
#include "CorteMaximo.h"

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

int ** criaPopulacao(GRAFO *gr, int elev, int *tamPopul, int tamCromossomo){
    int aleat;
    if(gr->vertices < 3){
        (*tamPopul) = elev;
    }else if(gr->vertices >= 3 && gr->vertices < 10){
        (*tamPopul) = (elev/4);
    }else if(gr->vertices >= 10 && gr->vertices < 20){
        (*tamPopul) = (elev/8);
    }else{
        (*tamPopul) = 100000;
    }
    int **populacao = (int**)malloc((*tamPopul) * sizeof(int *));
    for(int i = 0; i < (*tamPopul); i++){
        populacao[i] = (int*)malloc(tamCromossomo * sizeof(int));
    }

    //geração da população aleatoria
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
            ;
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

    //utilizei o graf reserva para construir as arestas de ida e volta, como se não fosse direcionado
    // o graf será para apresentar apenas as arestas uma única vez, e em ordem crescente
    int orig, dest, peso;
    int qtdvert, qtdaresta;

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

    int cortemax = 0, subconjuntoCorteMax = -1, MenorCorteMax = -1, subconjuntoMenorCorteMax = -1;

    int *subconjuntos = (int *)malloc(graf->vertices * sizeof(int));
    // aqui ainda tenho que tratar para fazer as alternancias
    int elev = pow(2, graf->vertices);
    /*
    int **combinacoes = (int **)malloc(elev * sizeof(int *));
    for (int i = 0; i < elev; i++)
    {
        combinacoes[i] = (int *)malloc(graf->vertices * sizeof(int));
    }
    */
    int tamPop = 0;
    int tamCromossomo = graf->vertices;
    int numgeracoes = 10;
    float probCruzamento = 0.95, probMutacao = 0.1;
    int **populacao = criaPopulacao(graf,elev,&tamPop, tamCromossomo);
    imprimirMatriz(graf,populacao,tamPop);


    //int *individuo = (int*)malloc(tamPop * sizeof(int));
    //int *aptidao = (int*)malloc(tamPop * sizeof(int));
    int **novageracao = (int**)malloc(tamPop * sizeof(int*));
    int **melhoresSolucoes = (int**)malloc(tamPop * sizeof(int*));
    for(int i = 0; i<tamPop; i++){
        novageracao[i] = (int*)malloc(tamCromossomo*sizeof(int));
        melhoresSolucoes[i] = (int*)malloc(tamCromossomo*sizeof(int));
    }

    int **populacaogeral = (int**)malloc(elev * sizeof(int*));
    for(int i = 0; i < elev; i++){
        populacaogeral[i] = (int*)malloc(graf->vertices * sizeof(int));
    }

    //inserir a populacao inicial na lista de combinações/população geral
    //por meio de inserir em cada posição ou com o auxilio de um contador para controlar quantas posições ja foram inseridas
    inserirNaGeral(populacaogeral,tamPop,populacao,tamCromossomo);

    printf("\nSegue a lista da populacao geral, ao adicionar a populacao inicial:\nobs: a populacao geral esta iniciando sem nenhuma alocacao.\n");
    imprimirMatriz(graf,populacaogeral,elev);

    for(int geracao = 0; geracao < numgeracoes; geracao++){
        printf("=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        if(geracao == 0){
            for(int i = 0; i < tamPop; i++){
                for(int j = tamCromossomo-1; j >= 0; j--){
                    novageracao[i][j] = populacao[i][j];
                }
            }
            //mutação
            int posaleat = tamPop+1, cromossomoaleat=tamCromossomo+1;
            srand(time(NULL));
            while(posaleat > tamPop){
                posaleat = rand()%tamPop;
            }
            while(cromossomoaleat > tamCromossomo){
                cromossomoaleat = rand()%tamCromossomo;
            }
            printf("\nAlterando na posicao %d e no cromossomo %d da populacao inicial.\n",posaleat,cromossomoaleat+1);
            if(novageracao[posaleat][cromossomoaleat] == azul){
                novageracao[posaleat][cromossomoaleat] = vermelho;
            }else if(populacao[posaleat][cromossomoaleat] == vermelho){
                novageracao[posaleat][cromossomoaleat] = azul;
            }
            imprimirMatriz(graf,novageracao,tamPop);
            inserirNaGeral(populacaogeral,tamPop,novageracao,tamCromossomo);

            printf("\nPopulacao geral apos a adicao da nova geracao:\n");
            imprimirMatriz(graf,populacaogeral,elev);
        }else{
            //mutação
            int posaleat = tamPop+1, cromossomoaleat=tamCromossomo+1;
            srand(geracao);
            while(posaleat > tamPop){
                posaleat = rand()%tamPop;
            }
            while(cromossomoaleat > tamCromossomo){
                cromossomoaleat = rand()%tamCromossomo;
            }
            printf("\nAlterando na posicao %d e no cromossomo %d da geracao anterior.\n",posaleat,cromossomoaleat+1);
            if(novageracao[posaleat][cromossomoaleat] == azul){
                novageracao[posaleat][cromossomoaleat] = vermelho;
            }else if(populacao[posaleat][cromossomoaleat] == vermelho){
                novageracao[posaleat][cromossomoaleat] = azul;
            }
            imprimirMatriz(graf,novageracao,tamPop);
            inserirNaGeral(populacaogeral,tamPop,novageracao,tamCromossomo);

            printf("\nPopulacao geral apos a adicao da nova geracao:\n");
            imprimirMatriz(graf,populacaogeral,elev);
        }
    }

    for(int i = 0; i < elev; i++){
        printf("=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        printf("\nCalculo do corte maximo na posicao %d da populacao geral.",i);
        CorteMaximo(graf,populacaogeral[i],&cortemax,&subconjuntoCorteMax,i);
    }

    /*
    int geracoes = 0;

    while(geracoes < numgeracoes){
        if(geracoes == 0){
            novaGeracao = populacao;
        }else{
            int posaleat = tamPop+1, cromossomoaleat=tamCromossomo+1;
            srand(geracoes);
            while(posaleat > tamPop){
                posaleat = rand()%tamPop;
            }
            while(cromossomoaleat > tamCromossomo){
                cromossomoaleat = rand()%tamCromossomo;
            }
            if(populacao[posaleat][cromossomoaleat] == azul){
                populacao[posaleat][cromossomoaleat] = vermelho;
            }else if(populacao[posaleat][cromossomoaleat] == vermelho){
                populacao[posaleat][cromossomoaleat] = azul;
            }
            novaGeracao = populacao;
        }
        imprimirMatriz(graf,novaGeracao,tamPop);
        for(int linhapop = 0; linhapop < tamPop; linhapop++){
            for(int col = 0; col < tamCromossomo; col++){
                indiv[col] = populacao[linhapop][col];
            }
            //imprime(grafreserva,indiv,linhapop);
            CorteMaximo(graf,indiv,&cortemax,&subconjuntoCorteMax,linhapop);
            if(linhapop == 0){
                MenorCorteMax = cortemax;
                subconjuntoMenorCorteMax = subconjuntoCorteMax;
            }else{
                if(MenorCorteMax > cortemax){
                    MenorCorteMax = cortemax;
                    subconjuntoMenorCorteMax = subconjuntoCorteMax;
                }
            }
        }
        printf("Ate a geracao %d,o menor corte maximo eh : %d, e esta no subconjunto %d.\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n",geracoes+1,MenorCorteMax,subconjuntoMenorCorteMax+1);
        geracoes++;
    }
    */

    /*preencheMatriz(graf, combinacoes, elev);
    imprimirMatriz(graf, combinacoes, elev);
    printf("\n");

    for (int w = 0; w < elev; w++)
    {
        printf("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
        for (int c = 0; c < graf->vertices; c++)
        {
        //subconjuntos[c] = combinacoes[w][c];
        subconjuntos[c] = populacao[w][c];
        }
        imprime(grafreserva, subconjuntos,w);
        CorteMaximo(graf, subconjuntos, &cortemax, &subconjuntoCorteMax, w);
    }

    printf("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    printf("\nO Corte-Maximo foi obtido no subconjunto de combinacao %d, e com valor de: %d\n", subconjuntoCorteMax+1, cortemax);
    //imprime(grafreserva,combinacoes[subconjuntoCorteMax],subconjuntoCorteMax);
    imprime(grafreserva,populacao[subconjuntoCorteMax],subconjuntoCorteMax);
    */
    fclose(arq);
}
