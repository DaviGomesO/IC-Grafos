#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "operacoesGrafos.h"
#include "CorteMaximo.h"
#include "operacoesAG.h"

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

void main()
{
    FILE *arq;
    arq = fopen("Exemplo2.txt", "rt");
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

    int cortemax = 0, subconjuntoCorteMax = -1;

    // aqui ainda tenho que tratar para fazer as alternancias
    int totalPop = pow(2, graf->vertices);

    int tamPop = 0;
    int tamCromossomo = graf->vertices;
    int numgeracoes = 1000;
    float probCruzamento = 0.6, probMutacao = 1-probCruzamento;

    //iniciando a popula��o
    int **populacao = criaPopulacao(graf,&totalPop,&tamPop, tamCromossomo);
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

    int contaPos = 0, auxcont = tamPop;

    //usei o auxcont para inserir os primeiros cromossomos aleatorios da popula��o inicial

    while(auxcont > 0){
        for(int i = 0; i < tamPop; i++){
            //vai inserir na popula��o geral apenas os cromossomos que sejam diferentes
            conferirInserirPopulacao(populacaogeral,populacao[i],&contaPos,tamCromossomo);
            auxcont--;
        }
    }

    printf("\nSegue a lista da populacao geral, ao adicionar a populacao inicial:\nobs: a populacao geral esta iniciando sem nenhuma alocacao.\n");
    //imprimirMatriz(graf,populacaogeral,totalPop);
    imprimirMatriz(graf,populacaogeral,contaPos);

    int *melhoresValoresCorte = (int*)malloc(tamPop * sizeof(int));
    for(int i = 0; i < tamPop; i++)
        melhoresValoresCorte[i] = -1;

    apresentarMelhoresCromossomos(graf,melhoresValoresCorte,melhoresSolucoes, tamPop, populacaogeral, contaPos, tamCromossomo);

    int semconvergencias = 0;
    //agora vou fazer as muta��es/crossover's e inserir na popula��o
    //adicionei essa outra condicional para evitar de iterar quando atingir o n�mero m�ximo de possibilidades que certa quantidade de v�rtices possam gerar de solu��es
    for(int geracao = 0; geracao < numgeracoes && contaPos < totalPop; geracao++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\nGeracao %d:",geracao);
        //quando estiver a um certo periodo de gera��es sem converg�ncias na popula��o, ir� recriar e se nessa nova popula��o conter algum cromossomo diferente da popula��o geral, ser� utilizada esta.
        if(semconvergencias >= 10){
            printf("\nEsta sendo analisado uma nova populacao, pois a um certo periodo sem convergencia na populacao.\n");
            int **popAux = criaPopulacao(graf,&totalPop,&tamPop,tamCromossomo);
            int auxContaPos = contaPos;
            for(int i = 0; i<tamPop; i++){
                conferirInserirPopulacao(populacaogeral,popAux[i],&contaPos,tamCromossomo);
            }
            //caso nessa nova popula��o n�o apare�a nenhum cromossomo diferente, permanecer� a atual e ser� feito ou a muta��o ou o crossover ainda na atual.
            if(auxContaPos == contaPos){
                printf("\nNao surgiu nenhum novo cromossomo, portanto, segue-se com a mesma populacao que esta sendo utilizada atualmente.\n");
                semconvergencias++;
            }else{
                semconvergencias = 0;
                printf("\nNova populacao que foi criada apos periodo sem convergencia onde a mesma sera utilizada para as proximas geracoes:\n");
                imprimirMatriz(graf,popAux,tamPop);
                for(int i = 0; i<tamPop; i++){
                    novapopulacao[i] = popAux[i];
                }
            }
            system("pause");
        }
        if(geracao == 0){
            for(int i = 0; i < tamPop; i++){
                for(int j = tamCromossomo-1; j >= 0; j--){
                    novapopulacao[i][j] = populacao[i][j];
                }
                //CorteMaximo(graf,novapopulacao[i],&cortemax,&subconjuntoCorteMax,i);
            }
            printf("\nPopulacao que esta sendo trabalhada:\n");
            imprimirMatriz(graf,novapopulacao,tamPop);
            if(probCruzamento <= probMutacao)
                mutacao(novapopulacao,populacao,geracao,contaPos,tamCromossomo,&probCruzamento,&probMutacao);
            else
                crossover(novapopulacao,populacao,geracao,contaPos,tamCromossomo,&probCruzamento,&probMutacao);
            printf("\nNova populacao que sera trabalhada\n");
            imprimirMatriz(graf,novapopulacao,tamPop);
            printf("\n");
            int auxContaPos = contaPos;
            for(int i = 0; i<tamPop; i++){
                conferirInserirPopulacao(populacaogeral,novapopulacao[i],&contaPos,tamCromossomo);
            }
            //aqui estou utilizando para quando houver um certo numero de gera��es e a cada 10 gera��es, se n�o tiver entrado nenhum cromossomo diferente nas solu��es, mais na frente ele ir� recriar a popula��o para trabalhar;
            if(auxContaPos == contaPos){
                semconvergencias++;
            }else{
                semconvergencias = 0;
            }
        }else{
            int **populacaoaux = novapopulacao;
            printf("\nPopulacao que esta sendo trabalhada:\n");
            imprimirMatriz(graf,novapopulacao,tamPop);

            if(probMutacao >= probCruzamento){
                mutacao(novapopulacao,populacaoaux,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao);
            }else{
                crossover(novapopulacao,populacaoaux,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao);
            }
            printf("\nNova populacao que sera trabalhada\n");
            imprimirMatriz(graf,novapopulacao,tamPop);
            printf("\n");
            int auxContaPos = contaPos;
            for(int i = 0; i<tamPop; i++){
                conferirInserirPopulacao(populacaogeral,novapopulacao[i],&contaPos,tamCromossomo);
            }
            if(auxContaPos == contaPos){
                semconvergencias++;
            }else{
                semconvergencias = 0;
            }
        }
        printf("\nPopulacao geral:\n");
        //imprimirMatriz(graf,populacaogeral,totalPop);
        imprimirMatriz(graf,populacaogeral,contaPos);
        //conferir os cortesmax da popula��o geral
        apresentarMelhoresCromossomos(graf,melhoresValoresCorte,melhoresSolucoes, tamPop, populacaogeral, contaPos, tamCromossomo);
    }

    //calcula na geral
    /*for(int i = 0; i < contaPos; i++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        printf("\nCalculo do corte maximo no cromossomo %d da populacao geral.",i);
        CorteMaximo(graf,populacaogeral[i],&cortemax,&subconjuntoCorteMax,i);
    }
    printf("\n~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    printf("\nO Corte-Maximo foi obtido no cromossomo de posicao %d na populacao geral, e com valor de: %d\n", subconjuntoCorteMax, cortemax);
    imprime(grafreserva,populacaogeral[subconjuntoCorteMax],subconjuntoCorteMax);
    */

    //calcula entre os maiores cortes de cromossomos no conjunto selecionado
    int cortemaxMelhores = 0, posCromossomoMelhoresCorteMax = -1;
    for(int i = 0; i < tamPop; i++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        printf("\nCalculo do corte maximo no cromossomo %d entre os melhores cromossomos selecionados.",i);
        CorteMaximo(graf,melhoresSolucoes[i],&cortemaxMelhores,&posCromossomoMelhoresCorteMax,i);
    }
    printf("\n~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    printf("\nO Corte-Maximo foi obtido no cromossomo de posicao %d no conjunto dos melhores cortes de cromossomos, e com valor de: %d\n", posCromossomoMelhoresCorteMax, cortemaxMelhores);
    imprime(grafreserva,melhoresSolucoes[posCromossomoMelhoresCorteMax],posCromossomoMelhoresCorteMax);

    fclose(arq);
}
