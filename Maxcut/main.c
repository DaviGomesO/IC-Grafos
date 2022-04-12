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
  printf("Vertices: %d.\nArestas: %d. \nLista de adjacencia do cromossomo %d\n", gr->vertices, gr->arestas,w); // imprime numero de vértice e arestas

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
      printf("- (Não esta em nenhum subconjunto): ");
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

//tratar para cortes negativos
void quick_sort(int *vetorComValores, int **populacao, int esquerda, int direita, int tamCromossomo) {
    int i, j, x, y;

    i = esquerda;
    j = direita;
    x = vetorComValores[(esquerda + direita) / 2];

    while(i <= j) {
        while(vetorComValores[i] < x && i < direita) {
            i++;
        }
        while(vetorComValores[j] > x && j > esquerda) {
            j--;
        }
        if(i <= j) {
            y = vetorComValores[i];
            vetorComValores[i] = vetorComValores[j];
            vetorComValores[j] = y;
            for(int gene = tamCromossomo-1; gene >= 0; gene--){
                int aux = populacao[i][gene];
                populacao[i][gene] = populacao[j][gene];
                populacao[j][gene] = aux;
            }
            i++;
            j--;
        }
    }

    if(j > esquerda) {
        quick_sort(vetorComValores, populacao, esquerda, j, tamCromossomo);
    }
    if(i < direita) {
        quick_sort(vetorComValores, populacao, i, direita, tamCromossomo);
    }
}

void insertion_sort(int *vetorComValores, int **populacao,int tam, int tamCromossomo){
    int i, j, valor;

    for(i = 1; i < tam; i++){
        valor = vetorComValores[i];
        for(j = i-1; j >= 0 && valor > vetorComValores[j]; j--){
            vetorComValores[j+1] = vetorComValores[j];
            for(int gene = tamCromossomo-1; gene >= 0; gene--){
                int aux = populacao[j+1][gene];
                populacao[j+1][gene] = populacao[j][gene];
                populacao[j][gene] = aux;
            }
        }
        vetorComValores[j+1] = valor;
    }
}

