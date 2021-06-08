/*
TRABALHO (VE 3)
Grupo: Alu XXXXXXX, Alu XXXXXXX, Alu XXXXXXX e Alu XXXXXXX
Sistema operacional: Windows
Programa em C que implementa um jogo de labirinto
*/

////////////////////////INCLUSAO DE BIBLIOTECAS////////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

////////////////////////DEFINICAO DE CONSTANTES////////////////////////
#define NOME_ARQUIVO_ENTRADA "entrada.txt" 
#define NOME_ARQUIVO_SAIDA "saida.txt" 
#define MAX_OPCOES 10

//////////////////////////DEFINICAO DE ENUMS//////////////////////////
typedef enum _tipo_no {
	raiz, nao_terminal, terminal
} tipo_no;

/////////////////////////DEFINICAO DE STRUCTS/////////////////////////
typedef struct _opcao {
	char opcao_selecionada;
	int indice_proximo_no;
} opcao;

typedef struct _no {
	int indice;
	char texto[501];
	tipo_no tipo;
	int n_opcoes;
	opcao opcoes[MAX_OPCOES];
	struct _no *ant;
	struct _no *prox;
} no;

/////////////////////////VARIAVEIS GLOBAIS//////////////////////////
//Referencia para o inicio e para a posicao atual na lista encadeada
no *ptr_inicio, *ptr_atual;
//Referencia para o arquivo de saida
FILE *arquivo_saida;
//Criterios globais
int chave = 0;

////////////////////////////PROTOTIPOS//////////////////////////////
char *ler_nome_jogador(char *nome_arquivo);
void cadastrar_no(int indice, char *texto, tipo_no tipo, int n_opcoes, opcao *opcoes);
void cadastrar_nos();
no *buscar_no(int indice);
int ler_indice_proximo_no(char opcao);
int verificar_criterio_acesso(int indice);
void atualizar_criterios_globais(int indice);
void apagar_lista();

///////////////////////////////MAIN///////////////////////////////
int main() { 
	
	//Abertura do arquivo de saida
	if (!(arquivo_saida = fopen(NOME_ARQUIVO_SAIDA, "w"))) {
		printf("ERRO AO ABRIR O ARQUIVO DE SAIDA!");
		exit(1); 
	}
	
	//Mensagem de boas-vindas
	char nome_jogador[100];
	strcpy(nome_jogador, ler_nome_jogador(NOME_ARQUIVO_ENTRADA));
	printf("Bem-vindo ao LABIRINTO de IC, %s!\n", nome_jogador);
	fprintf(arquivo_saida, "Bem-vindo ao LABIRINTO de IC, %s!\n", nome_jogador);
	
	//Montagem dos nos do jogo em uma lista encadeada
	cadastrar_nos();
	
	//Carregar no raiz
	ptr_atual = buscar_no(0);

	//laco de controle do jogo
	while(1) {
		//Se no nao eh terminal, apresentar texto e ler a opcao selecionada
		if(ptr_atual->tipo != terminal) {
			int indice_proximo_no = -1;
			while(indice_proximo_no == -1) {
				char opcao;
				printf("%s", ptr_atual->texto);
				fprintf(arquivo_saida, "%s", ptr_atual->texto);
				scanf(" %c", &opcao);
				fprintf(arquivo_saida, "%c\n", opcao);

				//Ler proximo no a partir da opcao
				indice_proximo_no = ler_indice_proximo_no(opcao);
				if(indice_proximo_no == -1) {
					printf("OPCAO INVALIDA!\n");
					fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
				}
			}
			//Se o acesso ao proximo no selecionado esta liberado, ler no, atualizar posicao atual e criterios globais
			if(verificar_criterio_acesso(indice_proximo_no)) {
				no *ptr_proximo_no = buscar_no(indice_proximo_no);
				ptr_atual = ptr_proximo_no;
				atualizar_criterios_globais(indice_proximo_no);
			}
		}
		//Se no eh terminal, apresentar texto e finalizar programa
		else {
			printf("%s", ptr_atual->texto);
			fprintf(arquivo_saida, "%s", ptr_atual->texto);
			break;
		}
	}
	
	//Apagar toda a lista e liberar espaco de memoria alocado
	apagar_lista();
	
	//Fechar o arquivo de saida
	fclose(arquivo_saida);
	
	return 0;
}

////////////////////////OUTRAS FUNCOES////////////////////////
//Funcao que le o nome do jogador a partir de um arquivo texto
char *ler_nome_jogador(char *nome_arquivo) {
	FILE *arquivo_entrada;
	char *nome = NULL, buffer[100];
	//Abrir o arquivo texto para leitura
	arquivo_entrada = fopen(nome_arquivo, "r");
	if (arquivo_entrada == NULL) {
		printf("\nERRO AO ABRIR O ARQUIVO DE ENTRADA!");
		fprintf(arquivo_saida, "\nERRO AO ABRIR O ARQUIVO DE ENTRADA!");
		exit(1); 
	}
	//Ler nome do jogador no arquivo
	if(!feof(arquivo_entrada)) {
		nome = fgets(buffer, 100, arquivo_entrada);
	}
	//Fechar o arquivo de entrada
	fclose(arquivo_entrada);
	//Retornar o nome lido
	return nome;
}

