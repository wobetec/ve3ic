////////////////////////INCLUSAO DE BIBLIOTECAS///////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>


////////////////////////DEFINICAO DE CONSTANTES///////////////////////
#define NOME_ARQUIVO_SAIDA "gerador.txt"
#define MAX_OPCOES 10

//////////////////////////DEFINICAO DE ENUMS//////////////////////////
typedef enum _tipo_no {//enum exigido pelo professor
	raiz, nao_terminal, terminal
} tipo_no;

typedef enum _no_complexo {//enum dos superNOS
    passar, entrada, sala_item, pedra, saida, fechadura, enigma, luta, rancho, dialogo, nao_complexo,
} no_complexo;


/////////////////////////DEFINICAO DE STRUCTS/////////////////////////
typedef struct _opcao{//Define uma opcao como um char e um indice de destino
    char opcao_selecionada;
    int indice_proximo_no;
} opcao;

typedef struct _no {//struct que armazena os nos, struct destinado aos ponteiros de no
	int indice;
    int code; //7pppttff (p=indice do pai, t=tipo, f="indice" do filho)
	char texto[501];
    no_complexo tipo_especifico;
	tipo_no tipo;
	int n_opcoes;
	opcao opcoes[MAX_OPCOES];
    char endereco_imagem[65];
	struct _no *ant;
	struct _no *prox;
} no;


//////////////////////////VARIAVEIS GLOBAIS///////////////////////////
no *ptr_inicio, *ptr_atual;//Referencia para o inicio e para a posicao atual na lista encadeada
FILE *arquivo_saida;//Referencia para o arquivo de saida

/////////////////////////////PROTOTIPOS///////////////////////////////
void cadastrar_no(int indice, int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char *endereco_imagem);
void cadastrar_nos();
no *buscar_no(int indice);
void apagar_lista();

void printar_imagem();

void imagem(char *nome_arquivo);


char *retornar_tipo1(){
    int code = ( ptr_atual->code % 10000)/100;
    switch(code){
        case passar:
            return "passar";
            break;
        case entrada:
            return "entrada";
            break;
        case sala_item:
            return "item";
            break;
        case pedra:
            return "joia";
            break;
        case saida:
            return "saida";
            break;
        case fechadura:
            return "fechadura";
            break;
        case enigma:
            return "enigma";
            break;
        case luta:
            return "luta";
            break;
        case rancho:
            return "rancho";
            break;
        case dialogo:
            return "dialogo";
            break;
        case nao_complexo:
            return "nao_complexo";
            break;
        default:
            return "ERRO";
            break;
        }

}

char *retornar_tipo2(){
    switch(ptr_atual->tipo){
        case raiz:
            return "raiz";
            break;
        case nao_terminal:
            return "nao_terminal";
            break;
        case terminal:
            return "terminal";
            break;
        default:
            break;
    }
    return NULL;
}

/////////////////////////////////MAIN/////////////////////////////////
int main(){

    if(!(arquivo_saida = fopen(NOME_ARQUIVO_SAIDA, "w"))){//Inicialização do processo de log
        printf("ERRO 01: ERRO AO ABRIR O ARQUIVO DE SAIDA.");
        exit(1);
    }
    
    //inicializacoes necessarias
    cadastrar_nos();
    ptr_atual = buscar_no(0);

    //Tela de boas vindas
    while(ptr_atual!=NULL){
        printf("%d\n", ptr_atual->indice);
        fprintf(arquivo_saida, "%3d  %s\n", ptr_atual->indice, retornar_tipo1());
        fprintf(arquivo_saida, "%s\n", retornar_tipo2());
        fprintf(arquivo_saida, "%s\n", ptr_atual->endereco_imagem);
        fprintf(arquivo_saida, "%s\n", ptr_atual->texto);
        for(int j=0; j<ptr_atual->n_opcoes; j++){
            if(ptr_atual->opcoes[j].opcao_selecionada != NULL){
                fprintf(arquivo_saida, "%c vai para no de indice: %3d\n", ptr_atual->opcoes[j].opcao_selecionada, ptr_atual->opcoes[j].indice_proximo_no);
            }else{
                break;
            }
            
        }
        fprintf(arquivo_saida, "\n#########################################################################################\n\n");
        ptr_atual = ptr_atual->prox;
    }

    //Libera o espaço utlizado pela lista encadeada
    apagar_lista();

    //fecha o arquivo de log
    fclose(arquivo_saida);

    return 0;
}


