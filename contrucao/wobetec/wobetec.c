/*
Olhar para refatorar:
    Implementar o Enigma no básico
    Criar as funcinalidades de jogo
    Colocar opcao no comeco do jogo
UU
    Implementar o no nao complexo
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
    int code; //7pppttff (p=indice do pai, t=tipo, f="indice" do filho)
	char texto[501];
	tipo_no tipo;
	int n_opcoes;
	opcao opcoes[MAX_OPCOES];
    char *adcional_c;
	struct _no *ant;
	struct _no *prox;
} no;

////////////////DEFINICAO DE STRUCTS DOS NOS COMPLEXOS////////////////


//////////////////////////VARIAVEIS GLOBAIS///////////////////////////
//Referencia para o inicio e para a posicao atual na lista encadeada
no *ptr_inicio, *ptr_atual;
//Referencia para o arquivo de saida
FILE *arquivo_saida;
//referencia para a criação dos nós
int contador_de_no = -1;

//Criterios globais
int pedras = 0;

/////////////////////////////PROTOTIPOS///////////////////////////////
char *ler_nome_jogador(char *nome_arquivo);

//Gerenciamento da lista encadeada
void cadastrar_no(int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char *adcional_c);
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
    char nome_jogador[100];
    strcpy(nome_jogador, ler_nome_jogador(NOME_ARQUIVO_ENTRADA));
    printf("Bem-vindo ao LABIRINTO de IC, %s!\n", nome_jogador);
	fprintf(arquivo_saida, "Bem-vindo ao LABIRINTO de IC, %s!\n", nome_jogador);

    //Montagem da lista encadeada
    cadastrar_nos();

    //carregar o no zero
    ptr_atual = buscar_no(0);

    //Laco principal
    while(1) {
		//Se no nao eh terminal, apresentar texto e ler a opcao selecionada
		if(ptr_atual->tipo != terminal){
			int indice_proximo_no = -1;
			while(indice_proximo_no == -1){
				char opcao;
				printf("%s", ptr_atual->texto);
				fprintf(arquivo_saida, "%s", ptr_atual->texto);
				scanf(" %c", &opcao);
				fprintf(arquivo_saida, "%c\n", opcao);
				//Ler proximo no a partir da opcao
				indice_proximo_no = ler_indice_proximo_no(opcao);
				if(indice_proximo_no == -1){
					printf("OPCAO INVALIDA!\n");
					fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
				}
			}
		}
		else{//Se no eh terminal, apresentar texto e finalizar programa
			printf("%s", ptr_atual->texto);
			fprintf(arquivo_saida, "%s", ptr_atual->texto);
			break;
		}
	}

    //Libera o espaço utlizado pela lista encadeada
    apagar_lista();

    //fecha o arquivo de log
    fclose(arquivo_saida);

    return 0;
}

////////////////////////////OUTRAS FUNCOES////////////////////////////

//faz a leitura do nome do jogador dentro do arquivo de entrada
char *ler_nome_jogador(char *nome_arquivo){
    //variaveis
    FILE *arquivo_entrada;
    char *nome = NULL;
    char buffer[100];

    //abertura do arquivo
    arquivo_entrada = fopen(nome_arquivo, "r");
    if(arquivo_entrada == NULL){
        printf("\nERRO 02: ERRO AO ABRIR O ARQUIVO DE ENTRADA.");
        fprintf(arquivo_saida, "\nERRO 02: ERRO AO ABRIR O ARQUIVO DE ENTRADA.");
        exit(1);
    }

    //copiando o arquivo para a variável nome
    if(!feof(arquivo_entrada)){
        nome = fgets(buffer, 100, arquivo_entrada);
    }

    //fechando o arquivo
    fclose(arquivo_entrada);

    //retorna a str nome
    return nome;
}


//##########CRIACAO DE NOS E MANIPULACAO DA LISTA ENCADEADA##########//

//criacao de nos e manipulacao da lista encadeada
void cadastrar_no(int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char *adcional_c){

    //alocação dinâmica da memória
    no *ptr = (no *) malloc(sizeof(no));
    if(ptr == NULL){
        printf("\nERRO 03: ERRO NA ALOCACAO DE MEMORIA.");
        fprintf(arquivo_saida, "\nERRO 03: ERRO NA ALOCACAO DE MEMORIA.");
        exit(1);
    }

    //Variáveis utilizadas em varios switchs e que deram erro
    no *ptr_aux = ptr_inicio;
    int codigo = compl*100;
    int make = 70000000;// 7 000 00 00

    //swirch que define como serão os nós especiais criados
    switch(compl){
        
        case passar:
            //define o indice de forma automática
            contador_de_no++;
            ptr->indice = contador_de_no;
            
            //gerando o código daquele nó
            if(code == -1){
                make += (contador_de_no*10000);
            }else{
                make += ((contador_de_no-(code%100))*10000 + code);
            }

            ptr->code = make;
            strcpy(ptr->texto, texto[n_textos]);
            ptr->tipo = tipo;
            ptr->n_opcoes = n_opcoes;

            for(int i=0; i<n_opcoes; i++){
                ptr->opcoes[i] = opcoes[i];
            }

            ptr->ant = NULL;
            ptr->prox = NULL;

            ptr->adcional_c = adcional_c;

            if(ptr_aux != NULL){
                while(ptr_aux->prox != NULL){
                    ptr_aux = ptr_aux->prox;
                }
                ptr_aux->prox = ptr;
                ptr->ant =ptr_aux;
            }else{
                ptr_inicio = ptr;
            }

            break;

        case entrada:
            cadastrar_no(-1, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            break;

        case sala_item:
            cadastrar_no(codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            cadastrar_no(codigo+1, texto, 1, tipo, passar, n_opcoes, opcoes, "");
            cadastrar_no(codigo+2, texto, 2, tipo, passar, n_opcoes, opcoes, "");
            break;

        case pedra:
            cadastrar_no(codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            break;

        case saida:
            cadastrar_no(codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            break;

        case fechadura:
            cadastrar_no(codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "CA2");
            break;

        case enigma:
    

            break;

        case luta:
            cadastrar_no(codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            cadastrar_no(codigo+1, texto, 1, tipo, passar, n_opcoes, opcoes, "");
            cadastrar_no(codigo+2, texto, 2, tipo, passar, n_opcoes, opcoes, "");
            cadastrar_no(codigo+3, texto, 3, tipo, passar, n_opcoes, opcoes, "");
            break;

        case nao_complexo:
            //define o indice de forma automática
            contador_de_no++;
            ptr->indice = contador_de_no;
            
            //gerando o código daquele nó
            if(code == -1){
                make += (contador_de_no*10000);
            }else{
                make += ((contador_de_no-(codigo%100))*10000 + codigo);
            }
            ptr->code = make;

            strcpy(ptr->texto, texto[n_textos]);
            ptr->tipo = tipo;
            ptr->n_opcoes = n_opcoes;

            for(int i=0; i<n_opcoes; i++){
                ptr->opcoes[i] = opcoes[i];
            }

            ptr->ant = NULL;
            ptr->prox = NULL;

            ptr->adcional_c = adcional_c;


            if(ptr_aux != NULL){
                while(ptr_aux->prox != NULL){
                    ptr_aux = ptr_aux->prox;
                }
                ptr_aux->prox = ptr;
                ptr->ant =ptr_aux;
            }else{
                ptr_inicio = ptr;
            }
            break;

        default:
            printf("\nERRO 04: TIPO DE NO INVALIDO.");
            fprintf(arquivo_saida, "\nERRO 04: TIPO DE NO INVALIDO.");
            exit(1);
       }




}


void log(no atual){

}


//cadastras todos os nos na lista encadeada
void cadastrar_nos(){
    /* MODELO DE CADASTRO
        cadastrar_no(
            code, 
            texto[][501],
            n_textos,
            tipo,
            compl,
            n_opcoes,
            opcoes,
            adcional_c)
    */

    opcao opcoes_0[2] = {{'D', 1}, {'E', 2}};
    char texto_0[1][501] = {"\nENTRADA DO LABIRINTO\nVoce esta na entrada do labirinto e precisa decidir qual direcao seguir.\nD - Ir para a direita\nE - Ir para a esquerda\n\nOpcao escolhida: "};
	cadastrar_no(
        -1, 
        texto_0,
        0,
		raiz,
        nao_complexo,
        2,
        opcoes_0,
        "");
    
    opcao opcoes_1[1] = {{'*', 0}};
    char texto_1[1][501] = {"\nCAMINHO SEM SAIDA\nVoce encontrou um caminho sem saida, porem ha uma chave caida no chao.\nDigite qualquer tecla + <ENTER> para pegar a chave e retornar a entrada do labirinto... "};
	cadastrar_no(
        -1, 
        texto_1,
        0,
		nao_terminal,
        nao_complexo,
        2,
        opcoes_1,
        "");

    opcao opcoes_2[2] = {{'A', 3}, {'B', 0}};
    char texto_2[1][501] = {"\nPASSAGEM BLOQUEADA\nVoce encontrou uma passagem bloqueada com uma fechadura. O que deseja fazer?\nA - Tentar desbloquear a passagem\nB - Voltar a entrada do labirinto\n\nOpcao escolhida: "};
	cadastrar_no(
        -1, 
        texto_2,
        0,
		terminal,
        nao_complexo,
        2,
        opcoes_2,
        "");
    
    char texto_3[1][501] = {"\nPARABENS! Voce conseguiu desbloquear a passagem com a chave e sair do labirinto!"};
	cadastrar_no(
        -1, 
        texto_3,
        0,
		terminal,
        nao_complexo,
        0,
        NULL,
        "");
}

