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

void conferirRestricao(GRAFO *grafo, int **populacao, int tamPop, int tamCromossomo){
    float *C = (float*)malloc(grafo->vertices*sizeof(float));
    float *Cpop = (float*)malloc(grafo->vertices*sizeof(float));
    //int *Cazul = (int*)malloc(grafo->vertices*sizeof(int));
    //int *Cverm = (int*)malloc(grafo->vertices*sizeof(int));
    int pop = 0;
    for(int i = 0; i < grafo->vertices; i++){
        C[i] = 0;
        //Cazul[i] = 0;
        //Cverm[i] = 0;
        Cpop[i] = 0;
        ADJACENCIA *aux = grafo->adj[i].cab;
        if(aux != NULL){
            while(aux != NULL){
                C[i] += aux->peso;
                printf("%d",aux->vertice);
                aux = aux->prox;
            }
        }
        printf(" ");
    }

    for(int i = 0; i < grafo->vertices; i++){
        printf("\nvalor total do corte-maximo das arestas para o vertice %d: %.0f\n",i,C[i]);
    }

    while(pop < tamPop){
        printf("\nVerificando cromossomo %d:\n",pop);

        for(int j = 0; j<grafo->vertices;j++){
            printf("[");
            if(populacao[pop][j] == azul){
                printf("azul");
            }else if(populacao[pop][j] == vermelho){
                printf("vermelho");
            }
            printf("]");
        }
        printf("\n");
        for(int gene = 0; gene < tamCromossomo; gene++){
            Cpop[gene] = 0;
            ADJACENCIA *aux = grafo->adj[gene].cab;
            if(aux != NULL){
                while(aux != NULL){
                    printf("%d ",aux->vertice);
                    if(populacao[pop][aux->vertice] == azul){
                        Cpop[gene] += ((aux->peso)*1);
                    }else if(populacao[pop][aux->vertice] == vermelho){
                        Cpop[gene] += ((aux->peso)*0);
                    }
                    aux = aux->prox;
                }
            }
            printf("\n");
            if(populacao[pop][gene] == azul){
                printf("%.1f <= %.1f\n",Cpop[gene],C[gene]/2);
                if(Cpop[gene] <= C[gene]/2){
                    printf("gene %d esta sendo aceito.\n",gene+1);
                }else{
                    printf("esse cromossomo nao eh aceito pois o vertice %d da populacao %d nao atende as restricoes.\n",gene,pop);
                    break;
                }
            }else if(populacao[pop][gene] == vermelho){
                printf("%.1f >= %.1f\n",Cpop[gene],C[gene]/2);
                if(Cpop[gene] >= C[gene]/2){
                    printf("gene %d esta sendo aceito.\n",gene+1);
                }else{
                    printf("esse cromossomo nao eh aceito pois o vertice %d da populacao %d nao atende as restricoes.\n",gene,pop);
                    break;
                }
            }
        }
        pop++;
    }
}

