#ifndef OPERACOESAG_H_INCLUDED
#define OPERACOESAG_H_INCLUDED

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

#endif // OPERACOESAG_H_INCLUDED
