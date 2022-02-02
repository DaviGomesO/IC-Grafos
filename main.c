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
    struct adjacencia *prox; // O próximo elemento da lista de adjacencias
}ADJACENCIA;

typedef struct vertice {
    /* Dados armazenados vão aqui */
    ADJACENCIA *cab; //possui apenas a cabeça da lista de adjacencia
}VERTICE;

typedef struct grafo {
    int vertices; // numero de vertice total do grafo
    int arestas; // numero de arestas totais do grafo
    VERTICE *adj; // Arranjo de vertices da estrutura
}GRAFO;

/**função para criar um GRAFO**/
GRAFO *criaGrafo (int qtdvert) {
	int i;

	GRAFO *graph = (GRAFO *)malloc(sizeof(GRAFO)); //aloca espaço para estrtura grafo
	graph->vertices = qtdvert; //atualizo o numero de vertice
	graph->arestas = 0;  //atualizo o numero de vertice
	graph->adj = (VERTICE *)malloc(qtdvert*sizeof(VERTICE));
	//Dentro da estrturua tem só o arranjo para o ponteiro de vertice, não o arranjo em si
	// então aloco o arranjo com (v) o numero de vertice desejado
	for (i=0; i<qtdvert; i++){
		graph->adj[i].cab=NULL; //coloco NULL em todas arestas
	}
	return(graph);
}

/**função para adicionar arestas no GRAFO**/

ADJACENCIA *criaAdj(int v, int peso){
	ADJACENCIA *temp = (ADJACENCIA *) malloc (sizeof(ADJACENCIA)); //aloca espaço para um nó
	temp->vertice =v; //vertice alvo da adjacencia
	temp->peso = peso; //peso da aresta
	temp->prox = NULL;
	return(temp); //retorno endereço da adjacencia
}

bool criaAresta(GRAFO *graph, int vi, int vf, PESO p) { //vai de vi a vf
	if(!graph) return (false);  //validações se o grafo existe
	if((vf<0)||(vf >= graph->vertices))return(false); //validações se os valores não são neg
	if((vi<0)||(vf >= graph->vertices))return(false); //ou maiores que o numero de vértice do grafo

	ADJACENCIA *novo = criaAdj(vf,p); //crio adjacencia com o vértice final e o peso
	ADJACENCIA *volta = criaAdj(vi,p); // criei a volta pois trata-se de uma grafo não direcionado
	//coloco a adjacencia na lista do vértice inicial
	novo->prox = graph->adj[vi].cab;
	volta->prox = graph->adj[vf].cab; //o campo prox da adjacencia vai receber a cabeça da lista
	graph->adj[vi].cab=novo;
	graph->adj[vf].cab=volta; // e a cabeça da lista passa a ser o novo elemento
	graph->arestas++; // atualizo o numero de aresta no grafo
	return (true);
}

void imprime(GRAFO *gr){
	//validações se o grafo existe

	printf("Vertices: %d.\nArestas: %d. \n",gr->vertices,gr->arestas); //imprime numero de vértice e arestas
	int i;

	for(i=0; i<gr->vertices; i++){
		printf("v%d: ",i+1); //Imprimo em qual aresta estou
		ADJACENCIA *ad = gr->adj[i].cab; //chamo a cabeça da lista de adjacencia desta aresta
			while(ad){ //enquanto as adjacencias não forem nula
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
    GRAFO * graph;
    int orig, dest, peso;
    int qtdvert, qtdaresta;
    while (!feof(arq)){
        // Lê uma linha (inclusive com o '\n')
        // Se foi possível ler
        if(i == 1){
            result = fscanf(arq, "%d%d", &qtdvert, &qtdaresta);
            graph = criaGrafo(qtdvert);
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
