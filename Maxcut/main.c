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
    //esse outro vetor vai servir para pegar os calculos feitos em cima das arestas e de acordo com subconjunto que o vértice vai está
    float *Cpop = (float*)malloc(grafo->vertices*sizeof(float));

    int pop = 0;

    //calculo os valores de corte total para cada vértice
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


    //aqui dentro irei verificar cada cromossomo da população
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
            //inicio zerando o valor dentro do vetor na posição correspondente ao gene/vértice
            Cpop[gene] = 0;
            //crio a variavel aux para pegar as arestas que se ligam com o gene/vértice
            ADJACENCIA *aux = grafo->adj[gene].cab;
            if(aux != NULL){
                while(aux != NULL){
                    //printf("%d ",aux->vertice);
                    //aplico a regra para aquela posição do vetor que o gene da vez,
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
            //que a metade do valor de corte total para o gene/vértice em vez
            //o mesmo ocorre para o vermelho, só que o sinal de comparação inverte para maior ou igual
            //faz o comparativo e apenas imprime, por enquanto, e para tudo com o break e passa para o próximo cromossomo da população
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
        //incrementa para pegar o próximo cromossomo da população
        pop++;
    }
    //ainda tenho que passar os valores de corte dos cromossomos aceitos
}

void informacoesMelhorCromossomo(FILE *info, int geracao, int* valoresCortesPopulacao, int OperacaoMelhorCromossomoEncontrado, int* QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo){
    /*fprintf(info,"\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
    if(geracao>0) fprintf(info,"Na geração %d, o melhor cromossomo atual tem valor igual a: %d",geracao, valoresCortesPopulacao[0]);
    else fprintf(info,"Na geração inicial, o melhor cromossomo atual tem valor igual a: %d",valoresCortesPopulacao[0]);
    */
    if(OperacaoMelhorCromossomoEncontrado == 1){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado ainda na população inicial.\n");
        QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[0]++;
    }else if(OperacaoMelhorCromossomoEncontrado == 2){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado na operação de mutação.\n");
        QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[1]++;
    }else if(OperacaoMelhorCromossomoEncontrado == 3){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado na operação de crossover.\n");
        QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[2]++;
    }else if(OperacaoMelhorCromossomoEncontrado == 4){
        //fprintf(info,"\nO melhor cromossomo atual foi encontrado na operação de busca local.\n");
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
    info = fopen("Informações/Testes-g05_80_0/Crossover percentual/(62) - Teste com criação de população normal, mutação em 2 genes, com busca a partir de 5 convergências com ilha.txt", "wt");
    resumo = fopen("Informações/Testes-g05_80_0/Crossover percentual/resumo 62.txt", "wt");
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
        printf("Problemas na criação do arquivo\n");
        return 1;
    }

    if(resumo != NULL)
        printf("Arquivo de informacoes criado com sucesso!!\n");
    else{
        printf("Problemas na criação do arquivo\n");
        return 2;
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

    int totalPop = pow(2, graf->vertices);

    int tamPop = 0;
    int tamCromossomo = graf->vertices;
    float probCruzamento = 0.6, probMutacao = 1-probCruzamento;

    //iniciando a população
    //geracao 0
    printf("\n<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\nGeracao inicial:\n");
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
    fprintf(resumo,"Valor do corte máximo ao decorrer das gerações...");
    fprintf(resumo,"\nGeração : maior corte máximo");
    fprintf(resumo,"\n%d : %d",0, maiorCorte);
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

    //busca local na população inicial
    int maiorCorteAux = maiorCorte;
    //Não vou conferir a busca local na população inicial agora
    /*BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo,0); // passei para procurar na primeira posição
    maiorCorte = valoresCortesPopulacao[0];
    diferenca = maiorCorte - maiorCorteAux;
    diferencaTotal += diferenca;
    if(diferenca > 0){
        fprintf(info,"\nAinda na geracao inicial o maior corte maximo será de %d, encontrado na verificação de alteração do subconjunto de cada gene, substituindo o antigo maior corte maximo, que tem valor de %d.\n", maiorCorte, maiorCorteAux);
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

        //quando estiver a um certo periodo de gerações sem convergências na população, irá recriar e se nessa nova população conter algum cromossomo diferente da população geral, será utilizada esta.
        //utilizo para fazer o código parar e não gerar mais populações
        if(semconvergencias >= periodoSemConvergencia){
            maiorCorteAux = maiorCorte;
            BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, tamPop-1); //ultimo cromossomoif(maiorCorteAux < valoresCortesPopulacao[0]){
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                OperacaoMelhorCromossomoEncontrado = 4;
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nApós um peridodo de %d gerações sem convergência, na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da ultima posição da população, que tem valor de %d, e foi encontrado na geração %d.\n", semconvergencias,geracao, maiorCorte, maiorCorteAux,geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(resumo,"\n%d : %d",geracao, maiorCorte);
                //geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }

            maiorCorteAux = maiorCorte;
            BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, (tamPop-1)/2); //posição intermediaria
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                OperacaoMelhorCromossomoEncontrado = 4;
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nApós um peridodo de %d gerações sem convergência, na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da posição %d da população, que tem valor de %d, e foi encontrado na geração %d.\n", semconvergencias,geracao, maiorCorte, (tamPop-1)/2,maiorCorteAux, geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(resumo,"\n%d : %d",geracao, maiorCorte);
                //geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }

            maiorCorteAux = maiorCorte;
            BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 1); //segundaPosição
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                OperacaoMelhorCromossomoEncontrado = 4;
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nApós um peridodo de %d gerações sem convergência, na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da segunda posição da população, que tem valor de %d, e foi encontrado na geração %d.\n", semconvergencias, geracao, maiorCorte, maiorCorteAux, geracaoDeAlteracaoDoMaior);
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
                fprintf(info,"\nApós um peridodo de %d gerações sem convergência, na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da primeira posição da população, que tem valor de %d, e foi encontrado na geração %d.\n", semconvergencias,geracao, maiorCorte,maiorCorteAux, geracaoDeAlteracaoDoMaior);
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
                fprintf(info,"Parou devido a um periodo de %d gerações sem convergências.",semconvergencias);
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
        //verifico se o topo está sendo alterado
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
            //aqui eu estava fazendo uma busca local para o caso de não encontrar um novo melhor cromossomo com os crossovers e com a mutação
            //agora estou fazendo busca local apenas quando o periodo de convergência estoura por isso comentei essa parte toda
            /*BuscaLocal(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 0);
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                //fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
                fprintf(info,"\nNa geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo, que tem valor de %d, e foi encontrado dentro dessa geração.\n", geracao, maiorCorte, maiorCorteAux);
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
    fprintf(info,"\nA diferença total do corte maximo encontrado na população inicial para o último corte maximo encontrado na geração %d é de: %d", geracaoDeAlteracaoDoMaior, diferencaTotal);

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
    fprintf(resumo,"\nOperação: vezes encontradas");
    fprintf(info,"O melhor cromossomo foi encontrado %d vezes ainda na população inicial.\nO melhor cromossomo foi encontrado %d vezes por meio da operação de mutação.\nO melhor cromossomo foi encontrado %d vezes por meio da operação de crossover.\nO melhor cromossomo foi encontrado %d vezes por meio da operação de busca local.\n",QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[0],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[1],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[2],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[3]);
    fprintf(resumo,"\nPopulação inicial: %d\nMutação: %d\nCrossover: %d\nBusca local: %d\n",QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[0],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[1],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[2],QtdGeracoesParaOperacoesQueEncontraramMelhorCromossomo[3]);


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
OK - transformar a busca local em uma função no arquivo de operações AG
OK - informar qual está sendo o valor do melhor cromossomo em cada geração
OK - fazer a mutação com 2 genes - Minha mutação já está em um formato que ela pode alterar no mínimo 1 gene ou todos, ou seja, uma quantidade aleatoria de genes
OK - identificar onde está sendo encontrado o atual melhor cromossomo
OK - transformar as mutações para 1 gene e para 2 genes.
fazer a nova criação de população icremental(ideia: fazer o aux->prox e multiplicar todos que estão abaixo do vértice da vez por 1, e os maiores por zero
ver qual vai ser a comparação para atribuir 0.75 e 0.25 a cada um.*/

//GULOSO
//nova forma de iniciar a população
//Calcular o C parcial icremental para o vetor de probabilidades
// criar o vetor de probabilidade entre 0 e 1, com diferenças entre 0,25 e 0,75(no que viola menos)
// sorteador de probabilidade de cair em um conjunto

//fazer os testes tirando o código de verificação da restrição

//fazer os testes com cada combinação solida, sem a mescla entre as operações

//iteração de 200, com a convergência de 5, 10 e 20 - ativar a busca local com essa quantidade sem convergência(para tentar fazer a busca nos 4 pontos especificados)

//produzir um gráfico com o periodo de iterações pelo valor de corte máximo do grafo utilizado e ter informação do tempo

//produzir um gráfico pizza tendo informação de quantas vezes foram encontrados os melhores soluções e onde foram encontrados, se foi na mutação, ou crossover, ou busca local.

/*
Problema analisado: ao ir se afunilando as gerações e tendo uma mescla muito pequena de cromossomos, ou seja, tendo muitos cromossomos dificeis, o crossover vai chegar a um ponto que não vai funcionar pois estou ajustando ele
para não fazer a troca de genes entre cromossomos iguais
solução provisória: tirar a condição que evita fazer o crossover entre cromossomos iguais

Padrão encontrado: chega um momento que a busca local nas mesmas posições acabam viciando e sendo apenas um extra para encontrar por acaso
*/