void ProcuraMelhorSolucaoNoMelhorCromossomo(FILE *info, GRAFO *graf, GRAFO *grafreserva, int **populacao, int **novapopulacao, int *valoresCortesNovaPopulacao, int *valoresCortesPopulacao, int tamPop, int tamCromossomo, int pos){
    //aqui irei verificar a possibilidade para uma geração de algum cromossomo melhor que o atual melhor, tendo como base esse atual melhor.
    // alterando cada vértice para o subconjunto contrario ao atual
    int **matrizParaVerificarAlteracaoDeSubconjunto = (int**)malloc(graf->vertices*sizeof(int*));
    for(int i = 0; i < graf->vertices; i++){
        matrizParaVerificarAlteracaoDeSubconjunto[i] = (int*)malloc(graf->vertices*sizeof(int));
    }
    int contador = 0;
    int *valoresDosCromossomosQueHouveAlteracao = (int*)malloc(graf->vertices*sizeof(int));
    int *melhorCromossomoAtual = (int*)malloc(graf->vertices*sizeof(int));
    //printf("\nO melhor cromossomo atual eh:\n");
    for(int i = 0; i<graf->vertices;i++){
        melhorCromossomoAtual[i] = populacao[pos][i];
        //printf("[");
        if(melhorCromossomoAtual[i] == azul){
            //printf("azul");
        }else if(melhorCromossomoAtual[i] == vermelho){
            //printf("vermelho");
        }
        //printf("]");
    }

    if(pos == 1){
        printf("\nO valor de corte do melhor cromossomo atual eh: %d\n", corteDoCromossomo(graf,melhorCromossomoAtual));
    }else{
        printf("\nO valor de corte do cromossomo atual eh: %d\n", corteDoCromossomo(graf,melhorCromossomoAtual));
    }
    for(int i = 0; i < graf->vertices; i++){
        int Cint = 0, Cext = 0;
        ADJACENCIA *aux = grafreserva->adj[i].cab;
        if(melhorCromossomoAtual[i] == azul){
            if (aux != NULL){
                while(aux != NULL){
                    //printf("%d ",aux->vertice);
                    if (melhorCromossomoAtual[aux->vertice] == azul){
                        Cint += aux->peso;
                    }else if (melhorCromossomoAtual[aux->vertice] == vermelho){
                        Cext += aux->peso;
                    }
                    aux = aux->prox;
                }
            }
        }else if(melhorCromossomoAtual[i] == vermelho){
            if (aux != NULL){
                while (aux != NULL){
                    //printf("%d ",aux->vertice);
                    if (melhorCromossomoAtual[aux->vertice] == vermelho){
                        Cint += aux->peso;
                    }
                    else if (melhorCromossomoAtual[aux->vertice] == azul){
                        Cext += aux->peso;
                    }
                    aux = aux->prox;
                }
            }
        }
        //printf("\nValor do corte interno envolvendo o gene %d eh: %d. E o externo envolvendo o mesmo eh: %d\n",i,Cint,Cext);
        if(Cint > Cext){
            int *PossivelNovoCromossomoMelhor = (int*)malloc(graf->vertices*sizeof(int));
            if(melhorCromossomoAtual[i] == azul){
                PossivelNovoCromossomoMelhor[i] = vermelho;
            }else if(melhorCromossomoAtual[i] == vermelho){
                PossivelNovoCromossomoMelhor[i] = azul;
            }
            for(int j = 0; j <graf->vertices; j++){
                if( j != i ){
                    PossivelNovoCromossomoMelhor[j] = melhorCromossomoAtual[j];
                }
                matrizParaVerificarAlteracaoDeSubconjunto[contador][j] = PossivelNovoCromossomoMelhor[j];
            }
            valoresDosCromossomosQueHouveAlteracao[contador] = corteDoCromossomo(graf,matrizParaVerificarAlteracaoDeSubconjunto[contador]);
            /*for(int j = 0; j<graf->vertices;j++){
                printf("[");
                if(matrizParaVerificarAlteracaoDeSubconjunto[contador][j] == azul){
                    printf("azul");
                }else if(matrizParaVerificarAlteracaoDeSubconjunto[contador][j] == vermelho){
                    printf("vermelho");
                }
                printf("]");
            }
            printf(" - Valor do corte Maximo: %d\n",valoresDosCromossomosQueHouveAlteracao[contador]);*/

            for(int j = 0; j<graf->vertices;j++){
                novapopulacao[tamPop+contador][j] = matrizParaVerificarAlteracaoDeSubconjunto[contador][j];
                valoresCortesNovaPopulacao[tamPop+contador] = valoresDosCromossomosQueHouveAlteracao[contador];
            }
            fprintf(info,"\nAo substituir o gene %d de subconjunto, houve uma melhora no valor do corte máximo, passando de %d para %d.\n", i+1,corteDoCromossomo(graf,melhorCromossomoAtual), valoresCortesNovaPopulacao[tamPop+contador]);
            contador++;
        }
    }

    if(contador > 0){
        insertion_sort(valoresCortesNovaPopulacao, novapopulacao, tamPop+contador, tamCromossomo);

        for(int i = 0; i < tamPop; i++){
            for(int j = tamCromossomo-1; j >= 0; j--){
                populacao[i][j] = novapopulacao[i][j];
            }
            valoresCortesPopulacao[i] = valoresCortesNovaPopulacao[i];
        }
        printf("\nApos a verificacao de alteracao de subconjunto de um gene, foi encontrado um novo melhor corte, que eh: %d", valoresCortesPopulacao[0]);
        fprintf(info,"Portanto o novo melhor valor de corte máximo é: %d.\n",valoresCortesPopulacao[0]);
    }
}

