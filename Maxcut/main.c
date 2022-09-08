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
  printf("Vertices: %d.\nArestas: %d. \nLista de adjacencia do cromossomo %d\n", gr->vertices, gr->arestas,w); // imprime numero de v�rtice e arestas

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
      printf("- (N�o esta em nenhum subconjunto): ");
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

//tratar para cortes negativos
//ajustar para ordenar decrescente
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


int conferirIlha(int **ilha, int *cromossomo, int qtdDeCromossomosIlha, int tamCromossomo, int tamPop){
    if(qtdDeCromossomosIlha == 0){
        return 0;
    }else if(qtdDeCromossomosIlha > 0 && qtdDeCromossomosIlha < tamPop){
        for(int i = 0; i < qtdDeCromossomosIlha; i++){
            int contaIgualdade = 0;
            for(int j = 0; j < tamCromossomo; j++){
                if(ilha[i][j] == cromossomo[j]){
                    contaIgualdade++;
                }
            }
            if(contaIgualdade == tamCromossomo){
                return 1;
            }
        }
        return 0;
    }else{
        return 2;
    }
}

void conferirRestricao(GRAFO *grafo, int **populacao, int *valoresCortePopulacao, int tamPop, int tamCromossomo, int **ilha, int *valoresCorteIlha, int *qtdCromossomosAceitos){
    //crio um vetor para pegar o corte total
    float *C = (float*)malloc(grafo->vertices*sizeof(float));
    //esse outro vetor vai servir para pegar os calculos feitos em cima das arestas e de acordo com subconjunto que o v�rtice vai est�
    float *Cpop = (float*)malloc(grafo->vertices*sizeof(float));

    int pop = 0;

    //calculo os valores de corte total para cada v�rtice
    for(int i = 0; i < grafo->vertices; i++){
        C[i] = 0;
        //Cpop[i] = 0;
        ADJACENCIA *aux = grafo->adj[i].cab;
        if(aux != NULL){
            while(aux != NULL){
                C[i] += aux->peso;
                //printf("%d",aux->vertice);
                aux = aux->prox;
            }
        }
        //printf(" ");
    }

    //apresento esses valores de corte total para cada vertice
    for(int i = 0; i < grafo->vertices; i++){
        printf("\nvalor total do corte-maximo das arestas para o vertice %d: %.0f\n",i,C[i]);
    }


    //aqui dentro irei verificar cada cromossomo da popula��o
    //dando enfase para o conjunto que ele pertence e aplicando a regra
    while(pop < tamPop){
        printf("\nVerificando cromossomo %d:\n",pop);

        /*for(int j = 0; j<grafo->vertices;j++){
            printf("[");
            if(populacao[pop][j] == azul){
                printf("azul");
            }else if(populacao[pop][j] == vermelho){
                printf("vermelho");
            }
            printf("]");
        }
        printf("\n");*/

        int cont = 0;
        for(int gene = 0; gene < tamCromossomo; gene++){
            //inicio zerando o valor dentro do vetor na posi��o correspondente ao gene/v�rtice
            Cpop[gene] = 0;
            //crio a variavel aux para pegar as arestas que se ligam com o gene/v�rtice
            ADJACENCIA *aux = grafo->adj[gene].cab;
            if(aux != NULL){
                while(aux != NULL){
                    //printf("%d ",aux->vertice);
                    //aplico a regra para aquela posi��o do vetor que o gene da vez,
                    //recebendo o peso da aresta ligado a ele multiplicado por 1 caso esteja no azul, ou multiplicado por zero, caso esteja no zero
                    if(populacao[pop][aux->vertice] == azul){
                        Cpop[gene] += ((aux->peso)*1);
                    }else if(populacao[pop][aux->vertice] == vermelho){
                        Cpop[gene] += ((aux->peso)*0);
                    }
                    aux = aux->prox;
                }
            }
            //printf("\n");

            //se o gene da vez for azul ele faz um comparativo para o valor de corte do gene da vez calculado com a regra sendo menor ou igual
            //que a metade do valor de corte total para o gene/v�rtice em vez
            //o mesmo ocorre para o vermelho, s� que o sinal de compara��o inverte para maior ou igual
            //faz o comparativo e apenas imprime, por enquanto, e para tudo com o break e passa para o pr�ximo cromossomo da popula��o
            if(populacao[pop][gene] == azul){
                //printf("%.1f <= %.1f\n",Cpop[gene],C[gene]/2);
                if(Cpop[gene] <= C[gene]/2){
                    //printf("gene %d esta sendo aceito.\n",gene+1);
                    cont++;
                }else{
                    //printf("esse cromossomo nao eh aceito pois o gene %d do cromossomo %d nao atende as restricoes.\n",gene+1,pop);
                    break;
                }
            }else if(populacao[pop][gene] == vermelho){
                //printf("%.1f >= %.1f\n",Cpop[gene],C[gene]/2);
                if(Cpop[gene] >= C[gene]/2){
                    //printf("gene %d esta sendo aceito.\n",gene+1);
                    cont++;
                }else{
                    //printf("esse cromossomo nao eh aceito pois o gene %d do cromossomo %d nao atende as restricoes.\n",gene+1,pop);
                    break;
                }
            }
        }
        if(cont == tamCromossomo){
            if(conferirIlha(ilha,populacao[pop],(*qtdCromossomosAceitos),tamCromossomo,tamPop) == 0){
                printf("\nO cromossomo %d eh aceito pois todos os genes passam nas restricoes.\n\n",pop);
                for(int contaux = 0; contaux < tamCromossomo; contaux++){
                    ilha[(*qtdCromossomosAceitos)][contaux] = populacao[pop][contaux];
                }
                valoresCorteIlha[(*qtdCromossomosAceitos)] = valoresCortePopulacao[pop];

                (*qtdCromossomosAceitos)++;
            }else if(conferirIlha(ilha,populacao[pop],(*qtdCromossomosAceitos),tamCromossomo,tamPop) == 2){
                printf("\nA ilha esta cheia.\n");
            }else if(conferirIlha(ilha,populacao[pop],(*qtdCromossomosAceitos),tamCromossomo,tamPop) == 1){
                printf("\nO cromossomo %d ja esta na ilha.\n",pop);
            }
        }
        //incrementa para pegar o pr�ximo cromossomo da popula��o
        pop++;
    }
    //ainda tenho que passar os valores de corte dos cromossomos aceitos
}

