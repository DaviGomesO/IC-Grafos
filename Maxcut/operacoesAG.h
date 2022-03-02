#ifndef OPERACOESAG_H_INCLUDED
#define OPERACOESAG_H_INCLUDED

int ** criaPopulacao(GRAFO *gr, int *totalPop, int *tamPopul, int tamCromossomo){
    int aleat;
    if(gr->vertices < 3){
        (*tamPopul) = (*totalPop)/2;
    }else if(gr->vertices >= 3 && gr->vertices < 10){
        (*tamPopul) = ((*totalPop)/4);
    }else if(gr->vertices >= 10 && gr->vertices < 17){
        (*tamPopul) = ((*totalPop)/8);
    }else{
        //limitar no máximo com 10000
        (*tamPopul) = 10000;
        (*totalPop) = (*totalPop)/8;

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

void mutacao(int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao){
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

    (*probCruzamento) = (*probCruzamento)+((*probCruzamento)*0.02);
    (*probMutacao) = (*probMutacao)-((*probMutacao)*0.05);
    //printf("\n%0.2f x %0.2f\n",(*probCruzamento),(*probMutacao));
}

void crossover(int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao){
    int cromossomoaleat1 = tamPop+1, cromossomoaleat2 = tamPop+1;
    int genealeat1 = tamCromossomo+1, genealeat2 = tamCromossomo+1;
    //esse formato consegui deixar mais aleatorio sem apresentar sorteios viciados
    srand(rand());
    while(cromossomoaleat1 > tamPop && cromossomoaleat2 > tamPop){
        cromossomoaleat1 = rand()%tamPop;
        cromossomoaleat2 = rand()%tamPop;
        //garantindo que os dois cromossomos sorteados não sejam o mesmo
        //o ou é para garantir que os dois cromossomos não sejam a mesma estrutura, ou seja, não tenham os mesmos genes
        //pois fazendo uma combinação entre os dois, irá gerar o mesmo cromossomo
        //portanto ajusta o valor das variaveis para continuar sorteando aleatoriamente
        if((cromossomoaleat1 == cromossomoaleat2) || conferirCromossomo(populacao[cromossomoaleat1],populacao[cromossomoaleat2],tamCromossomo) == tamCromossomo)
            cromossomoaleat1 = cromossomoaleat2 = tamPop+1;
    }
    //aqui só usei para ajustar em ordem crescente
    if(cromossomoaleat1 > cromossomoaleat2){
        int aux = cromossomoaleat1;
        cromossomoaleat1 = cromossomoaleat2;
        cromossomoaleat2 = aux;
    }
    while(genealeat1> tamCromossomo && genealeat2 > tamCromossomo){
        genealeat1 = rand()%tamCromossomo;
        genealeat2 = rand()%tamCromossomo;
        //se for sorteado o mesmo gene, ele faz o re-sorteio
        if(genealeat1 == genealeat2)
            genealeat1 = genealeat2 = tamCromossomo+1;
    }
    //aqui só usei para ajustar em ordem crescente
    if(genealeat1 > genealeat2){
        int aux = genealeat1;
        genealeat1 = genealeat2;
        genealeat2 = aux;
    }
    if(geracao == 0){
        printf("\nCombinando os genes %d e %d dos cromossomos %d e %d, da populacao inicial.\n",genealeat1+1,genealeat2+1,cromossomoaleat1,cromossomoaleat2);
    }else{
        printf("\nCombinando os genes %d e %d dos cromossomos %d e %d, da geracao anterior.\n",genealeat1+1,genealeat2+1,cromossomoaleat1,cromossomoaleat2);
    }

    //criei esses dois vetores para receber o cromossomo sorteado da população mais na frente
    int *novoCromossomo1, *novoCromossomo2;
    novoCromossomo1 = (int*)malloc(tamCromossomo*sizeof(int));
    novoCromossomo2 = (int*)malloc(tamCromossomo*sizeof(int));

    for(int i = 0; i < tamCromossomo; i++){
        novoCromossomo1[i] = novapopulacao[cromossomoaleat1][i];
        novoCromossomo2[i] = novapopulacao[cromossomoaleat2][i];
    }

    //estou imprimindo esses dois cromossomos para confirmar se pegou certo
    printf("\nCromossomo %d - ",cromossomoaleat1);
    for(int j = 0; j<tamCromossomo; j++){
        printf("[");
        if(novoCromossomo1[j] == azul)
            printf("azul");
        else if(novoCromossomo1[j] == vermelho)
            printf("vermelho");
        printf("]");
    }

    printf("\nCromossomo %d - ",cromossomoaleat2);
    for(int j = 0; j<tamCromossomo; j++){
        printf("[");
        if(novoCromossomo2[j] == azul)
            printf("azul");
        else if(novoCromossomo2[j] == vermelho)
            printf("vermelho");
        printf("]");
    }

    //depois ajustar para esses filhos gerados não tirem os pais da população, tentar adicionar a uma nova população
    // ou comparar para ficar com os melhores

    //aqui faço as combinações, alterando apenas nos vetores criados dentro da função
    novoCromossomo2[genealeat1] = populacao[cromossomoaleat1][genealeat1];
    novoCromossomo2[genealeat2] = populacao[cromossomoaleat1][genealeat2];
    novoCromossomo1[genealeat1] = populacao[cromossomoaleat2][genealeat1];
    novoCromossomo1[genealeat2] = populacao[cromossomoaleat2][genealeat2];


    //imprimo os vetores combinados
    printf("\nNovo cromossomo gerado - ");
    for(int j = 0; j<tamCromossomo; j++){
        printf("[");
        if(novoCromossomo1[j] == azul)
            printf("azul");
        else if(novoCromossomo1[j] == vermelho)
            printf("vermelho");
        printf("]");
    }

    printf("\nNovo cromossomo gerado - ");
    for(int j = 0; j<tamCromossomo; j++){
        printf("[");
        if(novoCromossomo2[j] == azul)
            printf("azul");
        else if(novoCromossomo2[j] == vermelho)
            printf("vermelho");
        printf("]");
    }
    printf("\n\n");

    //verifica se gerou cromossomos que ja estão na população
    //estou verificando se cada vetor desse ja não esta nessa população que estamos modificando
    int podeinserir = -1;
    for(int i = 0; i<tamPop;i++){
        if(conferirCromossomo(novapopulacao[i],novoCromossomo1,tamCromossomo)==tamCromossomo){
            podeinserir = 0;
            break;
        }else{
            podeinserir = 1;
        }
    }
    //se não tiver nesse vetor, vai inserir-lo no vetor de geração da população
    if(podeinserir == 1){
        for(int i = 0; i<tamCromossomo; i++){
            novapopulacao[cromossomoaleat1][i] = novoCromossomo1[i];
        }
    }

    //reinicio para conferir o segundo cromossomo gerado
    podeinserir = -1;
    //se ele ja não estiver no vetor atual de geração de população, irá entrar
    for(int i = 0; i<tamPop;i++){
        if(conferirCromossomo(novapopulacao[i],novoCromossomo2,tamCromossomo)==tamCromossomo){
            podeinserir = 0;
            break;
        }else{
            podeinserir = 1;
        }
    }
    if(podeinserir == 1){
        for(int i = 0; i<tamCromossomo; i++){
            novapopulacao[cromossomoaleat2][i] = novoCromossomo2[i];
        }
    }
    (*probCruzamento) = (*probCruzamento)-((*probCruzamento)*0.05);
    (*probMutacao) = (*probMutacao)+((*probMutacao)*0.02);
    //printf("\n%0.2f x %0.2f\n",(*probCruzamento),(*probMutacao));
    //mais na frente levará esse vetor novapopulação para conferir se esses cromossomos gerados estão ou não na população geral
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
    printf("\n-Menor valor dentro dos melhores cromossomos: %d (esta no cromossomo %d)\n",vetorComCortedosMelhores[cromossomoDoMenor],cromossomoDoMenor);
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
    printf("\nAnalise para achar se tem algum cromossomo na populacao geral com corte maior do que o menor corte de um cromossomo que esta entre as melhores solucoes:");
    alocarMelhoresSolucoes(graf,melhoresValoresCorte,melhoresSolucoes, tamPop, populacaogeral, contaPos, tamCromossomo);

    printf("\nOs Cromossomos com os melhores cortes sao:\n");
    imprimirMatriz(graf,melhoresSolucoes,tamPop);
    for(int i = 0; i < tamPop; i++)
        printf("\nvalor de corte do cromossomo %d entre os melhores: %d",i,melhoresValoresCorte[i]);
}

#endif // OPERACOESAG_H_INCLUDED
