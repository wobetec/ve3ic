/*
Olhar para refatorar:

*/

////////////////////////INCLUSAO DE BIBLIOTECAS///////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

////////////////////////DEFINICAO DE CONSTANTES///////////////////////
#define NOME_ARQUIVO_ENTRADA "entrada.txt"
#define NOME_ARQUIVO_SAIDA "saida.txt"
#define MAX_OPCOES 10

//define que pode ser alterado dependo da história (começa no zero)
#define N_NOS 3
#define MAX_TEXTO 5

//////////////////////////DEFINICAO DE ENUMS//////////////////////////
typedef enum _tipo_no {
	raiz, nao_terminal, terminal
} tipo_no;

typedef enum _no_complexo {
    passar, entrada, sala_item, pedra, saida, fechadura, enigma, luta, nao_complexo 
} no_complexo;

/////////////////////////DEFINICAO DE STRUCTS/////////////////////////
typedef struct _opcao{
    char opcao_selecionada;
    int indice_proximo_no;
} opcao;

typedef struct _no {
	int indice;
    int code; //7pppff (p=indice do pai, f="indice" do filho)
	char texto[501];
    no_complexo compl;
	tipo_no tipo;
	int n_opcoes;
	opcao opcoes[MAX_OPCOES];
	struct _no *ant;
	struct _no *prox;
} no;

////////////////DEFINICAO DE STRUCTS DOS NOS COMPLEXOS////////////////


//////////////////////////VARIAVEIS GLOBAIS///////////////////////////
//Referencia para o inicio e para a posicao atual na lista encadeada
no *ptr_inicio, *ptr_atual;
//Referencia para o arquivo de saida
FILE *arquivo_saida;

//Criterios globais


/////////////////////////////PROTOTIPOS///////////////////////////////
char *ler_nome_jogador(char *nome_arquivo);

//Gerenciamento da lista encadeada
void cadastrar_no(int indice, int code, char **texto, int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes);
no *buscar_no(int indice); //busa no na lista encadeada
void apagar_lista();//freela alista encadeada

//cadastro dos nós
void cadastrar_nos();//aplica a cadastrar_no varias vezes

//secundarias
int ler_indice_proximo_no(char opcao);
int verificar_criterio_acesso(int indice);
void atualizar_criterios_globais(int indice);

//mecanica do jogo

/////////////////////////////////MAIN/////////////////////////////////
int main(){

    //Inicialização do processo de log
    if(!(arquivo_saida = fopen(NOME_ARQUIVO_SAIDA, "w"))){
        printf("ERRO 01: ERRO AO ABRIR O ARQUIVO DE SAIDA.");
        exit(1);
    }
    
    //Página inicial com menuzinho
    




    return 0;
}

////////////////////////////OUTRAS FUNCOES////////////////////////////

char *ler_nome_jogador(char *nome_arquivo){
    
}