void informacoesMelhorCromossomo(FILE *info, int geracao, int* valoresCortesPopulacao, int OperacaoMelhorCromossomoEncontrado, int* QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo){
    /*fprintf(info,"\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
    if(geracao>0) fprintf(info,"Na gera��o %d, o melhor cromossomo atual tem valor igual a: %d",geracao, valoresCortesPopulacao[0]);
    else fprintf(info,"Na gera��o inicial, o melhor cromossomo atual tem valor igual a: %d",valoresCortesPopulacao[0]);
    */
    if(OperacaoMelhorCromossomoEncontrado == 1){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado ainda na popula��o inicial.\n");
        QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[0]++;
    }else if(OperacaoMelhorCromossomoEncontrado == 2){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado na opera��o de muta��o.\n");
        QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[1]++;
    }else if(OperacaoMelhorCromossomoEncontrado == 3){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado na opera��o de crossover.\n");
        QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[2]++;
    }else if(OperacaoMelhorCromossomoEncontrado == 4){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado na opera��o de busca local.\n");
        QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[3]++;
    }
    //fprintf(info,"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
}

void main()
{
    clock_t tem = clock(); //armazena temp

    FILE *arq;
    FILE *info;
    FILE *resumo;
    arq = fopen("Testes/g05_80_0.txt", "rt");
    info = fopen("Informa��es/Testes-g05_80_0/Crossover percentual/(62) - Teste com cria��o de popula��o normal, muta��o em 2 genes, com busca a partir de 5 converg�ncias com ilha.txt", "wt");
    resumo = fopen("Informa��es/Testes-g05_80_0/Crossover percentual/resumo 62.txt", "wt");
    int periodoSemConvergencia = 5;
    GRAFO *graf, *grafreserva;
    char *result;
    int linha;
    int numgeracoes = 200;

    // Se houver erro na abertura
    if (arq == NULL)
    {
        printf("Problemas na abertura do arquivo\n");
        return;
    }
    if(info != NULL)
        printf("Arquivo de informacoes criado com sucesso!!\n");
    else{
        printf("Problemas na cria��o do arquivo\n");
        return 1;
    }

    if(resumo != NULL)
        printf("Arquivo de informacoes criado com sucesso!!\n");
    else{
        printf("Problemas na cria��o do arquivo\n");
        return 2;
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
          //criaAresta(graf, orig, dest, peso,0);
          //criaAresta(grafreserva, orig, dest, peso,1);
          criaAresta(graf, orig-1, dest-1, peso,0);
          criaAresta(grafreserva, orig-1, dest-1, peso,1);
        }
        linha++;
      }

    int cortemax = 0, subconjuntoCorteMax = -1;

    int totalPop = pow(2, graf->vertices);

    int tamPop = 0;
    int tamCromossomo = graf->vertices;
    float probCruzamento = 0.6, probMutacao = 1-probCruzamento;

    //iniciando a popula��o
    //geracao 0
    printf("\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\nGeracao inicial:\n");
    int **populacao = criaPopulacao(graf,&totalPop,&tamPop, tamCromossomo);
    int *valoresCortesPopulacao = (int*)malloc(tamPop*sizeof(int));
    //j� c�lculo os valores de corte dessa popula��o
    for(int cromossomo = 0; cromossomo < tamPop; cromossomo++){
        valoresCortesPopulacao[cromossomo] = corteDoCromossomo(graf, populacao[cromossomo]);
    }

    imprimirMatriz(graf,populacao,tamPop,valoresCortesPopulacao);
    //ordena essa popula��o em ordem decrescente
    insertion_sort(valoresCortesPopulacao, populacao, tamPop, tamCromossomo);
    printf("\nApos a ordenacao:\n");
    imprimirMatriz(graf, populacao, tamPop, valoresCortesPopulacao);

    //crio o vetor novapopula��p com o tamanho equivalente a quantidade do tamPop mais metade da popula��o, ou o mesmo que 1,5 * tamPop
    //para utilizar o mesmo mais na frente criando os cromossomos, ordenar e pegar os 'tamPop' melhores
    int **novapopulacao = (int**)malloc((tamPop + (tamPop/2)) * sizeof(int*));
    for(int i = 0; i<(tamPop + (tamPop/2)); i++){
        novapopulacao[i] = (int*)malloc(tamCromossomo*sizeof(int));
    }

    //int posMaior = maiorCorteMelhores(valoresCortesPopulacao, tamPop);
    int maiorCorte = valoresCortesPopulacao[0];
    fprintf(resumo,"Valor do corte m�ximo ao decorrer das gera��es...");
    fprintf(resumo,"\nGera��o : maior corte m�ximo");
    fprintf(resumo,"\n%d : %d",0, maiorCorte);
    fprintf(info, "O maior valor de corte da popula��o inicial � de: %d.\n",maiorCorte);
    int diferenca = 0, geracaoDeAlteracaoDoMaior = 0, diferencaTotal = 0;
    int semconvergencias = 0;

    //aqui estou passando as primeiras posi��es para o vetor que vai gerar novos cromossomos
    for(int i = 0; i < tamPop; i++){
        for(int j = tamCromossomo-1; j >= 0; j--){
            novapopulacao[i][j] = populacao[i][j];
        }
    }
    // e passando a copia do vetor com os valores de corte, j� ordenado
    int *valoresCortesNovaPopulacao = (int*)malloc((tamPop + (tamPop/2))*sizeof(int));
    for(int cromossomo = 0; cromossomo < tamPop; cromossomo++){
        valoresCortesNovaPopulacao[cromossomo] = valoresCortesPopulacao[cromossomo];
    }

    //busca local na popula��o inicial
    int maiorCorteAux = maiorCorte;
    //N�o vou conferir a busca local na popula��o inicial agora
    /*BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo,0); // passei para procurar na primeira posi��o
    maiorCorte = valoresCortesPopulacao[0];
    diferenca = maiorCorte - maiorCorteAux;
    diferencaTotal += diferenca;
    if(diferenca > 0){
        fprintf(info,"\nAinda na geracao inicial o maior corte maximo ser� de %d, encontrado na verifica��o de altera��o do subconjunto de cada gene, substituindo o antigo maior corte maximo, que tem valor de %d.\n", maiorCorte, maiorCorteAux);
        fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
        fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
    }*/

    ///*
    //ilha
    int **ilha = (int**)malloc(tamPop * sizeof(int*));
    for(int i = 0; i < tamPop; i++){
        ilha[i] = (int*)malloc(tamCromossomo*sizeof(int));
    }
    int *valoresCorteIlha = (int*)malloc(tamPop*sizeof(int));
    for(int i = 0; i < tamPop; i++){
        valoresCorteIlha[i] = 0;
    }

    int qtdCromossomosAceitos = 0;
    conferirRestricao(grafreserva,populacao, valoresCortesPopulacao, tamPop,tamCromossomo,ilha, valoresCorteIlha,&qtdCromossomosAceitos);

    if(qtdCromossomosAceitos > 0){
        printf("\nCromossomos aceitos:\n");
        for(int teste = 0; teste < qtdCromossomosAceitos; teste++){
            for(int gene = 0; gene<tamCromossomo; gene++){
                printf("[");
                if(ilha[teste][gene] == azul){
                    printf("azul");
                }else if(ilha[teste][gene] == vermelho){
                    printf("vermelho");
                }
                printf("]");
            }
            //printf("\n");
            printf(" - Valor do corte deste cromossomo: %d\n",valoresCorteIlha[teste]);
        }
    }else{
        printf("\nNenhum cromossomo foi aceito.\n");
    }
    //*/

    int OperacaoMelhorCromossomoEncontrado = 1;
    int operacaoAtual = 1;

    int *QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo = (int*)malloc(4*sizeof(int));

    QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[0] = QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[1] = QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[2] = QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[3] = 0;

    informacoesMelhorCromossomo(info, 0, valoresCortesPopulacao, OperacaoMelhorCromossomoEncontrado, QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo);

    for(int geracao = 1; geracao <= numgeracoes; geracao++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\nGeracao %d:",geracao);

        //quando estiver a um certo periodo de gera��es sem converg�ncias na popula��o, ir� recriar e se nessa nova popula��o conter algum cromossomo diferente da popula��o geral, ser� utilizada esta.
        //utilizo para fazer o c�digo parar e n�o gerar mais popula��es
        if(semconvergencias >= periodoSemConvergencia){
            maiorCorteAux = maiorCorte;
            BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, tamPop-1); //ultimo cromossomoif(maiorCorteAux < valoresCortesPopulacao[0]){
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                OperacaoMelhorCromossomoEncontrado = 4;
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nAp�s um peridodo de %d gera��es sem converg�ncia, na geracao %d o maior corte maximo sera %d, encontrado na altera��o do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da ultima posi��o da popula��o, que tem valor de %d, e foi encontrado na gera��o %d.\n", semconvergencias,geracao, maiorCorte, maiorCorteAux,geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(resumo,"\n%d : %d",geracao, maiorCorte);
                //geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }

            maiorCorteAux = maiorCorte;
            BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, (tamPop-1)/2); //posi��o intermediaria
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                OperacaoMelhorCromossomoEncontrado = 4;
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nAp�s um peridodo de %d gera��es sem converg�ncia, na geracao %d o maior corte maximo sera %d, encontrado na altera��o do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da posi��o %d da popula��o, que tem valor de %d, e foi encontrado na gera��o %d.\n", semconvergencias,geracao, maiorCorte, (tamPop-1)/2,maiorCorteAux, geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(resumo,"\n%d : %d",geracao, maiorCorte);
                //geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }

            maiorCorteAux = maiorCorte;
            BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 1); //segundaPosi��o
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                OperacaoMelhorCromossomoEncontrado = 4;
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nAp�s um peridodo de %d gera��es sem converg�ncia, na geracao %d o maior corte maximo sera %d, encontrado na altera��o do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da segunda posi��o da popula��o, que tem valor de %d, e foi encontrado na gera��o %d.\n", semconvergencias, geracao, maiorCorte, maiorCorteAux, geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(resumo,"\n%d : %d",geracao, maiorCorte);
                //geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }

            maiorCorteAux = maiorCorte;
            BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 0);
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                OperacaoMelhorCromossomoEncontrado = 4;
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nAp�s um peridodo de %d gera��es sem converg�ncia, na geracao %d o maior corte maximo sera %d, encontrado na altera��o do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da primeira posi��o da popula��o, que tem valor de %d, e foi encontrado na gera��o %d.\n", semconvergencias,geracao, maiorCorte,maiorCorteAux, geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(resumo,"\n%d : %d",geracao, maiorCorte);
                semconvergencias = 0;
            }

            //ideia: fazer uma busca local em um cromossomo aleatorio e diferente dos anteriores (tamPop/2, tamPop, 0, 1).

            if(semconvergencias == 0){
                geracaoDeAlteracaoDoMaior = geracao;
            }

            /*if(semconvergencias >= (numgeracoes/5)*2){
                fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
                fprintf(info,"Parou devido a um periodo de %d gera��es sem converg�ncias.",semconvergencias);
                break;
            }*/
        }

        printf("\nPopulacao que esta sendo trabalhada:\n");
        //imprimirMatriz(graf,populacao,tamPop,valoresCortesNovaPopulacao);

        if(probCruzamento <= probMutacao){
            operacaoAtual = 2;
            //mutacao(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao, valoresCortesNovaPopulacao);
            //mutacao1gene(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao, valoresCortesNovaPopulacao);
            mutacao2gene(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao, valoresCortesNovaPopulacao);
        }else{
            operacaoAtual = 3;
            //crossover(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao,valoresCortesNovaPopulacao);
            crossoverPercentual(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao,valoresCortesNovaPopulacao);
            //crossoverPrevaleceIgualdade(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao,valoresCortesNovaPopulacao);
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
        maiorCorteAux = maiorCorte;
        //verifico se o topo est� sendo alterado
        if(maiorCorteAux < valoresCortesPopulacao[0]){
            OperacaoMelhorCromossomoEncontrado = operacaoAtual;
            maiorCorte = valoresCortesPopulacao[0];
            diferenca = maiorCorte - maiorCorteAux;
            diferencaTotal += diferenca;
            fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
            fprintf(info,"\nNa geracao %d o maior corte maximo sera %d, substituindo o antigo maior corte maximo, que tem valor de %d, e foi encontrado na geracao %d.\n", geracao, maiorCorte, maiorCorteAux,geracaoDeAlteracaoDoMaior);
            fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
            fprintf(resumo,"\n%d : %d",geracao, maiorCorte);
            geracaoDeAlteracaoDoMaior = geracao;
            semconvergencias = 0;
        }else{
            semconvergencias++;
            //aqui eu estava fazendo uma busca local para o caso de n�o encontrar um novo melhor cromossomo com os crossovers e com a muta��o
            //agora estou fazendo busca local apenas quando o periodo de converg�ncia estoura por isso comentei essa parte toda
            /*BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 0);
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                //fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
                fprintf(info,"\nNa geracao %d o maior corte maximo sera %d, encontrado na altera��o do subconjunto de um gene, substituindo o antigo maior corte maximo, que tem valor de %d, e foi encontrado dentro dessa gera��o.\n", geracao, maiorCorte, maiorCorteAux);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
                geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }*/
        }


        ///*//confiro novamente se algum desses cromossomos pode ser um candidato a entrar na ilha
        conferirRestricao(grafreserva,populacao, valoresCortesPopulacao, tamPop,tamCromossomo,ilha, valoresCorteIlha,&qtdCromossomosAceitos);

        if(qtdCromossomosAceitos > 0){
            printf("\nCromossomos aceitos:\n");
            for(int teste = 0; teste < qtdCromossomosAceitos; teste++){
                for(int gene = 0; gene<tamCromossomo; gene++){
                    printf("[");
                    if(ilha[teste][gene] == azul){
                        printf("azul");
                    }else if(ilha[teste][gene] == vermelho){
                        printf("vermelho");
                    }
                    printf("]");
                }
                //printf("\n");
                printf("- Valor do corte deste cromossomo: %d\n",valoresCorteIlha[teste]);
            }
        }else{
            printf("\nNenhum cromossomo foi aceito.\n");
        }//*/
        informacoesMelhorCromossomo(info, geracao, valoresCortesPopulacao, OperacaoMelhorCromossomoEncontrado, QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo);
    }
    fprintf(resumo,"\n%d : %d",200, maiorCorte);

    //calcula entre os maiores cortes de cromossomos no conjunto selecionado
    int cortemaxMelhores = 0, posCromossomoMelhoresCorteMax = -1;
    for(int i = 0; i < tamPop; i++){
        //printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
        //printf("\nCalculo do corte maximo no cromossomo %d entre os cromossomos de populacao.",i);
        CorteMaximo(graf,populacao[i],&cortemaxMelhores,&posCromossomoMelhoresCorteMax,i);
    }
    printf("\n~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n");
    printf("\nO Corte-Maximo foi obtido no cromossomo da posicao %d na populacao, e com valor de: %d\n", posCromossomoMelhoresCorteMax, cortemaxMelhores);
    imprime(grafreserva,populacao[posCromossomoMelhoresCorteMax],posCromossomoMelhoresCorteMax);

    //fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
    fprintf(info,"\nA diferen�a total do corte maximo encontrado na popula��o inicial para o �ltimo corte maximo encontrado na gera��o %d � de: %d", geracaoDeAlteracaoDoMaior, diferencaTotal);

    fprintf(info,"\nCromossomo - ");
    for(int j = 0; j<graf->vertices;j++){
        fprintf(info,"[");
        if(populacao[0][j] == azul){
            fprintf(info,"azul");
        }else if(populacao[0][j] == vermelho){
            fprintf(info,"vermelho");
        }
        fprintf(info,"]");
    }
    fprintf(info," - Valor do corte Maximo: %d\n",valoresCortesPopulacao[0]);

    fprintf(info,"\nForam aceitos %d cromossomos na ilha.\n",qtdCromossomosAceitos);
    fprintf(resumo,"\nCromossomos aceitos na ilha: %d",qtdCromossomosAceitos);
    fprintf(resumo,"\nOpera��o: vezes encontradas");
    fprintf(info,"O melhor cromossomo foi encontrado %d vezes ainda na popula��o inicial.\nO melhor cromossomo foi encontrado %d vezes por meio da opera��o de muta��o.\nO melhor cromossomo foi encontrado %d vezes por meio da opera��o de crossover.\nO melhor cromossomo foi encontrado %d vezes por meio da opera��o de busca local.\n",QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[0],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[1],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[2],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[3]);
    fprintf(resumo,"\nPopula��o inicial: %d\nMuta��o: %d\nCrossover: %d\nBusca local: %d\n",QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[0],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[1],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[2],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[3]);


    tem = clock() - tem; //tempo final - tempo inicial
    //imprime o tempo na tela

    int minutos = (((double)tem)/(CLOCKS_PER_SEC/1000))/60000;
    int segundos = (((((double)tem)/(CLOCKS_PER_SEC/1000))/60000)-minutos)*60;

    printf("Tempo de execucao: ~ %d minutos e %d segundos", minutos, segundos);
    fprintf(info,"Tempo de execucao: ~ %d minutos e %d segundos", minutos, segundos);
    fprintf(resumo,"Tempo de execucao: ~ %d minutos e %d segundos", minutos, segundos);

    fclose(arq);
    fclose(info);
    fclose(resumo);

    return 0;
}

