/*
Olhar para refatorar:
    implementar check point
    implementar morte
    implementar sistema de XP
*/

////////////////////////INCLUSAO DE BIBLIOTECAS///////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

////////////////////////DEFINICAO DE CONSTANTES///////////////////////
#define NOME_ARQUIVO_ENTRADA "entrada.txt"
#define NOME_ARQUIVO_SAIDA "saida.txt"
#define MAX_OPCOES 10

//define que pode ser alterado dependo da história (começa no zero)
#define MAX_TEXTO 5
    
//////////////////////////DEFINICAO DE ENUMS//////////////////////////
typedef enum _tipo_no {
	raiz, nao_terminal, terminal
} tipo_no;

typedef enum _no_complexo {
    passar, entrada, sala_item, pedra, saida, fechadura, enigma, luta, rancho, dialogo, nao_complexo,
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
    char endereco_imagem[65];
	struct _no *ant;
	struct _no *prox;
} no;

//////////////////////////VARIAVEIS GLOBAIS///////////////////////////
//Referencia para o inicio e para a posicao atual na lista encadeada
no *ptr_inicio, *ptr_atual;
//Referencia para o arquivo de saida
FILE *arquivo_saida;
//referencia para a criação dos nós
int contador = 0;

//Criterios globais
int vida = 100;
int vida_MAX = 100;

//pedras
int pedras = 0;
int ja_coletas[15];
int topo_ja_coletas = -1;

//atributos globais
int XP = 60;
int nivel = 2;
int ataque = 14;
int armadura = 12;

//vida e ataque do inimigo para lutas
int vida_inimigo=-1, vida_inimigo_MAX=0, ataque_inimigo=0;

//itens: item daquela fase
int itens[2]={0, 0};

//salas bloqueadas por ja terem sido acessadas
int salas_bloqueadas[128], cont_salas = 0;

/////////////////////////////PROTOTIPOS///////////////////////////////
char *ler_nome_jogador(char *nome_arquivo);

//Gerenciamento da lista encadeada
void cadastrar_no(int indice, int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char *endereco_imagem);
no *buscar_no(int indice); //busa no na lista encadeada
void apagar_lista();//freela alista encadeada

//cadastro dos nós
void cadastrar_nos();//aplica a cadastrar_no varias vezes
void cadastrar_nos1();//apenas para testes

//secundarias
int ler_indice_proximo_no(char opcao);

void printar_imagem();
void printar_esperar(int *indice_proximo_no);
void printar_opcao(char *opcao, int *indice_proximo_no);

//parte grafica
void imagem(char *nome_arquivo);
void pausa();
void limpar();
void resize();
void barra_superior();
void barra_superior_luta();

//mecanica do jogo
void sala_pedras(int indice);
void atualizar_vida(int ganho, int valor);
void dano_vida_luta();
void set_luta();

void bloquear_no(int indice);
int checar_bloqueio(int indice);
void add_item(int id);
int check_item(int id);