//##########CRIACAO DE NOS E MANIPULACAO DA LISTA ENCADEADA##########//
void cadastrar_no(int indice, int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char endereco_imagem[64]){//adiciona os nos a lista encadeada, e cria os nos baseado no tipo 

    //alocação dinâmica da memória
    no *ptr = (no *) malloc(sizeof(no));
    if(ptr == NULL){
        printf("\nERRO 03: ERRO NA ALOCACAO DE MEMORIA.");
        fprintf(arquivo_saida, "\nERRO 03: ERRO NA ALOCACAO DE MEMORIA.");
        exit(1);
    }

    //Variáveis utilizadas em varios switchs e que deram erro
    no *ptr_aux = ptr_inicio;
    int codigo;
    if(code == -1){
        codigo = compl*100;
    }else{
        codigo = code;
    }
    
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

            strcpy(ptr->endereco_imagem, endereco_imagem);
            

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
            cadastrar_no(indice, -1, texto, 0, tipo, passar, n_opcoes, opcoes, endereco_imagem);
            break;

        case sala_item:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, 1, opcoes, endereco_imagem);
            cadastrar_no(indice+1, codigo+1, texto, 1, tipo, passar, 2, opcoes, endereco_imagem);
            break;

        case pedra:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, endereco_imagem);
            break;

        case saida:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, endereco_imagem);
            break;

        case fechadura:
            cadastrar_no(indice, codigo, texto, 0, tipo, nao_complexo, n_opcoes, opcoes, endereco_imagem);
            cadastrar_no(indice+1, codigo+1, texto, 1, tipo, passar, n_opcoes+1, opcoes, endereco_imagem);
            cadastrar_no(indice+2, codigo+1, texto, 2, tipo, passar, n_opcoes+2, opcoes, endereco_imagem);
            break;

        case enigma:
            cadastrar_no(indice, codigo, texto, 0, tipo, nao_complexo, n_opcoes, opcoes, endereco_imagem);

            for(int i=0; i<n_opcoes-1; i++){
                cadastrar_no(indice+i+1, codigo+1+i, texto, i+1, tipo, passar, n_opcoes+1+i, opcoes, endereco_imagem);
            }

            break;

        case luta:
            if(n_opcoes==4){
                cadastrar_no(indice, codigo, texto, 0, tipo, passar, 1, opcoes, endereco_imagem);
                for(int i=0; i<n_opcoes-1; i++){
                    cadastrar_no(indice+1+i, codigo+1+i, texto, i+1, tipo, passar, 2+i, opcoes, endereco_imagem);
                }
            }else if(n_opcoes==5){
                cadastrar_no(indice, codigo, texto, 0, tipo, nao_complexo, 2, opcoes, endereco_imagem);
                for(int i=0; i<n_opcoes-2; i++){
                    cadastrar_no(indice+1+i, codigo+1+i, texto, i+1, tipo, passar, 3+i, opcoes, endereco_imagem);
                }
            }

            break;

        case rancho:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, 1, opcoes,endereco_imagem);
            break;

        case dialogo:
            for(int i=0; i<n_opcoes; i++){
                cadastrar_no(indice+i, codigo+i, texto, i, tipo, passar, 1+i, opcoes, endereco_imagem);
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

            strcpy(ptr->endereco_imagem, endereco_imagem);


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

void cadastrar_nos(){//contem os nos da historia e executa o cadastrar_no varias vezes


}

no *buscar_no(int indice){//funcao para buscar no a partir do indice
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

void apagar_lista(){//funcao para liberar a memoria alocada
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


//#################FUNCOES PARA SIMPLIFICAR A PARTE DE EXECUCAO DA MAIN#################//

void printar_imagem(){//verifica se o no possui imagem vinculada e printa em caso afirmativo
    if(ptr_atual->endereco_imagem[0] == '.'){
        imagem(ptr_atual->endereco_imagem);
    }
}


//###########################PARTE GRAFICA##########################//
void imagem(char *nome_arquivo){//print as imagens na tela e escreve no arquivo de saida
    FILE* arquivo;
    char linha[91], *leitura=NULL;

    arquivo=fopen(nome_arquivo, "rt");

    if(arquivo==NULL){
        printf("\nERRO 07: ERRO AO ABRIR A IMAGEM.");
        fprintf(arquivo, "\nERRO 07: ERRO AO ABRIR A IMAGEM."); 
    }

    while(!feof(arquivo)){
        leitura=fgets(linha, 91, arquivo);
            if(leitura){
                printf("%s", linha);
				fprintf(arquivo_saida, "%s", linha);
            }
    }      
    printf("\n"); 
    fprintf(arquivo_saida, "\n");
    fclose(arquivo);
}

