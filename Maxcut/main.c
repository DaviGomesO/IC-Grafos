#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

typedef int bool;
typedef int PESO;

typedef struct adjacencia {
    int vertice; // vertice de destino
    PESO peso; // peso associado a aresta que leva ao vertice de destino
    struct adjacencia *prox; // O pr�ximo elemento da lista de adjacencias
}ADJACENCIA;

typedef struct vertice {
    /* Dados armazenados v�o aqui */
    ADJACENCIA *cab; //possui apenas a cabe�a da lista de adjacencia
}VERTICE;

typedef struct grafo {
    int vertices; // numero de vertice total do grafo
    int arestas; // numero de arestas totais do grafo
    VERTICE *adj; // Arranjo de vertices da estrutura
}GRAFO;

/**fun��o para criar um GRAFO**/
GRAFO *criaGrafo (int qtdvert) {
	int i;

	GRAFO *graf = (GRAFO *)malloc(sizeof(GRAFO)); //aloca espa�o para estrtura grafo
	graf->vertices = qtdvert; //atualizo o numero de vertice
	graf->arestas = 0;  //atualizo o numero de vertice
	graf->adj = (VERTICE *)malloc(qtdvert*sizeof(VERTICE));
	//Dentro da estrturua tem s� o arranjo para o ponteiro de vertice, n�o o arranjo em si
	// ent�o aloco o arranjo com (v) o numero de vertice desejado
	for (i=0; i<qtdvert; i++){
		graf->adj[i].cab=NULL; //coloco NULL em todas arestas
	}
	return(graf);
}

/**fun��o para adicionar arestas no GRAFO**/

ADJACENCIA *criaAdj(int v, int peso){
	ADJACENCIA *temp = (ADJACENCIA *) malloc (sizeof(ADJACENCIA)); //aloca espa�o para um n�
	temp->vertice =v; //vertice alvo da adjacencia
	temp->peso = peso; //peso da aresta
	temp->prox = NULL;
	return(temp); //retorno endere�o da adjacencia
}

bool criaAresta(GRAFO *graf, int vi, int vf, PESO p) { //vai de vi a vf
	if(!graf) return (false);  //valida��es se o grafo existe
	if((vf<0)||(vf >= graf->vertices))return(false); //valida��es se os valores n�o s�o neg
	if((vi<0)||(vf >= graf->vertices))return(false); //ou maiores que o numero de v�rtice do grafo

	ADJACENCIA *novo = criaAdj(vf,p); //crio adjacencia com o v�rtice final e o peso
	ADJACENCIA *volta = criaAdj(vi,p); // criei a volta pois trata-se de uma grafo n�o direcionado
	//coloco a adjacencia na lista do v�rtice inicial
	novo->prox = graf->adj[vi].cab;
	volta->prox = graf->adj[vf].cab; //o campo prox da adjacencia vai receber a cabe�a da lista
	graf->adj[vi].cab=novo;
	graf->adj[vf].cab=volta; // e a cabe�a da lista passa a ser o novo elemento
	graf->arestas++; // atualizo o numero de aresta no grafo
	return (true);
}

void imprime(GRAFO *gr){
	//valida��es se o grafo existe

	printf("Vertices: %d.\nArestas: %d. \n",gr->vertices,gr->arestas); //imprime numero de v�rtice e arestas
	int i;

	for(i=0; i<gr->vertices; i++){
		printf("v%d: ",i+1); //Imprimo em qual aresta estou
		ADJACENCIA *ad = gr->adj[i].cab; //chamo a cabe�a da lista de adjacencia desta aresta
			while(ad){ //enquanto as adjacencias n�o forem nula
				printf("v%d(%d) ",ad->vertice+1,ad->peso); //imprimo a adjacencia e seu peso
				ad=ad->prox; //passo para proxima adjacencia
			}
		printf("\n");
	}
}

void main()
{
    FILE *arq;
    char *result;
    int i;
    arq = fopen("Exemplo.txt", "rt");

    // Se houver erro na abertura
    if (arq == NULL){
        printf("Problemas na abertura do arquivo\n");
        return;
    }
    i = 1;
    GRAFO * graf;
    int orig, dest, peso;
    int qtdvert, qtdaresta;
    while (!feof(arq)){
        // L� uma linha (inclusive com o '\n')
        // Se foi poss�vel ler
        if(i == 1){
            result = fscanf(arq, "%d%d", &qtdvert, &qtdaresta);
            graf = criaGrafo(qtdvert);
        }else{
            result = fscanf(arq, "%d%d%d", &orig, &dest, &peso);
            criaAresta(graf,orig,dest,peso);
        }
        i++;
    }
    imprime(graf);
    fclose(arq);
    return 0;
}
