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
    int vertices; // numero de vertice total do grafo     /*Apesar de n�o ser nescessario guardar o numero de vertices  e arestas, o fazemos, pois caso quisermos*/
    int arestas; // numero de arestas totais do grafo  	/*saber desta informa��o, teriamos que percorrer toda a estrutura. Parece melhor perder 8bits de mem�ria salvando isto ;)*/
    VERTICE *adj; // Arranjo de vertices da estrutura (linha 22)
}GRAFO;

/**fun��o para criar um GRAFO**/
GRAFO *criaGrafo (int v) {
	int i;

	GRAFO *g = (GRAFO *)malloc(sizeof(GRAFO)); //aloca espa�o para estrtura grafo
	g->vertices = v; //atualizo o numero de vertice
	g->arestas = 0;  //atualizo o numero de vertice
	g->adj = (VERTICE *)malloc(v*sizeof(VERTICE)); //ler abaixo
	//Dentro da estrturua tem s� o arranjo para o ponteiro de vertice, n�o o arranjo em si
	// ent�o aloco o arranjo com (v) o numero de vertice desejado
	for (i=0; i<v; i++){
		g->adj[i].cab=NULL; //coloco NULL em todas arestas
	}
	return(g);
}

/**fun��o para adicionar arestas no GRAFO**/

ADJACENCIA *criaAdj(int v, int peso){
	ADJACENCIA *temp = (ADJACENCIA *) malloc (sizeof(ADJACENCIA)); //aloca espa�o para um n�
	temp->vertice =v; //vertice alvo da adjacencia
	temp->peso = peso; //peso da aresta
	temp->prox = NULL;
	return(temp); //retorno endere�o da adjacencia
}

bool criaAresta(GRAFO *gr, int vi, int vf, PESO p) { //vai de vi a vf
	if(!gr) return (false);  //valida��es se o grafo existe
	if((vf<0)||(vf >= gr->vertices))return(false); //valida��es se os valores n�o s�o neg
	if((vi<0)||(vf >= gr->vertices))return(false); //ou maiores que o numero de v�rtice do grafo

	ADJACENCIA *novo = criaAdj(vf,p); //crio adjacencia com o v�rtice final e o peso
	//coloco a adjacencia na lista do v�rtice inicial
	novo->prox = gr->adj[vi].cab; //o campo prox da adjacencia vai receber a cabe�a da lista
	gr->adj[vi].cab=novo; // e a cabe�a da lista passa a ser o novo elemento
	gr->arestas++; // atualizo o numero de aresta no grafo
	return (true);
}

void imprime(GRAFO *gr){
	//valida��es se o grafo existe

	printf("Vertices: %d. Arestas: %d. \n",gr->vertices,gr->arestas); //imprime numero de v�rtice e arestas
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
    char Linha[100];
    char *result;
    int i;
    arq = fopen("Exemplo.txt", "rt");

    // Se houver erro na abertura
    if (arq == NULL){
        printf("Problemas na abertura do arquivo\n");
        return;
    }
    i = 1;
    GRAFO * graph;
    int orig, dest, peso;
    int qtdvert, qtdaresta;
    while (!feof(arq)){
        // L� uma linha (inclusive com o '\n')
        // Se foi poss�vel ler
        if(i == 1){
            result = fscanf(arq, "%d%", &qtdvert);
            graph = criaGrafo(qtdvert);
        }else if(i == 2){
            result = fscanf(arq, "%d%", &qtdaresta);
        }else{
            result = fscanf(arq, "%d%d%d", &orig, &dest, &peso);
            criaAresta(graph,orig,dest,peso);
        }
        i++;
    }
    imprime(graph);
    fclose(arq);
    return 0;
}