/*
OK - transformar a busca local em uma fun��o no arquivo de opera��es AG
OK - informar qual est� sendo o valor do melhor cromossomo em cada gera��o
OK - fazer a muta��o com 2 genes - Minha muta��o j� est� em um formato que ela pode alterar no m�nimo 1 gene ou todos, ou seja, uma quantidade aleatoria de genes
OK - identificar onde est� sendo encontrado o atual melhor cromossomo
OK - transformar as muta��es para 1 gene e para 2 genes.
fazer a nova cria��o de popula��o icremental(ideia: fazer o aux->prox e multiplicar todos que est�o abaixo do v�rtice da vez por 1, e os maiores por zero
ver qual vai ser a compara��o para atribuir 0.75 e 0.25 a cada um.*/

//GULOSO
//nova forma de iniciar a popula��o
//Calcular o C parcial icremental para o vetor de probabilidades
// criar o vetor de probabilidade entre 0 e 1, com diferen�as entre 0,25 e 0,75(no que viola menos)
// sorteador de probabilidade de cair em um conjunto

//fazer os testes tirando o c�digo de verifica��o da restri��o

//fazer os testes com cada combina��o solida, sem a mescla entre as opera��es

//itera��o de 200, com a converg�ncia de 5, 10 e 20 - ativar a busca local com essa quantidade sem converg�ncia(para tentar fazer a busca nos 4 pontos especificados)

//produzir um gr�fico com o periodo de itera��es pelo valor de corte m�ximo do grafo utilizado e ter informa��o do tempo

//produzir um gr�fico pizza tendo informa��o de quantas vezes foram encontrados os melhores solu��es e onde foram encontrados, se foi na muta��o, ou crossover, ou busca local.

/*
Problema analisado: ao ir se afunilando as gera��es e tendo uma mescla muito pequena de cromossomos, ou seja, tendo muitos cromossomos dificeis, o crossover vai chegar a um ponto que n�o vai funcionar pois estou ajustando ele
para n�o fazer a troca de genes entre cromossomos iguais
solu��o provis�ria: tirar a condi��o que evita fazer o crossover entre cromossomos iguais

Padr�o encontrado: chega um momento que a busca local nas mesmas posi��es acabam viciando e sendo apenas um extra para encontrar por acaso
*/
