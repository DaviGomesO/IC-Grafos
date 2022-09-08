#ifndef OPERACOESAG_H_INCLUDED
#define OPERACOESAG_H_INCLUDED

int ** criaPopulacao(GRAFO *gr, int *totalPop, int *tamPopul, int tamCromossomo){
    int aleat;
    if(gr->vertices < 3){
        (*tamPopul) = (*totalPop);
    }else if(gr->vertices >= 3 && gr->vertices < 10){
        (*tamPopul) = ((*totalPop)/2);
    }else if(gr->vertices >= 10 && gr->vertices <= 14){
        (*tamPopul) = ((*totalPop)/4);
    }else if(gr->vertices > 14 && gr->vertices < 100){
        //limitar no máximo com 10000
        (*tamPopul) = 5000;
        if((*totalPop)/8 <= 10000){
            (*totalPop) = 5000;
        }else{
            (*totalPop) = 7000;
        }
    }else{
        (*tamPopul) = 7000;
        (*totalPop) = 10000;
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

/*float * criaVetorProbabilidade(GRAFO* grafo, int tamCromossomo){
    float * vetorProbabilidade = (int*)malloc(tamCromossomo*sizeof(int));
    for(int i = 0; i < grafo->vertices; i++){
        vetorProbabilidade[i] = 0;
    }

    float *C = (float*)malloc(grafo->vertices*sizeof(float));
    float *Cpop = (float*)malloc(grafo->vertices*sizeof(float));


    //calculo os valores de corte total para cada vértice
    for(int i = 0; i < grafo->vertices; i++){
        C[i] = 0;

        ADJACENCIA *aux = grafo->adj[i].cab;
        while(aux != NULL){
            C[i] += aux->peso;
            aux = aux->prox;
        }

    }

    //aqui dentro irei verificar cada cromossomo da população
    //dando enfase para o conjunto que ele pertence e aplicando a regra
    int cont = 0;
    for(int gene = 0; gene < tamCromossomo; gene++){
        //inicio zerando o valor dentro do vetor na posição correspondente ao gene/vértice
        Cpop[gene] = 0;
        //crio a variavel aux para pegar as arestas que se ligam com o gene/vértice
        ADJACENCIA *aux = grafo->adj[gene].cab;
        while(aux != NULL){
            if(aux->vertice < gene){
                Cpop[gene] += ((aux->peso)*1);
            }else{
                Cpop[gene] += ((aux->peso)*0);
            }
            aux = aux->prox;
        }

        if(Cpop[gene] >= C[gene]/2){
            vetorProbabilidade[gene] = 0.75;
        }else{
            vetorProbabilidade[gene] = 0.25;
        }

    }

    return vetorProbabilidade;
}

int ** criaPopulacaoGuloso(GRAFO *gr, int *totalPop, int *tamPopul, int tamCromossomo){
    int aleat;
    if(gr->vertices < 3){
        (*tamPopul) = (*totalPop);
    }else if(gr->vertices >= 3 && gr->vertices < 10){
        (*tamPopul) = ((*totalPop)/2);
    }else if(gr->vertices >= 10 && gr->vertices <= 14){
        (*tamPopul) = ((*totalPop)/4);
    }else if(gr->vertices > 14 && gr->vertices < 100){
        //limitar no máximo com 10000
        (*tamPopul) = 5000;
        if((*totalPop)/8 <= 10000){
            (*totalPop) = 5000;
        }else{
            (*totalPop) = 7000;
        }
    }else{
        (*tamPopul) = 7000;
        (*totalPop) = 10000;
    }
    int **populacao = (int**)malloc((*tamPopul) * sizeof(int *));
    for(int i = 0; i < (*tamPopul); i++){
        populacao[i] = (int*)malloc(tamCromossomo * sizeof(int));
    }

    float *vetorProbabilidadeSer1 = criaVetorProbabilidade(gr,tamCromossomo);

    printf("\nVetor Probabilidade de ser 1: ");
    for(int i = 0; i < tamCromossomo; i++){
        printf("%.2f ",vetorProbabilidadeSer1[i]);
    }
    printf("\n");

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
}*/

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

void mutacao(GRAFO *gr,int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao, int *valoresCortesNovaPopulacao){
    printf("\nEsta utilizando a mutacao.\n");
    for(int geraCromossomo = tamPop; geraCromossomo < (tamPop+(tamPop/2)); geraCromossomo++){
        int cromossomoaleat = tamPop+1, genealeat = tamCromossomo+1;
        int qtdGenes = tamCromossomo+1;
        //esse formato consegui deixar mais aleatorio sem apresentar sorteios viciados
        srand(rand());
        while(cromossomoaleat > tamPop){
            cromossomoaleat = rand()%tamPop;
        }

        //ajustar aqui para adicionar nas posições posteriores
        for(int i = tamCromossomo-1; i>=0; i--){
            novapopulacao[geraCromossomo][i] = populacao[cromossomoaleat][i];
        }

        while(qtdGenes > tamCromossomo || qtdGenes < 1){
            qtdGenes = rand()%tamCromossomo;
        }
        //printf("\nSera(ao) alterado(s) %d gene(s):",qtdGenes);
        for(int vezes = 1; vezes <= qtdGenes; vezes++){
            genealeat = tamCromossomo+1;
            while(genealeat > tamCromossomo){
                genealeat = rand()%tamCromossomo;
            }
            /*if(geracao == 1){
                printf("\nFazendo a alteracao %d: no gene %d do cromossomo %d da populacao inicial.\n",vezes, genealeat+1,cromossomoaleat);
            }else{
                printf("\nFazendo a alteracao %d: no gene %d do cromossomo %d da geracao anterior e adicionando no cromossomo %d.\n",vezes, genealeat+1,cromossomoaleat, geraCromossomo);
            }*/
            //tratar o caso de quando sorteia o gene mais de uma vez para ser alterado
            if(populacao[cromossomoaleat][genealeat] == azul){
                //printf("(azul -> vermelho)\n");
                novapopulacao[geraCromossomo][genealeat] = vermelho;
            }else if(populacao[cromossomoaleat][genealeat] == vermelho){
                //printf("(vermelho -> azul)\n");
                novapopulacao[geraCromossomo][genealeat] = azul;
            }
        }
        //printf("\n");
        valoresCortesNovaPopulacao[geraCromossomo] = corteDoCromossomo(gr,novapopulacao[geraCromossomo]);
    }

    (*probCruzamento) = (*probCruzamento)+((*probCruzamento)*0.05);
    (*probMutacao) = (*probMutacao)-((*probMutacao)*0.03);
}

void mutacao1gene(GRAFO *gr,int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao, int *valoresCortesNovaPopulacao){
    printf("\nEsta utilizando a mutacao.\n");
    for(int geraCromossomo = tamPop; geraCromossomo < (tamPop+(tamPop/2)); geraCromossomo++){
        int cromossomoaleat = tamPop+1, genealeat = tamCromossomo+1;
        //int qtdGenes = tamCromossomo+1;
        //esse formato consegui deixar mais aleatorio sem apresentar sorteios viciados
        srand(rand());
        while(cromossomoaleat > tamPop){
            cromossomoaleat = rand()%tamPop;
        }

        //ajustar aqui para adicionar nas posições posteriores
        for(int i = tamCromossomo-1; i>=0; i--){
            novapopulacao[geraCromossomo][i] = populacao[cromossomoaleat][i];
        }

        //printf("\nSera(ao) alterado(s) %d gene(s):",qtdGenes);

        genealeat = tamCromossomo+1;
        while(genealeat > tamCromossomo){
            genealeat = rand()%tamCromossomo;
        }

        //printf("Alterando o gene %d do cromossomo %d.\n", genealeat,cromossomoaleat);
        //tratar o caso de quando sorteia o gene mais de uma vez para ser alterado
        if(populacao[cromossomoaleat][genealeat] == azul){
            //printf("(azul -> vermelho)\n");
            novapopulacao[geraCromossomo][genealeat] = vermelho;
        }else if(populacao[cromossomoaleat][genealeat] == vermelho){
            //printf("(vermelho -> azul)\n");
            novapopulacao[geraCromossomo][genealeat] = azul;
        }
        //printf("\n");

        valoresCortesNovaPopulacao[geraCromossomo] = corteDoCromossomo(gr,novapopulacao[geraCromossomo]);
    }

    (*probCruzamento) = (*probCruzamento)+((*probCruzamento)*0.05);
    (*probMutacao) = (*probMutacao)-((*probMutacao)*0.03);
}

void mutacao2gene(GRAFO *gr,int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao, int *valoresCortesNovaPopulacao){
    printf("\nEsta utilizando a mutacao.\n");
    for(int geraCromossomo = tamPop; geraCromossomo < (tamPop+(tamPop/2)); geraCromossomo++){
        int cromossomoaleat = tamPop+1, genealeat1 = tamCromossomo+1, genealeat2 = tamCromossomo+1;
        //int qtdGenes = tamCromossomo+1;
        //esse formato consegui deixar mais aleatorio sem apresentar sorteios viciados
        srand(rand());
        while(cromossomoaleat > tamPop){
            cromossomoaleat = rand()%tamPop;
        }

        //ajustar aqui para adicionar nas posições posteriores
        for(int i = tamCromossomo-1; i>=0; i--){
            novapopulacao[geraCromossomo][i] = populacao[cromossomoaleat][i];
        }

        //printf("\nSera(ao) alterado(s) %d gene(s):",qtdGenes);

        genealeat1 = tamCromossomo+1;
        genealeat2 = tamCromossomo+1;
        while(genealeat1 > tamCromossomo && genealeat2 > tamCromossomo){
            genealeat1 = rand()%tamCromossomo;
            genealeat2 = rand()%tamCromossomo;

            if(genealeat1 == genealeat2){
                genealeat1 = tamCromossomo+1;
                genealeat2 = tamCromossomo+1;
            }
        }

        //printf("Alterando os gene %d e %d do cromossomo %d.\n", genealeat1, genealeat2,cromossomoaleat);
        //tratar o caso de quando sorteia o gene mais de uma vez para ser alterado
        if(populacao[cromossomoaleat][genealeat1] == azul){
            //printf("Gene 1 - (azul -> vermelho)\n");
            novapopulacao[geraCromossomo][genealeat1] = vermelho;
        }else if(populacao[cromossomoaleat][genealeat1] == vermelho){
            //printf("Gene 1 - (vermelho -> azul)\n");
            novapopulacao[geraCromossomo][genealeat1] = azul;
        }
        //printf("\n");
        if(populacao[cromossomoaleat][genealeat2] == azul){
            //printf("Gene 2 - (azul -> vermelho)\n");
            novapopulacao[geraCromossomo][genealeat2] = vermelho;
        }else if(populacao[cromossomoaleat][genealeat2] == vermelho){
            //printf("Gene 2 - (vermelho -> azul)\n");
            novapopulacao[geraCromossomo][genealeat2] = azul;
        }
        //printf("\n");

        valoresCortesNovaPopulacao[geraCromossomo] = corteDoCromossomo(gr,novapopulacao[geraCromossomo]);
    }

    (*probCruzamento) = (*probCruzamento)+((*probCruzamento)*0.05);
    (*probMutacao) = (*probMutacao)-((*probMutacao)*0.03);
}

void crossover(GRAFO *gr, int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao,  int *valoresCortesNovaPopulacao){
    printf("\nEsta utilizando o crossover.\n");
    for(int geraCromossomo = tamPop; geraCromossomo < (tamPop+(tamPop/2)); geraCromossomo = geraCromossomo + 2){
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
            if((cromossomoaleat1 == cromossomoaleat2))
                cromossomoaleat1 = cromossomoaleat2 = tamPop+1;             /*estou tirando essa condicional para permitir que o crossover ocorra com cromossomos iguais
                                                                            if((cromossomoaleat1 == cromossomoaleat2) || conferirCromossomo(populacao[cromossomoaleat1],populacao[cromossomoaleat2],tamCromossomo) == tamCromossomo)
                                                                            */
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

        //criei esses dois vetores para receber o cromossomo sorteado da população mais na frente
        int *novoCromossomo1, *novoCromossomo2;
        novoCromossomo1 = (int*)malloc(tamCromossomo*sizeof(int));
        novoCromossomo2 = (int*)malloc(tamCromossomo*sizeof(int));

        for(int i = 0; i < tamCromossomo; i++){
            novoCromossomo1[i] = novapopulacao[cromossomoaleat1][i];
            novoCromossomo2[i] = novapopulacao[cromossomoaleat2][i];
        }

        //vou adicionar ao final da população e depois ordenar para pegar os tamPop's primeiros

        //aqui faço as combinações, alterando apenas nos vetores criados dentro da função
        novoCromossomo2[genealeat1] = populacao[cromossomoaleat1][genealeat1];
        novoCromossomo2[genealeat2] = populacao[cromossomoaleat1][genealeat2];
        novoCromossomo1[genealeat1] = populacao[cromossomoaleat2][genealeat1];
        novoCromossomo1[genealeat2] = populacao[cromossomoaleat2][genealeat2];

        for(int i = 0; i<tamCromossomo; i++){
            novapopulacao[geraCromossomo][i] = novoCromossomo1[i];
            novapopulacao[geraCromossomo+1][i] = novoCromossomo2[i];
        }
        valoresCortesNovaPopulacao[geraCromossomo] = corteDoCromossomo(gr,novapopulacao[geraCromossomo]);
        valoresCortesNovaPopulacao[geraCromossomo+1] = corteDoCromossomo(gr,novapopulacao[geraCromossomo+1]);
    }

    (*probCruzamento) = (*probCruzamento)-((*probCruzamento)*0.03);
    (*probMutacao) = (*probMutacao)+((*probMutacao)*0.05);

    //mais na frente levará esse vetor novapopulação para conferir se esses cromossomos gerados estão ou não na população geral
}

void crossoverPercentual(GRAFO *gr, int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao,  int *valoresCortesNovaPopulacao){
    printf("\nEsta utilizando o crossover.\n");
    for(int geraCromossomo = tamPop; geraCromossomo < (tamPop+(tamPop/2)); geraCromossomo = geraCromossomo + 2){
        int cromossomoaleat1 = tamPop+1, cromossomoaleat2 = tamPop+1;
        int qtdgene = tamCromossomo+1;
        //esse formato consegui deixar mais aleatorio sem apresentar sorteios viciados
        srand(rand());
        while(cromossomoaleat1 > tamPop && cromossomoaleat2 > tamPop){
            cromossomoaleat1 = rand()%tamPop;
            cromossomoaleat2 = rand()%tamPop;
            //garantindo que os dois cromossomos sorteados não sejam o mesmo
            //o ou é para garantir que os dois cromossomos não sejam a mesma estrutura, ou seja, não tenham os mesmos genes
            //pois fazendo uma combinação entre os dois, irá gerar o mesmo cromossomo
            //portanto ajusta o valor das variaveis para continuar sorteando aleatoriamente

            //tirar essa parte depois do OU
            if((cromossomoaleat1 == cromossomoaleat2))
                cromossomoaleat1 = cromossomoaleat2 = tamPop+1;
        }
        //aqui só usei para ajustar em ordem crescente
        if(cromossomoaleat1 > cromossomoaleat2){
            int aux = cromossomoaleat1;
            cromossomoaleat1 = cromossomoaleat2;
            cromossomoaleat2 = aux;
        }
        //maior ou igual garante que pelo menos o último seja inalterado
        while(qtdgene>= tamCromossomo || qtdgene < 1){
            qtdgene = rand()%tamCromossomo;
        }

        /*if(geracao == 1){
            printf("\nCombinando os %d primeiros genes com os %d ultimos genes dos cromossomos %d e %d, da populacao inicial.\n",qtdgene,tamCromossomo-qtdgene,cromossomoaleat1,cromossomoaleat2);
        }else{
            printf("\nCombinando os %d primeiros genes com os %d ultimos genes dos cromossomos %d e %d, da geracao anterior.\n",qtdgene,tamCromossomo-qtdgene,cromossomoaleat1,cromossomoaleat2);
        }*/

        //criei esses dois vetores para receber o cromossomo sorteado da população mais na frente
        int *novoCromossomo1, *novoCromossomo2;
        novoCromossomo1 = (int*)malloc(tamCromossomo*sizeof(int));
        novoCromossomo2 = (int*)malloc(tamCromossomo*sizeof(int));

        for(int i = 0; i < tamCromossomo; i++){
            novoCromossomo1[i] = novapopulacao[cromossomoaleat1][i];
            novoCromossomo2[i] = novapopulacao[cromossomoaleat2][i];
        }

        /*
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
        */

        int cont;
        for(cont = 0; cont < qtdgene; cont++){
            novoCromossomo1[cont] = populacao[cromossomoaleat2][cont];
            novoCromossomo2[cont] = populacao[cromossomoaleat1][cont];
        }

        for(cont; cont<tamCromossomo; cont++){
            novoCromossomo1[cont] = populacao[cromossomoaleat1][cont];
            novoCromossomo2[cont] = populacao[cromossomoaleat2][cont];
        }

        /*
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
        */

        for(int i = 0; i<tamCromossomo; i++){
            novapopulacao[geraCromossomo][i] = novoCromossomo1[i];
            novapopulacao[geraCromossomo+1][i] = novoCromossomo2[i];
        }
        valoresCortesNovaPopulacao[geraCromossomo] = corteDoCromossomo(gr,novapopulacao[geraCromossomo]);
        valoresCortesNovaPopulacao[geraCromossomo+1] = corteDoCromossomo(gr,novapopulacao[geraCromossomo+1]);
    }

    (*probCruzamento) = (*probCruzamento)-((*probCruzamento)*0.03);
    (*probMutacao) = (*probMutacao)+((*probMutacao)*0.05);
    //printf("\n%0.2f x %0.2f\n",(*probCruzamento),(*probMutacao));
    //mais na frente levará esse vetor novapopulação para conferir se esses cromossomos gerados estão ou não na população geral
}

void crossoverPrevaleceIgualdade(GRAFO *gr, int **novapopulacao, int **populacao, int geracao, int tamPop, int tamCromossomo, float *probCruzamento, float *probMutacao,  int *valoresCortesNovaPopulacao){
    printf("\nEsta utilizando o crossover.\n");
    for(int geraCromossomo = tamPop; geraCromossomo < (tamPop+(tamPop/2)); geraCromossomo = geraCromossomo + 2){
        int cromossomoaleat1 = tamPop+1, cromossomoaleat2 = tamPop+1;
        int qtdgene = tamCromossomo+1;
        //esse formato consegui deixar mais aleatorio sem apresentar sorteios viciados
        srand(rand());
        while(cromossomoaleat1 > tamPop && cromossomoaleat2 > tamPop){
            cromossomoaleat1 = rand()%tamPop;
            cromossomoaleat2 = rand()%tamPop;
            //garantindo que os dois cromossomos sorteados não sejam o mesmo
            //o ou é para garantir que os dois cromossomos não sejam a mesma estrutura, ou seja, não tenham os mesmos genes
            //pois fazendo uma combinação entre os dois, irá gerar o mesmo cromossomo
            //portanto ajusta o valor das variaveis para continuar sorteando aleatoriamente

            //tirar essa parte depois do OU
            if((cromossomoaleat1 == cromossomoaleat2))
                cromossomoaleat1 = cromossomoaleat2 = tamPop+1;
        }
        //aqui só usei para ajustar em ordem crescente
        if(cromossomoaleat1 > cromossomoaleat2){
            int aux = cromossomoaleat1;
            cromossomoaleat1 = cromossomoaleat2;
            cromossomoaleat2 = aux;
        }
        //maior ou igual garante que pelo menos o último seja inalterado
        while(qtdgene>= tamCromossomo || qtdgene < 1){
            qtdgene = rand()%tamCromossomo;
        }

        /*
        if(geracao == 1){
            printf("\nCombinando os %d primeiros genes com os %d ultimos genes dos cromossomos %d e %d, da populacao inicial.\n",qtdgene,tamCromossomo-qtdgene,cromossomoaleat1,cromossomoaleat2);
        }else{
            printf("\nCombinando os %d primeiros genes com os %d ultimos genes dos cromossomos %d e %d, da geracao anterior.\n",qtdgene,tamCromossomo-qtdgene,cromossomoaleat1,cromossomoaleat2);
        }
        */
        //criei esses dois vetores para receber o cromossomo sorteado da população mais na frente
        int *novoCromossomo1, *novoCromossomo2;
        novoCromossomo1 = (int*)malloc(tamCromossomo*sizeof(int));
        novoCromossomo2 = (int*)malloc(tamCromossomo*sizeof(int));

        for(int i = 0; i < tamCromossomo; i++){
            novoCromossomo1[i] = novapopulacao[cromossomoaleat1][i];
            novoCromossomo2[i] = novapopulacao[cromossomoaleat2][i];
        }

        /*
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
        */

        for(int cont = 0; cont<tamCromossomo; cont++){
            if(populacao[cromossomoaleat1][cont] == populacao[cromossomoaleat2][cont]){
                //onde os genes são iguais, eles prevalecem
                //printf("\nAmbos os cromossomos sao iguais no gene %d.",cont);
                novoCromossomo1[cont] = populacao[cromossomoaleat1][cont];
                novoCromossomo2[cont] = populacao[cromossomoaleat2][cont];
            }else{
                //onde houver divergência entre os dois, sorteia um dos dois conjuntos para aquele gene divergente
                int aleat1, aleat2;
                aleat1 = (rand() % 10)%2;
                aleat2 = (rand() % 10)%2;
                if(aleat1 == 0){
                    novoCromossomo1[cont] = azul;
                }else if(aleat1 == 1){
                    novoCromossomo1[cont] = vermelho;
                }

                if(aleat2 == 0){
                    novoCromossomo2[cont] = azul;
                }else if(aleat1 == 1){
                    novoCromossomo2[cont] = vermelho;
                }
            }
        }

        /*
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
        */

        for(int i = 0; i<tamCromossomo; i++){
            novapopulacao[geraCromossomo][i] = novoCromossomo1[i];
            novapopulacao[geraCromossomo+1][i] = novoCromossomo2[i];
        }
        valoresCortesNovaPopulacao[geraCromossomo] = corteDoCromossomo(gr,novapopulacao[geraCromossomo]);
        valoresCortesNovaPopulacao[geraCromossomo+1] = corteDoCromossomo(gr,novapopulacao[geraCromossomo+1]);
    }

    (*probCruzamento) = (*probCruzamento)-((*probCruzamento)*0.03);
    (*probMutacao) = (*probMutacao)+((*probMutacao)*0.05);
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
    printf("\n\nMenor valor dentro dos melhores cromossomos: %d (esta no cromossomo %d)\n",vetorComCortedosMelhores[cromossomoDoMenor],cromossomoDoMenor);
    return cromossomoDoMenor;
}

int maiorCorteMelhores(int *vetorComCortedosMelhores, int tamPop){
    int maior = vetorComCortedosMelhores[0], cromossomoDoMaior = 0;
    for(int i = 1; i<tamPop; i++){
        if(maior < vetorComCortedosMelhores[i]){
            maior = vetorComCortedosMelhores[i];
            cromossomoDoMaior = i;
        }
    }
    printf("\nMaior valor dentro dos melhores cromossomos: %d (esta no cromossomo %d)\n",vetorComCortedosMelhores[cromossomoDoMaior],cromossomoDoMaior);
    return cromossomoDoMaior;
}


void alocarMelhoresSolucoes(GRAFO *gr, int *melhoresValoresCorte, int **melhoressolucoes, int tamPop, int **populacaogeral, int contaPos, int tamCromossomo){
    //aqui está alocando a população na matrix que contém as melhores
    if(contaPos <= tamPop+1){
        if(contaPos > tamPop){
            for(int i = 0; i < tamPop; i++){
                for(int j = tamCromossomo-1; j >= 0; j--){
                    melhoressolucoes[i][j] = populacaogeral[i][j];
                }
            melhoresValoresCorte[i] = corteDoCromossomo(gr,melhoressolucoes[i]);
            }
        }else{
        //enquanto todas as posições não forem alocadas do vetor com os melhores cromossomos, ele irá receber a população que existe
            for(int i = 0; i < contaPos; i++){
                for(int j = tamCromossomo-1; j >= 0; j--){
                    melhoressolucoes[i][j] = populacaogeral[i][j];
                }
                melhoresValoresCorte[i] = corteDoCromossomo(gr,melhoressolucoes[i]);
            }
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
                printf("e como o cromossomo %d da populacao geral tem valor de corte maior que o menor dentre os cromossomos com os maiores cortemax(%d > %d) faz-se a troca.\n",i,corteCromossomo,melhoresValoresCorte[posMenor]);
                for(int j = tamCromossomo-1; j>=0; j--){
                    melhoressolucoes[posMenor][j] = populacaogeral[i][j];
                    melhoresValoresCorte[posMenor] = corteCromossomo;
                    //aqui estou invertendo as posicoes, colocando os melhores no inicio da população geral
                    // e alocando aqueles que foram melhores antes no cromossomo de contagem que apareceu um maior
                    int aux = populacaogeral[posMenor][j];
                    populacaogeral[posMenor][j] = populacaogeral[i][j];
                    populacaogeral[i][j] = aux;
                }
                //atualiza onde esta o novo menor
                posMenor = menorcorteMelhores(melhoresValoresCorte, tamPop);
            }
        }
    }
}

void apresentarMelhoresCromossomos(GRAFO *graf, int *melhoresValoresCorte, int **melhoresSolucoes, int tamPop, int **populacaogeral, int contaPos, int tamCromossomo){
    printf("\nAnalise para achar se tem algum cromossomo na populacao geral com corte maior do que o menor corte de um cromossomo que esta entre as melhores solucoes:");
    alocarMelhoresSolucoes(graf,melhoresValoresCorte,melhoresSolucoes, tamPop, populacaogeral, contaPos, tamCromossomo);

    printf("\nOs Cromossomos com os melhores cortes sao:\n");
    imprimirMatriz(graf,melhoresSolucoes,tamPop, melhoresValoresCorte);
    for(int i = 0; i < tamPop; i++)
        printf("\nvalor de corte do cromossomo %d entre os melhores: %d",i,melhoresValoresCorte[i]);
}

void BuscaLocal(FILE *info, GRAFO *graf, GRAFO *grafreserva, int **populacao, int **novapopulacao, int *valoresCortesNovaPopulacao, int *valoresCortesPopulacao, int tamPop, int tamCromossomo, int pos){
    printf("\nBusca local no cromossomo %d:\n",pos+1);
    //aqui irei verificar a possibilidade para uma geração de algum cromossomo melhor que o atual melhor, tendo como base esse atual melhor.
    // alterando cada vértice para o subconjunto contrario ao atual
    int **matrizParaVerificarAlteracaoDeSubconjunto = (int**)malloc(graf->vertices*sizeof(int*));
    for(int i = 0; i < graf->vertices; i++){
        matrizParaVerificarAlteracaoDeSubconjunto[i] = (int*)malloc(graf->vertices*sizeof(int));
    }
    int contador = 0;
    int *valoresDosCromossomosQueHouveAlteracao = (int*)malloc(graf->vertices*sizeof(int));
    int *melhorCromossomoAtual = (int*)malloc(graf->vertices*sizeof(int));

    for(int i = 0; i<graf->vertices;i++){
        melhorCromossomoAtual[i] = populacao[pos][i];
    }

    if(pos == 0){
        printf("\nO valor de corte do melhor cromossomo atual eh: %d\n", corteDoCromossomo(graf,melhorCromossomoAtual));
    }else{
        printf("\nO valor de corte do cromossomo %d eh: %d\n", pos+1, corteDoCromossomo(graf,melhorCromossomoAtual));
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

#endif // OPERACOESAG_H_INCLUDED