//funcao para buscar no a partir do indice
no *buscar_no(int indice){
    no *ptr_aux = ptr_inicio;
    if(ptr_aux == NULL){
        return NULL;
    }else{
        while(ptr_aux != NULL){
            if(ptr_aux->indice == indice){
                return ptr_aux;
            }
            ptr_aux = ptr_aux->prox;
            }
        
        printf("\nERRO 05: NO NAO ENCONTRADO NA LISTA.");
        fprintf(arquivo_saida, "\nERRO 05: NO NAO ENCONTRADO NA LISTA.");
        exit(1);
        }
}

//funcao para liberar a memoria alocada
void apagar_lista(){
    no *ptr_aux = ptr_inicio;
    if(ptr_aux != NULL){
        while(ptr_aux->prox != NULL){
            if(ptr_aux->ant != NULL){
                free(ptr_aux->ant);
            }
            ptr_aux = ptr_aux->prox;
        }
        if(ptr_aux->ant != NULL){
            free(ptr_aux->ant);
        }
        free(ptr_aux);
    }
    ptr_inicio = NULL;
}


//#########################MECANICA DE JOGO#########################//

int ler_indice_proximo_no(char opcao){
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
    printf("\nERRO 06: PROXIMO NO NAO LOCALIZADO.");
    fprintf(arquivo_saida,"\nERRO 06: PROXIMO NO NAO LOCALIZADO.");
    exit(1);
}