//Funcao que cadastra um novo no na lista encadeada
void cadastrar_no(int indice, char *texto, tipo_no tipo, int n_opcoes, opcao *opcoes) {
	//Alocar memoria para o novo no
	no *ptr = (no *) malloc(sizeof(no));
	if(ptr == NULL) {
		printf("\nERRO NA ALOCACAO DE MEMORIA!");
		fprintf(arquivo_saida, "\nERRO NA ALOCACAO DE MEMORIA!");
		return;
	}
	//Inicializar valores do novo no
	ptr->indice = indice;
	strcpy(ptr->texto, texto);
	ptr->tipo = tipo;
	ptr->n_opcoes = n_opcoes;
	for(int i = 0; i < n_opcoes; i++)
		ptr->opcoes[i] = opcoes[i];
	ptr->ant = NULL;
	ptr->prox = NULL;
	//Encadear novo no na lista
	no *ptr_aux = ptr_inicio;
	if(ptr_aux != NULL) { //Lista n�o-vazia
		while(ptr_aux->prox != NULL)
			ptr_aux = ptr_aux->prox;
		ptr_aux->prox = ptr;
		ptr->ant = ptr_aux;
	}
	else //Lista vazia
		ptr_inicio = ptr;
}

//Funcao que cadastra todos os nos da lista encadeada (carregamento da lista)
void cadastrar_nos() {
	opcao opcoes_0[2] = {{'D', 1}, {'E', 2}};
	cadastrar_no(0, 
		"\nENTRADA DO LABIRINTO\nVoce esta na entrada do labirinto e precisa decidir qual direcao seguir.\nD - Ir para a direita\nE - Ir para a esquerda\n\nOpcao escolhida: ", 
		raiz, 2, opcoes_0);
	opcao opcoes_1[1] = {{'*', 0}};	
	cadastrar_no(1, "\nCAMINHO SEM SAIDA\nVoce encontrou um caminho sem saida, porem ha uma chave caida no chao.\nDigite qualquer tecla + <ENTER> para pegar a chave e retornar a entrada do labirinto... ", 
		nao_terminal, 1, opcoes_1);
	opcao opcoes_2[2] = {{'A', 3}, {'B', 0}};	
	cadastrar_no(2, "\nPASSAGEM BLOQUEADA\nVoce encontrou uma passagem bloqueada com uma fechadura. O que deseja fazer?\nA - Tentar desbloquear a passagem\nB - Voltar a entrada do labirinto\n\nOpcao escolhida: ", 
		nao_terminal, 2, opcoes_2);
	cadastrar_no(3, "\nPARABENS! Voce conseguiu desbloquear a passagem com a chave e sair do labirinto!", terminal, 0, NULL);
}

//Funcao que busca um no a partir do seu indice
no *buscar_no(int indice) {
	no *ptr_aux = ptr_inicio;
	if(ptr_aux == NULL)
		return NULL;
	else {
		while(ptr_aux != NULL) {
			if(ptr_aux->indice == indice)
				return ptr_aux;
			ptr_aux = ptr_aux->prox;
		}
		return NULL;		
	}
}

//Funcao que le o indice do proximo no da lista a partir de uma opcao selecionada no no atual
int ler_indice_proximo_no(char opcao) {
	if(ptr_atual->opcoes[0].opcao_selecionada == '*') {
		return ptr_atual->opcoes[0].indice_proximo_no;
	}
	else {
		for(int i = 0; i < ptr_atual->n_opcoes; i++) {
			if(ptr_atual->opcoes[i].opcao_selecionada == opcao)
				return ptr_atual->opcoes[i].indice_proximo_no;
		}
		return -1;
	}
}

//Funcao que verifica se o acesso para um no de determinado indice esta liberado 
int verificar_criterio_acesso(int indice) {
	if(indice == 1 && chave == 1) {
		printf("Voce ja pegou a chave nesta parte do labirinto. Permanecendo na entrada...\n");
		fprintf(arquivo_saida, "Voce ja pegou a chave nesta parte do labirinto. Permanecendo na entrada...\n");
		return 0;
	}
	else if(indice == 3 && chave == 0) {
		printf("So eh possivel desbloquear a passagem com uma chave. Permanecendo na posicao...\n");
		fprintf(arquivo_saida, "So eh possivel desbloquear a passagem com uma chave. Permanecendo na posicao...\n");
		return 0;
	}
	return 1;;	
}

//Funcao que atualiza as variaveis globais relacionadas aos criterios de acesso aos nos, caso necessario
void atualizar_criterios_globais(int indice) {
	if(indice == 1)
		chave = 1;	
}

//Funcao que apaga toda a lista, liberando os espacos de memorias alocados
void apagar_lista() {
	no *ptr_aux = ptr_inicio;
	if(ptr_aux != NULL) {
		while(ptr_aux->prox != NULL) {
			if(ptr_aux->ant != NULL) {
				free(ptr_aux->ant);
			}
			ptr_aux = ptr_aux->prox;
		}
		if(ptr_aux->ant != NULL) {
			free(ptr_aux->ant);
		}
		free(ptr_aux);
	}
	ptr_inicio = NULL;
}
