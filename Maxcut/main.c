#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "operacoesGrafos.h"
#include "CorteMaximo.h"

void imprime(GRAFO *gr, int *estruturaSubC, int w)
{
  printf("Vertices: %d.\nArestas: %d. \nLista de adjacencia da combinacao %d\n", gr->vertices, gr->arestas,w); // imprime numero de vértice e arestas

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
    //função usada para adicionar quando utilizar a alocação referente a posição tendo em vista o número binário
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
    //esse formato consegui deixar mais aleatorio sem apresentar sorteios viciados
    srand(rand());
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

    if(populacao[cromossomoaleat][genealeat] == azul){
        printf("(azul -> vermelho)\n");
        novapopulacao[cromossomoaleat][genealeat] = vermelho;
    }else if(populacao[cromossomoaleat][genealeat] == vermelho){
        printf("(vermelho -> azul)\n");
        novapopulacao[cromossomoaleat][genealeat] = azul;
    }
}

int conferirCromossomo(int *populacaogeral, int *populacao, int tamCromossomo){
    int aux = 0;
    for(int gene = tamCromossomo-1; gene >= 0; gene--){
        if(populacaogeral[gene] == populacao[gene]){
            aux++;
        }else{
            aux--;
        }
    }
    //printf("tam cromossomo: %d x genes iguais: %d\n", tamCromossomo, aux);
    return aux;
    //se aux for igual ao tamanho do cromossomo, quer dizer que os dois são iguais em todas os genes
}

void conferirInserirPopulacao(int **populacaogeral, int *populacao, int *contaPos, int tamCromossomo){
    //essa função vai ser usada para quando estivermos adicionando o elemento da população em qualquer posição da população da tabela, de acordo com sua ordem de aparição
    //consistindo em adicionar o cromossomo apenas quando a mesma não exista na geral
    int podeinserir = -1;
    for(int auxcromossomo = 0; auxcromossomo <= (*contaPos); auxcromossomo++){
        //vai conferir se os cromossomos estão iguais ou não
        if(conferirCromossomo(populacaogeral[auxcromossomo], populacao, tamCromossomo) == tamCromossomo){
            podeinserir = 0;
            break;
        }else{
            podeinserir = 1;
        }
    }
    if(podeinserir == 1){
        for(int j = tamCromossomo-1; j >= 0; j--){
            populacaogeral[(*contaPos)][j] = populacao[j];
        }
        (*contaPos)++;
    }
}

int menorcorteMelhores(int *vetorComCortedosMelhores, int tamPop){
    //função para indicar em qual posição do vetor contendo os melhores cromossomos está o menor corte.
    int menor = vetorComCortedosMelhores[0], cromossomoDoMenor = 0;
    for(int i = 1; i<tamPop; i++){
        //printf("%d ",vetorComCortedosMelhores[i]);
        if(menor > vetorComCortedosMelhores[i]){
            menor = vetorComCortedosMelhores[i];
            cromossomoDoMenor = i;
        }
    }
    printf("\nmenor valor dentro dos melhores cromossomos: %d (esta no cromossomo %d)\n",vetorComCortedosMelhores[cromossomoDoMenor],cromossomoDoMenor);
    return cromossomoDoMenor;
}

void alocarMelhoresSolucoes(GRAFO *gr, int *melhoresValoresCorte, int **melhoressolucoes, int tamPop, int **populacaogeral, int contaPos, int tamCromossomo){
    //aqui está alocando a população na matrix que contém as melhores
    if(contaPos <= tamPop){
        //enquanto todas as posições não forem alocadas do vetor com os melhores cromossomos, ele irá receber a população que existe
        for(int i = 0; i < contaPos; i++){
            for(int j = tamCromossomo-1; j >= 0; j--){
                melhoressolucoes[i][j] = populacaogeral[i][j];
            }
            melhoresValoresCorte[i] = corteDoCromossomo(gr,melhoressolucoes[i]);
        }
    }else{
        //quando todas as posições forem preenchidas, iremos fazer comparação entre o menor corte dentre esses melhores cromossomos com o corte do cromossomo que vem aparecendo na população geral
        int posMenor = menorcorteMelhores(melhoresValoresCorte,tamPop);
        for(int i = tamPop; i < contaPos; i++){
            int corteCromossomo = corteDoCromossomo(gr,populacaogeral[i]);
            //comparar se o corte desse cromossomo é maior que o menor dentro do conjunto solução
            //se for adiciona no lugar

            printf("\nvalor do corte do cromossomo %d: %d\n",i,corteCromossomo);
            if(corteCromossomo > melhoresValoresCorte[posMenor]){
                printf("\nPortanto, como o cromossomo %d que tem o menor valor entre o conjunto atual de melhores cromossomos, com  valor: %d\n",posMenor,melhoresValoresCorte[posMenor]);
                printf("e como o cromossomo %d da populacao geral tem valor de corte maior que o menor dentre os cromossomos com os maiores cortemax(%d > %d) faz-se a troca.",i,corteCromossomo,melhoresValoresCorte[posMenor]);
                for(int j = tamCromossomo-1; j>=0; j--){
                    melhoressolucoes[posMenor][j] = populacaogeral[i][j];
                    melhoresValoresCorte[posMenor] = corteCromossomo;
                    //aqui estou invertendo as posicoes, colocando os melhores no inicio da população geral
                    // e alocando aqueles que foram melhores antes no cromossomo de contagem que apareceu um maior
                    int aux = populacaogeral[posMenor][j];
                    populacaogeral[posMenor][j] = populacaogeral[i][j];
                    populacaogeral[i][j] = aux;
                }
            }
        }
    }
}