void main()
{
    FILE *arq;
    FILE *info;
    arq = fopen("Testes/g05_60_0.txt", "rt");
    info = fopen("informações g05_60_0 com crossover prevalecendo os genes iguais.txt", "wt");
    GRAFO *graf, *grafreserva;
    char *result;
    int linha;

    // Se houver erro na abertura
    if (arq == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
        return;
    }
    if(info != NULL)
        printf("Arquivo de informacoes criado com sucesso!!\n");
    else{
        printf("Problemas na criação do arquivo\n");
        return 1;
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
          //criaAresta(graf, orig, dest, peso,0);
          //criaAresta(grafreserva, orig, dest, peso,1);
          criaAresta(graf, orig-1, dest-1, peso,0);
          criaAresta(grafreserva, orig-1, dest-1, peso,1);
        }
        linha++;
      }

    int cortemax = 0, subconjuntoCorteMax = -1;

    // aqui ainda tenho que tratar para fazer as alternancias
    int totalPop = pow(2, graf->vertices);

    int tamPop = 0;
    int tamCromossomo = graf->vertices;
    int numgeracoes = 100; //30 ~ 300
    float probCruzamento = 0.6, probMutacao = 1-probCruzamento;

    //iniciando a população
    //geracao 0
    printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\nGeracao inicial:\n");
    int **populacao = criaPopulacao(graf,&totalPop,&tamPop, tamCromossomo);
    int *valoresCortesPopulacao = (int*)malloc(tamPop*sizeof(int));
    //já cálculo os valores de corte dessa população
    for(int cromossomo = 0; cromossomo < tamPop; cromossomo++){
        valoresCortesPopulacao[cromossomo] = corteDoCromossomo(graf, populacao[cromossomo]);
    }

    imprimirMatriz(graf,populacao,tamPop,valoresCortesPopulacao);
    //ordena essa população em ordem decrescente
    insertion_sort(valoresCortesPopulacao, populacao, tamPop, tamCromossomo);
    printf("\nApos a ordenacao:\n");
    imprimirMatriz(graf, populacao, tamPop, valoresCortesPopulacao);

    //crio o vetor novapopulaçãp com o tamanho equivalente a quantidade do tamPop mais metade da população, ou o mesmo que 1,5 * tamPop
    //para utilizar o mesmo mais na frente criando os cromossomos, ordenar e pegar os 'tamPop' melhores
    int **novapopulacao = (int**)malloc((tamPop + (tamPop/2)) * sizeof(int*));
    for(int i = 0; i<(tamPop + (tamPop/2)); i++){
        novapopulacao[i] = (int*)malloc(tamCromossomo*sizeof(int));
    }

    //int posMaior = maiorCorteMelhores(valoresCortesPopulacao, tamPop);
    int maiorCorte = valoresCortesPopulacao[0];
    fprintf(info, "O maior valor de corte da população inicial é de: %d.\n",maiorCorte);
    int diferenca = 0, geracaoDeAlteracaoDoMaior = 0, diferencaTotal = 0;
    int semconvergencias = 0;

    //aqui estou passando as primeiras posições para o vetor que vai gerar novos cromossomos
    for(int i = 0; i < tamPop; i++){
        for(int j = tamCromossomo-1; j >= 0; j--){
            novapopulacao[i][j] = populacao[i][j];
        }
    }
    // e passando a copia do vetor com os valores de corte, já ordenado
    int *valoresCortesNovaPopulacao = (int*)malloc((tamPop + (tamPop/2))*sizeof(int));
    for(int cromossomo = 0; cromossomo < tamPop; cromossomo++){
        valoresCortesNovaPopulacao[cromossomo] = valoresCortesPopulacao[cromossomo];
    }


    for(int geracao = 1; geracao <= numgeracoes; geracao++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\nGeracao %d:",geracao);

        //quando estiver a um certo periodo de gerações sem convergências na população, irá recriar e se nessa nova população conter algum cromossomo diferente da população geral, será utilizada esta.
        //utilizo para fazer o código parar e não gerar mais populações
        if(semconvergencias >= numgeracoes/5){
            printf("Parou devido a falta de convergencia.");
            fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
            fprintf(info, "\nDevido um periodo de %d gerações sem convergência na população, o código parou na geração %d.", numgeracoes/5,geracao);
            break;
        }

        printf("\nPopulacao que esta sendo trabalhada:\n");
        //imprimirMatriz(graf,populacao,tamPop,valoresCortesNovaPopulacao);

        if(probCruzamento <= probMutacao){
            mutacao(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao, valoresCortesNovaPopulacao);
        }else{
            //crossover(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao,valoresCortesNovaPopulacao);
            //crossoverPercentual(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao,valoresCortesNovaPopulacao);
            crossoverPrevaleceIgualdade(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao,valoresCortesNovaPopulacao);
        }

        printf("\nNova populacao gerada\n");
        //imprimirMatriz(graf,novapopulacao,(tamPop+(tamPop/2)),valoresCortesNovaPopulacao);

        printf("\nOrdenando essa populacao...\n");
        insertion_sort(valoresCortesNovaPopulacao,novapopulacao,(tamPop+(tamPop/2)),tamCromossomo);
        //imprimirMatriz(graf,novapopulacao,(tamPop+(tamPop/2)),valoresCortesNovaPopulacao);


        printf("\nNova populacao que sera trabalhada\n");
        for(int i = 0; i < tamPop; i++){
            for(int j = tamCromossomo-1; j >= 0; j--){
                populacao[i][j] = novapopulacao[i][j];
            }
            valoresCortesPopulacao[i] = valoresCortesNovaPopulacao[i];
        }
        //imprimirMatriz(graf,populacao,tamPop,valoresCortesPopulacao);

        //int posMaiorAux = maiorCorteMelhores(valoresCortesPopulacao, tamPop);
        int maiorCorteAux = maiorCorte;
        //verifico se o topo está sendo alterado
        if(maiorCorteAux < valoresCortesPopulacao[0]){
            maiorCorte = valoresCortesPopulacao[0];
            diferenca = maiorCorte - maiorCorteAux;
            diferencaTotal += diferenca;
            fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
            fprintf(info,"\nNa geracao %d o maior corte maximo sera %d, substituindo o antigo maior corte maximo, que tem valor de %d, e foi encontrado na geracao %d.\n", geracao, maiorCorte, maiorCorteAux,geracaoDeAlteracaoDoMaior);
            fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
            geracaoDeAlteracaoDoMaior = geracao;
            semconvergencias = 0;
        }else{
            semconvergencias++;
        }

        //verificar se houve convergência observando apenas o topo
    }


    //calcula entre os maiores cortes de cromossomos no conjunto selecionado
    int cortemaxMelhores = 0, posCromossomoMelhoresCorteMax = -1;
    for(int i = 0; i < tamPop; i++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        printf("\nCalculo do corte maximo no cromossomo %d entre os cromossomos de populacao.",i);
        CorteMaximo(graf,populacao[i],&cortemaxMelhores,&posCromossomoMelhoresCorteMax,i);
    }
    printf("\n~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    printf("\nO Corte-Maximo foi obtido no cromossomo da posicao %d na populacao, e com valor de: %d\n", posCromossomoMelhoresCorteMax, cortemaxMelhores);
    imprime(grafreserva,populacao[posCromossomoMelhoresCorteMax],posCromossomoMelhoresCorteMax);

    fprintf(info,"\n~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    fprintf(info,"\nA diferença total do corte maximo encontrado na população inicial para o último corte maximo encontrado na geração %d é de: %d", geracaoDeAlteracaoDoMaior, diferencaTotal);

    fclose(arq);
    fclose(info);

    return 0;
}