/////////////////////////////////MAIN/////////////////////////////////
int main(){
    //redimensiona o terminal
    resize();

    //Inicialização do processo de log
    if(!(arquivo_saida = fopen(NOME_ARQUIVO_SAIDA, "w"))){
        printf("ERRO 01: ERRO AO ABRIR O ARQUIVO DE SAIDA.");
        exit(1);
    }

    //Montagem da lista encadeada
    cadastrar_nos();

    //Tela de boas vindas
    limpar();
    imagem("./imagens/bemvindo.txt");
    char nome_jogador[100];
    strcpy(nome_jogador, ler_nome_jogador(NOME_ARQUIVO_ENTRADA));
    printf("Bem-vindo a masmorra da casa do trem, %s!\n", nome_jogador);
    fprintf(arquivo_saida, "Bem-vindo a masmorra da casa do trem, %s!\n", nome_jogador);
    pausa();

    salas_bloqueadas[0]=0;

    //carregar o no zero
    ptr_atual = buscar_no(0);

    //Laco principal
    while(1) {
        //Se no nao eh terminal, apresentar texto e ler a opcao selecionada
        if(ptr_atual->tipo != terminal){
            int code = ( ptr_atual->code % 10000)/100;
            int indice_proximo_no = -1;
            while(indice_proximo_no == -1){
                switch(ptr_atual->tipo_especifico){
                    case passar:
                        switch(code){
                            case sala_item:
                                bloquear_no(ptr_atual->indice);
                                add_item(0);
                                contador = 0;
                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_esperar(&indice_proximo_no);
                                break;

                            case enigma:
                                bloquear_no(ptr_atual->indice);
                                contador = 0;
                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_esperar(&indice_proximo_no);
                                break;

                            case pedra:
                                bloquear_no(ptr_atual->indice);
                                contador = 0;
                                limpar();
                                barra_superior();
                                printar_imagem();
                                sala_pedras(ptr_atual->indice);
                                printar_esperar(&indice_proximo_no);
                                break;

                            case luta:
                                bloquear_no(ptr_atual->indice);
                                if(contador == 0){
                                    set_luta();
                                    contador = 1;
                                }else{
                                    dano_vida_luta();
                                }

                                limpar();
                                barra_superior_luta();
                                printar_imagem();
                                printar_esperar(&indice_proximo_no);
                                break;

                            case rancho:
                                atualizar_vida(1, 20);

                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_esperar(&indice_proximo_no);
                                break;

                            default:
                                contador = 0;
                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_esperar(&indice_proximo_no);
                                break;
                        }
                        break;
                    default:
                        switch(code){;
                            char opcao;
                            case fechadura:
                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_opcao(&opcao, &indice_proximo_no);

                                if(indice_proximo_no >= 10000){
                                    int id = indice_proximo_no/10000;
                                    if(check_item(id)){
                                        indice_proximo_no = indice_proximo_no%1000;
                                    }else{
                                        indice_proximo_no = indice_proximo_no%1000 + 1;
                                    }
                                }else{
                                    if(indice_proximo_no/1000 == pedras && pedras != 0){
                                        indice_proximo_no = indice_proximo_no %1000;
                                        pedras=0;
                                    }else if(indice_proximo_no/1000>0){
                                        indice_proximo_no = indice_proximo_no %1000 + 1;
                                    }
                                }
                                break;
                            
                            case luta:
                                if(contador == 0){
                                    set_luta();
                                    contador = 1;
                                }else{
                                    dano_vida_luta();
                                }
                                limpar();
                                barra_superior_luta();
                                printar_imagem();
                                printar_opcao(&opcao, &indice_proximo_no);
                                break;

                            default:
                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_opcao(&opcao, &indice_proximo_no);
                                break;
                        }
                    break;
                }
            }
            if(!checar_bloqueio(indice_proximo_no)){
                ptr_atual = buscar_no(indice_proximo_no);
            }else{
                ptr_atual = ptr_atual;
                limpar();
                barra_superior();
                imagem("./imagens/caminho_bloqueado.txt");
                printf("\nEste caminho esta bloqueado.\nVoce ja passou por aqui, esta perdido por acaso?\n");
                fprintf(arquivo_saida, "\nEste caminho esta bloqueado.\nVoce ja passou por aqui, esta perdido por acaso?\n");
                pausa();
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
void cadastrar_no(int indice, int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char endereco_imagem[64]){

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
            cadastrar_no(indice+2, codigo+2, texto, 2, tipo, passar, 3, opcoes, endereco_imagem);
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


void cadastrar_nos(){
    /* MODELO DE CADASTRO
        cadastrar_no(
            indice,
            code, 
            texto[][501],
            n_textos,
            tipo,
            compl,
            n_opcoes,
            opcoes,
            endereco_imagem)
    */
    opcao opcoes_0[1] = {{'#', 1}};
    char texto_0[1][501] = {"Quinta-feira pela manha, aula de AlgLin que parece nao ter fim, e la esta o Aluno do IMEnso instituto, tentando se concentrar na aulinha enquanto nao para de pensar no Empire of Dungeons, um jogo que encontrou na internet recentemente e nao ve a hora de comprar. Essa aula esta realmente chata e o aluno esta muito concentrado em seus pensamentos, imagiando cada detalhe do jogo, quando derrepente um clarao.\n"};
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

    opcao opcoes_1[3] = {{'#', 2}, {'#', 3}, {'#', 4}};
    char texto_1[3][501] = {"-Voz misteriosa: Ola, quem vos fala eh o criador desse belo jogo. Por enquanto, voces nao saberao quem sou eu... na verdade, poucos descobrirao. O que está em jogo agora é a suas vidas.\n", "Se conseguirem chegar ao ultimo andar da torre de 5 andares que se apresenta adiante, resolvendo todos os enigmas necessarios, coletando todos os itens, tornando-se mais fortes e derrotando os BOSS de cada andar, sairao vivos.\n", "Caso contrario, vocês ja sabem. Por que estou fazendo isso? pura diversão. No mais, boa sorte!\n"};
	cadastrar_no(
        1,
        -1, 
        texto_1,
        3,
		nao_terminal,
        dialogo,
        3,
        opcoes_1,
        "./imagens/andar1/guianu.txt");

    opcao opcoes_4[2] = {{'E', 5}, {'D', 10}};
    char texto_4[1][501] = {"-Voce: Uma bifurcacao, por onde devo ir? \nE-Esquerda \nD-Direita \nDigite uma opcao: "};
	cadastrar_no(
        4,
        -1, 
        texto_4,
        0,
		nao_terminal,
        nao_complexo,
        2,
        opcoes_4,
        "./imagens/labirinto2.txt");

    opcao opcoes_5[4] = {{'#', 6}, {'#', 7}, {'#', 8}, {'#', 9}};
    char texto_5[4][501] = {"-Voce: O que eh aquilo, parece um goblim, ele esta dormindo.\n-Servo: Errado Aluno, nao esta mais.\n","-Goblim: lara ratu peri.\n-Voce: Seja la o que for, eu quem vou ganhar, sou treinado na arte das integrais multiplas.\n","-Goblim: baru sate lorem.-Voce: morraaaaa!\n", "-Servo: estou impressionado de que voce tenha saido dessa vivo, a maioria morre antes de virar a esquerda.\n-Voz misteriosa: Nao encoraje esse quadrupede, esse eh soh o principio das dores\n"};
	cadastrar_no(
        5,
        -1, 
        texto_5,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_5,
        "./imagens/batalha4.txt");
    
    opcao opcoes_9[1] = {{'#', 4}};
    char texto_9[1][501] = {"-Voce: Olha, parece so uma pedra, devo pegar?\n-Servo: Nao estou aqui para ser sua baba, faca o que quiser\n-Voce: vou levar comigo.\n-Voz misteriosa: Veja se nao temos um colecionador de pedras aqui, vai mesmo guardar essa pedra?\n-Voce: Vai que preciso jogar em alguem\n-Voz misteriosa: ou em algo."};
	cadastrar_no(
        9,
        -1, 
        texto_9,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_9,
        "./imagens/gema.txt");

    opcao opcoes_10[4] = {{'E', 3}, {'F', 9}, {'D', 1}, {'V', 4}};
    char texto_10[1][501] = {"-Voce: 3 caminhos agora, estou começando a me perder aqui.\n-Servo: Hehe, soh continue andando aluno, quanto mais cedo voce morrer, mais cedo estou livre.\nE-Esquerda \nF-Frente \nD-Direita \nV-Voltar \nDigite uma opcao: "};
	cadastrar_no(
        10,
        -1, 
        texto_10,
        0,
		nao_terminal,
        nao_complexo,
        4,
        opcoes_10,
        "./imagens/labirinto3.txt");
    
    opcao opcoes_11[4] = {{'#', 12}, {'#', 13}, {'#', 14}, {'#', 15}};
    char texto_11[4][501] = {"-Voce(sussurando): Aquilo eh um fantasma?\n-Servo(sussurando): Acorde ele e descubra.*Lutar *Voltar\n", "-Servo: agora me diga como acertar um fantasma?\n-Voce: facil, eles sao frageis contra raios, e basta gerar um pouco de atrito que consigo produzir faiscas.\n", "-Fantasma: ihhhhhhhhh.\n-Voce: Morra denovo seu infame.\n", "-Voz misteriosa: Vai ter que fazer muito melhor que isso se quiser sair vivo desse lugar.\n"};
	cadastrar_no(
        11,
        -1, 
        texto_11,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_11,
        "./imagens/batalha2.txt");
    
    opcao opcoes_15[3] = {{'#', 16}, {'#', 17}, {'#', 14}};
    char texto_15[3][501] = {"-Voce: Uau, que sala incrivel, deve ter alguma armadilha aqui, parece aquelas salas do tesouro.\n-Goblim: Cala a boca aluno, so joga, voce fala demais.\n-Voce: Olha, tem mesmo um tesouro ali, o que eh aquilo?\n", "-Voz misteriosa: O elmo de Richard Franco,forjado com escama de dragoes nas profundezas da masmorra do trem. Eh um objeto indispensavel para derrotar o TC Rock.\n", "-Voce: parece pesado, mas eh muito legal.\n"};
	cadastrar_no(
        15,
        -1, 
        texto_15,
        3,
		nao_terminal,
        sala_item,
        3,
        opcoes_15,
        "./imagens/personagemelmo.txt");
    
    /*
    opcao opcoes_$[3] = {{'E', 3}, {'D', 9}, {'V', 1}};
    char texto_$[1][501] = {"-Outra bifurcacao? \n-Nao pondere aluno... \nE-Esquerda \nD-Direita \nV-Voltar \nDigite a opcao: "};
	cadastrar_no(
        $,
        -1, 
        texto_$,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_$,
        "./imagens/labirinto2.txt");
    */

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

//printa o texto, olha o proximo indice
void printar_esperar(int *indice_proximo_no){
    printf("%s", ptr_atual->texto);
    fprintf(arquivo_saida, "%s", ptr_atual->texto);
    *indice_proximo_no = ler_indice_proximo_no('#');
    if(*indice_proximo_no == -1){
        printf("OPCAO INVALIDA!\n");
        fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
    }
    pausa();
}

void printar_opcao(char *opcao, int *indice_proximo_no){
    printf("%s", ptr_atual->texto);
    fprintf(arquivo_saida, "%s", ptr_atual->texto);
    scanf(" %c", opcao);
    fprintf(arquivo_saida, "%c\n", *opcao);
    //Ler proximo no a partir da opcao
    *indice_proximo_no = ler_indice_proximo_no(*opcao);
    if(*indice_proximo_no == -1){
        printf("OPCAO INVALIDA!\n");
        fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
    }
}

void printar_imagem(){
    if(ptr_atual->endereco_imagem[0] == '.'){
        imagem(ptr_atual->endereco_imagem);
    }
}

//###########################PARTE GRAFICA##########################//

//print as imagens na tela e escreve no arquivo de saida
void imagem(char *nome_arquivo){
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

//barras de vida
void barra_superior(){
    // vida, xp, nivel, ataque, pedras
    //parte de cima
    char temp[90];
    printf("----------------------------- -----------------------------  -------  ---------  ---------\n");
    fprintf(arquivo_saida, "----------------------------- -----------------------------  -------  ---------  ---------\n");

    //vida
    printf("HP: ");
    fprintf(arquivo_saida, "%s", "HP: ");
    int frac = vida*17/vida_MAX;
    for(int i=0; i<frac; i++){
        printf("#");
        fprintf(arquivo_saida, "#");
    }
    for(int i=0; i<17-frac; i++){
        printf(" ");
        fprintf(arquivo_saida, " ");
    }
    printf(" %3d/%3d ", vida, vida_MAX);
    fprintf(arquivo_saida, " %3d/%3d ", vida, vida_MAX);

    //Xp
    printf("XP: ");
    fprintf(arquivo_saida, "%s", "XP: ");
    frac = XP*17/100;
    for(int i=0; i<frac; i++){
        printf("#");
        fprintf(arquivo_saida, "#");
    }
    for(int i=0; i<17-frac; i++){
        printf(" ");
        fprintf(arquivo_saida, " ");
    }
    printf(" %3d/%3d  ", XP, 100);
    fprintf(arquivo_saida, " %3d/%3d  ", XP, 100);

    
    //outras coisas NIVEL ATAQUE PEDRAS
    printf("NIVEL:%1d  ATAQUE:%2d  PEDRAS:%2d", nivel, ataque, pedras);
    fprintf(arquivo_saida, "NIVEL:%1d  ATAQUE:%2d  PEDRAS:%2d", nivel, ataque, pedras);

    //parte de baixo que eh copia da de cima
    printf("\n----------------------------- -----------------------------  -------  ---------  ---------\n");
    fprintf(arquivo_saida, "\n----------------------------- -----------------------------  -------  ---------  ---------\n");
}

void barra_superior_luta(){
    //parte de cima
    printf("------------------------------ ---------          --------- ------------------------------\n");
    fprintf(arquivo_saida, "------------------------------ ---------          --------- ------------------------------\n");
    //vida
    printf("HP: ");
    fprintf(arquivo_saida, "HP: ");
    int frac = vida*18/vida_MAX;
    for(int i=0; i<frac; i++){
        printf("#");
        fprintf(arquivo_saida, "#");
    }
    for(int i=0; i<18-frac; i++){
        printf(" ");
        fprintf(arquivo_saida, " ");
    }
    printf(" %3d/%3d ", vida, vida_MAX);
    fprintf(arquivo_saida, " %3d/%3d ", vida, vida_MAX);

    printf("ATAQUE:%2d          ATAQUE:%2d ", ataque, ataque_inimigo);
    fprintf(arquivo_saida, "ATAQUE:%2d          ATAQUE:%2d ", ataque, ataque_inimigo);

    //vida_inimigo
    printf("HP: ");
    frac = vida_inimigo*18/vida_inimigo_MAX;
    for(int i=0; i<frac; i++){
        printf("#");
        fprintf(arquivo_saida, "#");
    }
    for(int i=0; i<18-frac; i++){
        printf(" ");
        fprintf(arquivo_saida, " ");
    }
    printf(" %3d/%3d", vida_inimigo, vida_inimigo_MAX);
    fprintf(arquivo_saida, " %3d/%3d", vida_inimigo, vida_inimigo_MAX);

    //parte de baixo copia da de cima
    printf("\n------------------------------ ---------          --------- ------------------------------\n");
    fprintf(arquivo_saida, "\n------------------------------ ---------          --------- ------------------------------\n");
}

//parte gráfica do cmd
void limpar(){//portavel
    #ifdef LINUX
    system ("clear");
    #elif defined WIN32
    system ("cls");
    #else
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    #endif
}

void pausa(){//portavel
    #ifdef WIN32
    system("pause");
    fprintf(arquivo_saida, "Pressione qualquer tecla para continuar. . .\n");
    #else
    char c;
    printf("\nPressione ENTER para continuar. . .");
    fprintf(arquivo_saida, "\nPressione ENTER para continuar. . .");
    scanf("%c%*c", &c);
    #endif

    
}

void resize(){//portavel
    #ifdef WIN32
    system("MODE con cols=91 lines=35 ");
    #else
    printf("Para uma melhor experiencia, redimencione seu terminal para 90x35\n");
    fprintf(arquivo_saida, "Para uma melhor experiencia, redimencione seu terminal para 90x35\n");
    pausa();
    #endif
}

//#########################MECANICA DE JOGO#########################//

//mecanica das pedras para abrir portas
void sala_pedras(int indice){
    int contador=0;
    for(int i=0; i<topo_ja_coletas+1; i++){
        if(indice == ja_coletas[i]){
            contador++;
            break;
        }
    }
    if(contador == 0){
        topo_ja_coletas++;
        pedras++;
        ja_coletas[topo_ja_coletas] = indice;
    }
}

//adiciona o remove vida do personagem principal( ganho = 0 --> perda)
void atualizar_vida(int ganho, int valor){
    if(ganho == 0){
        vida-=valor;
    }else{
        if(vida + valor >= vida_MAX){
            vida = vida_MAX;
        }else{
            vida+=valor;
        }
    }
}

//coloca uma vida inicial para o inimigo
void set_luta(){
    if(vida_inimigo <= 0){
        vida_inimigo = 100;
        vida_inimigo_MAX = 100;
        ataque_inimigo = 15;
    }
}

//da e recebe dano(altera direto nas globais) plausivel de ser modificada para dano constante
void dano_vida_luta(){
    srand(time(NULL));
    int dano_recebido, dano_aflingido;

    dano_recebido = (100 - armadura)*ataque_inimigo/200;
    dano_aflingido = ataque * (rand()% 100 + 200)/100;

    atualizar_vida(0, dano_recebido);
    if(vida_inimigo-dano_aflingido <= 0){
        vida_inimigo = 0;
    }else{
        vida_inimigo-=dano_aflingido;
    }
}

void bloquear_no(int indice){
    cont_salas++;
    salas_bloqueadas[cont_salas-1] = indice;
}

int checar_bloqueio(int indice){
    for(int i=0; i<=cont_salas; i++){
        if(salas_bloqueadas[i] == indice){
            return 1;
            break;
        }
    }
    return 0;
}

void add_item(int id){
    itens[id]++;
}

int check_item(int id){
    if(itens[id] != 0){
        return 1;
    }else{
        return 0;
    }
}