void main()
{
    FILE *arq;
    FILE *info;
    arq = fopen("Exemplo.txt", "rt");
    info = fopen("Informações 5.txt", "wt");
    GRAFO *graf, *grafreserva;
    char *result;
    int linha;
    int numgeracoes = 100;

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
          criaAresta(graf, orig, dest, peso,0);
          criaAresta(grafreserva, orig, dest, peso,1);
          //criaAresta(graf, orig-1, dest-1, peso,0);
          //criaAresta(grafreserva, orig-1, dest-1, peso,1);
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

    //ilha
    int maiorCorteAux = maiorCorte;
    ProcuraMelhorSolucaoNoMelhorCromossomo(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo,0); // passei para procurar na primeira posição
    maiorCorte = valoresCortesPopulacao[0];
    diferenca = maiorCorte - maiorCorteAux;
    diferencaTotal += diferenca;
    if(diferenca > 0){
        fprintf(info,"\nAinda na geracao inicial o maior corte maximo será de %d, encontrado na verificação de alteração do subconjunto de cada gene, substituindo o antigo maior corte maximo, que tem valor de %d.\n", maiorCorte, maiorCorteAux);
        fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
        fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
    }

    conferirRestricao(grafreserva,populacao,tamPop,tamCromossomo);

    /*
    for(int geracao = 1; geracao <= numgeracoes; geracao++){
        printf("\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\nGeracao %d:",geracao);

        //quando estiver a um certo periodo de gerações sem convergências na população, irá recriar e se nessa nova população conter algum cromossomo diferente da população geral, será utilizada esta.
        //utilizo para fazer o código parar e não gerar mais populações
        if(semconvergencias >= numgeracoes/5){
            printf("\nBusca local no cromossomo %d:\n",tamPop);
            maiorCorteAux = maiorCorte;
            ProcuraMelhorSolucaoNoMelhorCromossomo(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, tamPop-1); //ultimo cromossomoif(maiorCorteAux < valoresCortesPopulacao[0]){
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                maiorCorteAux = maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nApós um peridodo de %d gerações sem convergência, na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da ultima posição da população, que tem valor de %d, e foi encontrado na geração %d.\n", numgeracoes/5,geracao, maiorCorte, maiorCorteAux,geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }


            printf("\nBusca local no cromossomo %d:\n",(tamPop-1)/2);
            ProcuraMelhorSolucaoNoMelhorCromossomo(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, (tamPop-1)/2); //posição intermediaria
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                maiorCorteAux = maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nApós um peridodo de %d gerações sem convergência, na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da posição %d da população, que tem valor de %d, e foi encontrado na geração %d.\n", numgeracoes/5,geracao, maiorCorte, (tamPop-1)/2,maiorCorteAux, geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }

            printf("\nBusca local no cromossomo %d:\n",2);
            ProcuraMelhorSolucaoNoMelhorCromossomo(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 2); //segundaPosição
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nApós um peridodo de %d gerações sem convergência, na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo do cromossomo da segunda posição da população, que tem valor de %d, e foi encontrado na geração %d.\n", numgeracoes/5, geracao, maiorCorte, maiorCorteAux, geracaoDeAlteracaoDoMaior);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                geracaoDeAlteracaoDoMaior = geracao;
                semconvergencias = 0;
            }

            if(semconvergencias >= (numgeracoes/5)*2){
                fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
                fprintf(info,"Parou devido a um periodo de %d gerações sem convergências.",semconvergencias);
                break;
            }

            //printf("Parou devido a falta de convergencia.");
            //fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
            //fprintf(info, "\nDevido um periodo de %d gerações sem convergência na população, o código parou na geração %d.", numgeracoes/5,geracao);
            //break;
        }

        printf("\nPopulacao que esta sendo trabalhada:\n");
        //imprimirMatriz(graf,populacao,tamPop,valoresCortesNovaPopulacao);

        if(probCruzamento <= probMutacao){
            mutacao(graf,novapopulacao,populacao,geracao,tamPop,tamCromossomo,&probCruzamento,&probMutacao, valoresCortesNovaPopulacao);
        }else{
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
            maiorCorte = valoresCortesPopulacao[0];
            diferenca = maiorCorte - maiorCorteAux;
            diferencaTotal += diferenca;
            fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
            fprintf(info,"\nNa geracao %d o maior corte maximo sera %d, substituindo o antigo maior corte maximo, que tem valor de %d, e foi encontrado na geracao %d.\n", geracao, maiorCorte, maiorCorteAux,geracaoDeAlteracaoDoMaior);
            fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
            geracaoDeAlteracaoDoMaior = geracao;

            maiorCorteAux = maiorCorte;
            ProcuraMelhorSolucaoNoMelhorCromossomo(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 0);
            if(maiorCorteAux < valoresCortesPopulacao[0]){
                maiorCorte = valoresCortesPopulacao[0];
                diferenca = maiorCorte - maiorCorteAux;
                diferencaTotal += diferenca;
                fprintf(info,"\nAinda na geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo, que tem valor de %d, e foi encontrado dentro dessa geração.\n", geracao, maiorCorte, maiorCorteAux);
                fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
                fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
            }

            semconvergencias = 0;
        }else{
            semconvergencias++;
            ProcuraMelhorSolucaoNoMelhorCromossomo(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo, 0);
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
            }
        }

        /*maiorCorteAux = maiorCorte;
        ProcuraMelhorSolucaoNoMelhorCromossomo(info, graf, grafreserva, populacao, novapopulacao, valoresCortesNovaPopulacao, valoresCortesPopulacao, tamPop, tamCromossomo);
        if(maiorCorteAux < valoresCortesPopulacao[0]){
            maiorCorte = valoresCortesPopulacao[0];
            diferenca = maiorCorte - maiorCorteAux;
            diferencaTotal += diferenca;
            fprintf(info,"\nNa geracao %d o maior corte maximo sera %d, encontrado na alteração do subconjunto de um gene, substituindo o antigo maior corte maximo, que tem valor de %d, e foi encontrado dentro dessa geração.\n", geracao, maiorCorte, maiorCorteAux);
            fprintf(info,"Portanto a diferenca do novo maior corte para o antigo maior corte eh de: %d\n",diferenca);
            fprintf(info,"\n=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~\n");
            geracaoDeAlteracaoDoMaior = geracao;
            semconvergencias = 0;
        }*/
        //verificar se houve convergência observando apenas o topo
    //}


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

    fclose(arq);
    fclose(info);

    return 0;
}