void apresentarMelhoresCromossomos(GRAFO *graf, int *melhoresValoresCorte, int **melhoresSolucoes, int tamPop, int **populacaogeral, int contaPos, int tamCromossomo){
    printf("\nOs Cromossomos com os melhores cortes sao:\n");
    alocarMelhoresSolucoes(graf,melhoresValoresCorte,melhoresSolucoes, tamPop, populacaogeral, contaPos, tamCromossomo);
    printf("\n");
    imprimirMatriz(graf,melhoresSolucoes,tamPop);
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

    int cortemax = 0, subconjuntoCorteMax = -1;
    int corteMin = 0, subconjuntoCorteMIN = -1;

    int *subconjuntos = (int *)malloc(graf->vertices * sizeof(int));
    // aqui ainda tenho que tratar para fazer as alternancias
    int totalPop = pow(2, graf->vertices);

    int tamPop = 0;
    int tamCromossomo = graf->vertices;
    int numgeracoes = 100;
    float probCruzamento = 0.95, probMutacao = 0.1;

    //iniciando a população
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

    //inserir a populacao inicial na lista de combinações/população geral
    //por meio de inserir em cada posição ou com o auxilio de um contador para controlar quantas posições ja foram inseridas

    int contaPos = 0, auxcont = tamPop;

    //usei o auxcont para inserir os primeiros cromossomos aleatorios da população inicial

    while(auxcont > 0){
        for(int i = 0; i < tamPop; i++){
            //vai inserir na população geral apenas os cromossomos que sejam diferentes
            conferirInserirPopulacao(populacaogeral,populacao[i],&contaPos,tamCromossomo);
            auxcont--;
        }
    }

    printf("\nSegue a lista da populacao geral, ao adicionar a populacao inicial:\nobs: a populacao geral esta iniciando sem nenhuma alocacao.\n");
    imprimirMatriz(graf,populacaogeral,totalPop);

    int *melhoresValoresCorte = (int*)malloc(tamPop * sizeof(int));

    apresentarMelhoresCromossomos(graf,melhoresValoresCorte,melhoresSolucoes, tamPop, populacaogeral, contaPos, tamCromossomo);

    //agora vou fazer as mutações e inserir na população
    for(int geracao = 0; geracao < numgeracoes; geracao++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        if(geracao == 0){
            for(int i = 0; i < tamPop; i++){
                for(int j = tamCromossomo-1; j >= 0; j--){
                    novapopulacao[i][j] = populacao[i][j];
                }
                //CorteMaximo(graf,novapopulacao[i],&cortemax,&subconjuntoCorteMax,i);
            }
            imprimirMatriz(graf,novapopulacao,tamPop);
            mutacao(novapopulacao,populacao,geracao,contaPos,tamCromossomo);
            imprimirMatriz(graf,novapopulacao,tamPop);
            printf("\n");
            for(int i = 0; i<tamPop; i++){
                conferirInserirPopulacao(populacaogeral,novapopulacao[i],&contaPos,tamCromossomo);
            }
            imprimirMatriz(graf,populacaogeral,totalPop);
        }else{
            int **populacaoaux = novapopulacao;
            imprimirMatriz(graf,novapopulacao,tamPop);
            mutacao(novapopulacao,populacaoaux,geracao,tamPop,tamCromossomo);
            imprimirMatriz(graf,novapopulacao,tamPop);
            printf("\n");
            for(int i = 0; i<tamPop; i++){
                conferirInserirPopulacao(populacaogeral,novapopulacao[i],&contaPos,tamCromossomo);
            }
            imprimirMatriz(graf,populacaogeral,totalPop);
        }
        //conferir os cortesmax da população geral
        apresentarMelhoresCromossomos(graf,melhoresValoresCorte,melhoresSolucoes, tamPop, populacaogeral, contaPos, tamCromossomo);
    }

    for(int i = 0; i < tamPop; i++)
        printf("\nvalor de corte do cromossomo %d entre os melhores: %d",i,melhoresValoresCorte[i]);

    for(int i = 0; i < contaPos; i++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        printf("\nCalculo do corte maximo no cromossomo %d da populacao geral.",i);
        CorteMaximo(graf,populacaogeral[i],&cortemax,&subconjuntoCorteMax,i);
    }
    printf("\n~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    printf("\nO Corte-Maximo foi obtido no cromossomo de posicao %d na populacao geral, e com valor de: %d\n", subconjuntoCorteMax, cortemax);
    imprime(grafreserva,populacaogeral[subconjuntoCorteMax],subconjuntoCorteMax);

    fclose(arq);
}
