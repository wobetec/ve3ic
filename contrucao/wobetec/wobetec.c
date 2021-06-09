/*
Olhar para refatorar:
    Criar as funcinalidades de jogo
    Colocar opcao no comeco do jogo
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
    no_complexo tipo_especifico;
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
void cadastrar_no(int indice, int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char *adcional_c);
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
        system("cls");
		if(ptr_atual->tipo != terminal){
			int indice_proximo_no = -1;
			while(indice_proximo_no == -1){
                switch(ptr_atual->tipo_especifico){
                    case passar:
                        printf("%s", ptr_atual->texto);
                        fprintf(arquivo_saida, "%s", ptr_atual->texto);
                        indice_proximo_no = ler_indice_proximo_no('#');
                        if(indice_proximo_no == -1){
                            printf("OPCAO INVALIDA!\n");
                            fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
                        }
                        system("pause");
                        break;
                    default:;
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
                    break;
                }
			}
            if(indice_proximo_no == 8){
                printf("Teste");
            }
            ptr_atual= buscar_no(indice_proximo_no);
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
void cadastrar_no(int indice, int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char *adcional_c){

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
            ptr->indice = indice;
            
            //gerando o código daquele nó
            if(code == -1){
                make += (indice*10000);
            }else{
                make += ((indice-(code%100))*10000 + code);
            }

            ptr->code = make;
            strcpy(ptr->texto, texto[n_textos]);
            ptr->tipo_especifico = passar;
            ptr->tipo = tipo;
            ptr->n_opcoes = n_opcoes;

            ptr->opcoes[0]= opcoes[n_opcoes-1];

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
            cadastrar_no(indice, -1, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            break;

        case sala_item:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            cadastrar_no(indice+1, codigo+1, texto, 1, tipo, passar, n_opcoes, opcoes, "");
            cadastrar_no(indice+2, codigo+2, texto, 2, tipo, passar, n_opcoes, opcoes, "");
            break;

        case pedra:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            break;

        case saida:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "");
            break;

        case fechadura:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, "CA2");
            break;

        case enigma:
            cadastrar_no(indice, codigo, texto, 0, tipo, nao_complexo, n_opcoes, opcoes, "");

            for(int i=0; i<n_opcoes; i++){
                cadastrar_no(indice+i+1, codigo+1+i, texto, i+1, tipo, passar, n_opcoes+1+i, opcoes, "");
            }

            break;

        case luta:
            if(n_opcoes==4){
                cadastrar_no(indice, codigo, texto, 0, tipo, passar, 1, opcoes, "");
                for(int i=0; i<n_opcoes-1; i++){
                    cadastrar_no(indice+1+i, codigo+1+i, texto, i+1, tipo, passar, 2+i, opcoes, "");
                }
            }else if(n_opcoes==5){
                cadastrar_no(indice, codigo, texto, 0, tipo, nao_complexo, 2, opcoes, "");
                for(int i=0; i<n_opcoes; i++){
                    cadastrar_no(indice+1+i, codigo+1+i, texto, i+1, tipo, passar, 3+i, opcoes, "");
                }
            }

            break;

        case nao_complexo:

            ptr->indice = indice;
            
            //gerando o código daquele nó
            if(code == -1){
                make += (indice*10000);
            }else{
                make += ((indice-(codigo%100))*10000 + codigo);
            }
            ptr->code = make;

            strcpy(ptr->texto, texto[n_textos]);
            ptr->tipo_especifico = nao_complexo;
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
    opcao opcoes_0[1] = {{'#', 1}};
    char texto_0[1][501] = {"-Que lugar eh esse? Onde eu estou?\n-Ta perdido aluno? - Disse uma voz misteriosa - Essa eh a masmorra da casa do trem, ngm nunca saiu daqui, nao vivo pelo menos. Eu sou o espirito Cerqueireanista que guia os nobres engenheiros nessa jornada. Ja vou avisando que estou com pressa, ja ja outro quadrupede vai cair aqui, entao pegue aquela faca e vamos. \n-Como assim? o que ta acontecendo?\n-Tudo sera respondido no seu tempo, mas por hora vamos em frente\n"};
	cadastrar_no(
        0,
        -1, 
        texto_0,
        0,
		raiz,
        entrada,
        1,
        opcoes_0,
        "");

    opcao opcoes_1[2] = {{'E', 2}, {'D', 100}};
    char texto_1[1][501] = {"Escolha logo por onde quer ir:\nE-Esquerda\nD-Direita\nDigite a opcao: "};
	cadastrar_no(
        1,
        -1, 
        texto_1,
        0,
		nao_terminal,
        nao_complexo,
        2,
        opcoes_1,
        "");
    
    opcao opcoes_2[3] = {{'E', 3}, {'D', 9}, {'V', 1}};
    char texto_2[1][501] = {"-Outra bifurcacao? \n-Nao pondere aluno... \nE-Esquerda \nD-Direita \nV-Voltar \nDigite a opcao: "};
	cadastrar_no(
        2,
        -1, 
        texto_2,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_2,
        "");

    opcao opcoes_3[8] = {{'1', 4}, {'2', 5}, {'3', 6}, {'4', 7}, {'#', 3}, {'#', 8}, {'#', 3}, {'#', 3}};
    char texto_3[5][501] = {"-Essa eh a sa salados enigmas, dizem que foi aqui que Einstein trancou e Galileu ficou maluco. Vamos ver do que voce eh capaz:\nENIGMA: Qual elemento da azul no teste de chama?\n1-Na  2-Cu  3-Mg  4-K \nDigite a opcao: ", "Qual a ideia aluno?\n", "Voce devia conhecer o tal de Lavoisier, ele tambem acertou essa.\n", "eu hein, ta perdido?\n", "kkkkkkkk\n"};
	cadastrar_no(
        3,
        -1, 
        texto_3,
        5,
		nao_terminal,
        enigma,
        4,
        opcoes_3,
        "");
    
    opcao opcoes_8[1] = {{'#', 2}};
    char texto_8[1][501] = {"-Uma gema! mas pra que eu vou usar isso?\n-Calma aluno, faz parte do seu path of warrior.\n"};
	cadastrar_no(
        8,
        -1, 
        texto_8,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_8,
        "");

    opcao opcoes_9[4] = {{'#', 10}, {'#', 11}, {'#', 12}, {'#', 13}};
    char texto_9[4][501] = {"-Corre Cerque alguma coisa, um monstroooo! \n-Antes ate dava para fugir se voce nao tivesse acordado ele, agora eh guerra tua. \n-O que eu facooo? ele ta vindo!! \n-Tenta acertar a faca nele imbecil.\n", "-Bom, mas vai ter que fazer melhor se quiser sair dessa vivo. \n-VIVO?\n", "-Por espartaaaa\n", "-Parece que voce esta pegando o jeito, mas isso nao foi um elogio, disse a mesma coisa mara o Isack e o infeliz morreu com uma maca que caiu na cabeca.\n"};
	cadastrar_no(
        9,4
        -1, 
        texto_9,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_9,
        "");
        
    opcao opcoes_13[1] = {{'#', 2}};
    char texto_13[1][501] = {"-Encontrou um gema, nem pergunte nada, so pegue.\n"};
	cadastrar_no(
        13,
        -1, 
        texto_13,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_13,
        "");
    


    char texto_f[1][501] = {"\nFIm apenas para fins de teste"};
	cadastrar_no(
        100,
        -1, 
        texto_f,
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
    if(opcao == '#'){
        return ptr_atual->opcoes[0].indice_proximo_no;
    }
    if(ptr_atual->opcoes[0].opcao_selecionada == '*') {
		return ptr_atual->opcoes[0].indice_proximo_no;
	}
	else {
		for(int i = 0; i < ptr_atual->n_opcoes; i++) {
			if(ptr_atual->opcoes[i].opcao_selecionada == opcao){
				return ptr_atual->opcoes[i].indice_proximo_no;
            }
		}
		return -1;
	}
    printf("\nERRO 06: PROXIMO NO NAO LOCALIZADO.");
    fprintf(arquivo_saida,"\nERRO 06: PROXIMO NO NAO LOCALIZADO.");
    exit(1);
}
