/*
Olhar para refatorar:
    fazer as pedras diminuirem na fechadura
    bloquear salas ja passadas
    sistema de perda e ganho de vida 
    implementar o sistema de vida a luta
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

////////////////DEFINICAO DE STRUCTS DOS NOS COMPLEXOS////////////////


//////////////////////////VARIAVEIS GLOBAIS///////////////////////////
//Referencia para o inicio e para a posicao atual na lista encadeada
no *ptr_inicio, *ptr_atual;
//Referencia para o arquivo de saida
FILE *arquivo_saida;
//referencia para a criação dos nós
int contador_de_no = -1;

//Criterios globais
int vida = 50;
int vida_MAX = 100;

//pedras
int pedras = 0;
int ja_coletas[15];
int topo_ja_coletas = -1;

//atributos globais
int XP = 0;
int nivel = 0;
int ataque = 14;

//vida e ataque do inimigo para lutas
int vida_inimigo=-1, vida_inimigo_MAX=0, ataque_inimigo=0;

/////////////////////////////PROTOTIPOS///////////////////////////////
char *ler_nome_jogador(char *nome_arquivo);

//Gerenciamento da lista encadeada
void cadastrar_no(int indice, int code, char texto[][501], int n_textos, tipo_no tipo, no_complexo compl, int n_opcoes, opcao *opcoes, char *endereco_imagem);
no *buscar_no(int indice); //busa no na lista encadeada
void apagar_lista();//freela alista encadeada

//cadastro dos nós
void cadastrar_nos();//aplica a cadastrar_no varias vezes

//secundarias
int ler_indice_proximo_no(char opcao);
void imagem(char *nome_arquivo);

//parte grafica
void pausa();
void limpar();
void barra_superior();
void barra_superior_inimigo();

//mecanica do jogo
void sala_pedras(int indice);
void atualizar_vida(int ganho, int valor);
void dano_vida_luta();


/////////////////////////////////MAIN/////////////////////////////////
int main(){
    //redimensiona o terminal
    system("MODE con cols=91 lines=35 ");

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
    //Página inicial com menuzinho
    char nome_jogador[100];
    strcpy(nome_jogador, ler_nome_jogador(NOME_ARQUIVO_ENTRADA));
    printf("Bem-vindo ao LABIRINTO de IC, %s!\n", nome_jogador);
    fprintf(arquivo_saida, "Bem-vindo ao LABIRINTO de IC, %s!\n", nome_jogador);
    pausa();

    //carregar o no zero
    ptr_atual = buscar_no(0);

    //Laco principal
    while(1) {
        limpar();

        barra_superior();

        //apresneta a imagem de cada no
        if(ptr_atual->endereco_imagem[0] == '.'){
            imagem(ptr_atual->endereco_imagem);
        }

        //Se no nao eh terminal, apresentar texto e ler a opcao selecionada
        if(ptr_atual->tipo != terminal){
            int code = ( ptr_atual->code % 10000)/100;
            int indice_proximo_no = -1;
            while(indice_proximo_no == -1){
                switch(ptr_atual->tipo_especifico){
                    case passar:
                        switch(code){
                            case pedra:
                                sala_pedras(ptr_atual->indice);
                                printf("%s", ptr_atual->texto);
                                fprintf(arquivo_saida, "%s", ptr_atual->texto);
                                indice_proximo_no = ler_indice_proximo_no('#');
                                if(indice_proximo_no == -1){
                                    printf("OPCAO INVALIDA!\n");
                                    fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
                                }
                                pausa();
                                break;

                            case luta:
                                limpar();
                                barra_superior_luta();
                                //apresneta a imagem de cada no
                                if(ptr_atual->endereco_imagem[0] == '.'){
                                    imagem(ptr_atual->endereco_imagem);
                                }

                                sala_pedras(ptr_atual->indice);
                                printf("%s", ptr_atual->texto);
                                fprintf(arquivo_saida, "%s", ptr_atual->texto);
                                indice_proximo_no = ler_indice_proximo_no('#');
                                if(indice_proximo_no == -1){
                                    printf("OPCAO INVALIDA!\n");
                                    fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
                                }
                                pausa();
                                break;

                            case rancho:
                                atualizar_vida(1, 20);
                                limpar();
                                barra_superior();
                                if(ptr_atual->endereco_imagem[0] == '.'){
                                    imagem(ptr_atual->endereco_imagem);
                                }

                                sala_pedras(ptr_atual->indice);
                                printf("%s", ptr_atual->texto);
                                fprintf(arquivo_saida, "%s", ptr_atual->texto);
                                indice_proximo_no = ler_indice_proximo_no('#');
                                if(indice_proximo_no == -1){
                                    printf("OPCAO INVALIDA!\n");
                                    fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
                                }
                                pausa();
                                break;
                            default:
                                printf("%s", ptr_atual->texto);
                                fprintf(arquivo_saida, "%s", ptr_atual->texto);
                                indice_proximo_no = ler_indice_proximo_no('#');
                                if(indice_proximo_no == -1){
                                    printf("OPCAO INVALIDA!\n");
                                    fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
                                }
                                pausa();
                                break;
                        }
                        break;
                    default:
                        switch(code){;
                            char opcao;
                            case fechadura:
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

                                if(indice_proximo_no/1000 == pedras){
                                    indice_proximo_no = indice_proximo_no %1000;
                                    pedras = pedras - (indice_proximo_no/1000);
                                }else{
                                    indice_proximo_no = indice_proximo_no %1000 + 1;
                                }
                                break;
                            default:
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

            for(int i=0; i<n_opcoes; i++){
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


//cadastras todos os nos na lista encadeada
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
    char texto_0[1][501] = {"No de entrada\n"};
	cadastrar_no(
        0,
        -1, 
        texto_0,
        0,
		raiz,
        entrada,
        1,
        opcoes_0,
        "./imagens/labirinto1.txt");

    opcao opcoes_1[4] = {{'#', 2}, {'#', 3}, {'#', 4}, {'#', 5}};
    char texto_1[4][501] = {"Texto 1 dialogo\n", "Texto 2 dialogo\n", "Texto 3 dialogo\n", "Texto 4 dialogo\n"};
	cadastrar_no(
        1,
        -1, 
        texto_1,
        0,
		nao_terminal,
        dialogo,
        4,
        opcoes_1,
        "./imagens/guianu.txt");
    
    opcao opcoes_5[2] = {{'#', 6}};
    char texto_5[1][501] = {"mais vida +20\n"};
	cadastrar_no(
        5,
        -1, 
        texto_5,
        0,
		nao_terminal,
        rancho,
        1,
        opcoes_5,
        "./imagens/labirinto1.txt");
    
    char texto_6[1][501] = {"FIM - terminal"};
	cadastrar_no(
        6,
        -1, 
        texto_6,
        0,
		terminal,
        nao_complexo,
        0,
        NULL,
        "./imagens/personagem.txt");


    /*
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
        "./imagens/labirinto1.txt");

    opcao opcoes_1[2] = {{'E', 2}, {'D', 14}};
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
        "./imagens/labirinto2.txt");
    
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
        "./imagens/labirinto2.txt");

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
        "./imagens/cenario1.txt");
    
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
        "./imagens/gema.txt");

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
        "./imagens/batalha1.txt");
        
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
        "./imagens/gema.txt");

    opcao opcoes_14[3] = {{'E', 20}, {'D', 15}, {'V', 1}};
    char texto_14[1][501] = {"-Vamos logo, estou com pressa, escolha por onde ir: \nE-Esquerda \nD-Direita \nV-Voltar \nDigite a opcao: "};
	cadastrar_no(
        14,
        -1, 
        texto_14,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_14,
        "./imagens/labirinto2.txt");

    opcao opcoes_15[4] = {{'#', 16}, {'#', 17}, {'#', 18}, {'#', 19}};
    char texto_15[4][501] = {"-O que eh aquilooo?? \n-Nao precisava descobir agora... mas ja que gritou n tem jeito, guerra tua. \n-Ele vai me mataaaar!! \n-Reaje aluno, a faca ai na mao nao eh para ser enfeite\n", "-Continua aluno. \n-Eu nao aguento maaais.\n", "-Thunder, thunder, thundercaaaats.\n", "-Agora o aluno ficou maluco de vez, mais um, pelo menos saiu vivo\n"};
	cadastrar_no(
        15,
        -1, 
        texto_15,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_15,
        "./imagens/batalha4.txt");
    
    opcao opcoes_19[1] = {{'#', 14}};
    char texto_19[1][501] = {"-OLha la, uma gema.\n-So pega logo aluno\n"};
	cadastrar_no(
        19,
        -1, 
        texto_19,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_19,
        "./imagens/gema.txt");
    
    opcao opcoes_20[3] = {{'E', 27}, {'D', 21}, {'V', 14}};
    char texto_20[1][501] = {"-Por onde vamos seguir aluno? \nE-Esquerda \nD-Direita \nV-Voltar \nDigite a opcao: "};
	cadastrar_no(
        20,
        -1, 
        texto_20,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_20,
        "./imagens/labirinto2.txt");

    opcao opcoes_21[4] = {{'1', 3022}, {'2', 20}, {'#', 24}, {'#', 21}};
    char texto_21[3][501] = {"-Uma porta, o que eu tenho que fazer? \n-Voce eh burro? tem 3 buracos, acho que voce precisa colocar algo la ne... \n-verdade \n-quer tentar? \n1-Tentar Abrir a passagem \n2-Voltar \nDigite a opcao: ", "-Bom, parece que voce consegui as 3 gemas.\n", "-Va procurar as gemas e volte outra hora.\n"};
	cadastrar_no(
        21,
        -1, 
        texto_21,
        3,
		nao_terminal,
        fechadura,
        2,
        opcoes_21,
        "./imagens/porta3.txt");
    
    
    opcao opcoes_24[3] = {{'#', 25}, {'#', 26}, {'#', 20}};
    char texto_24[3][501] = {"-Olha, que sala cool. O que eh aquilo? \n-Va la e veja.\n", "-Uau, um peitora. \n-Nao eh um peitoral qualquer, esse foi utilizado pelo proprio ricardo franco na guerra contra os australopitecos\n", "-Posso pegar? \n-Se eu fosse voce eu pegaria, vai precisar para continuar vivo hehe.\n"};
	cadastrar_no(
        24,
        -1, 
        texto_24,
        3,
		nao_terminal,
        sala_item,
        1,
        opcoes_24,
        "");
    
    
    opcao opcoes_27[5] = {{'N', 20}, {'S', 28}, {'#', 29}, {'#', 30}, {'#', 31}};
    char texto_27[4][501] = {"##sussurros##  \n-Olha so, parece que alguem esta sendo cauteloso - disse o cerqueireanista com tom de orgulho \n-Posso escolher lutar ou n?\n-Claro, a escolha eh sua:\nS-Acordar e lutar contra o monstro\nN-Voltar e deixar o monstro dormindo\nDigita a opcao: ", "-HAHA,SEGURA ESSA INTEGRAL TRIPLA ALUNO REPUGNANTE. \n-Aprendi isso com 14 anos seu otario.\n", "-NAAAAAOOOO.\n-Essa luta eh minha seu professor de uma figa\n", "-Muito bem aluno-disse o cerqueireanista-ha tempos que n encontrava um guerreiro tao bravo nessa masmorra.\n"};
	cadastrar_no(
        27,
        -1, 
        texto_27,
        5,
		nao_terminal,
        luta,
        5,
        opcoes_27,
        "./imagens/batalhaboss3.txt");
    
    opcao opcoes_31[1] = {{'#', 32}};
    char texto_31[1][501] = {"\n-Uma escada\n-Creio que seja sua saida daqui aluno, voce derrotou o grande TC Rock, esta livre\n\nO aluno nesse momento sai livre, subindo as escadas(mesmo as odiando devido ao trauma do IMEnso instituto), a luz começa a ficar mais clara...\n"};
	cadastrar_no(
        31,
        -1, 
        texto_31,
        0,
		nao_terminal,
        saida,
        1,
        opcoes_31,
        "./imagens/guianu.txt");
    
    char texto_32[1][501] = {"-Dormindo na aula aluno?\n-Nao senhor-Respondeu o aluno com a baba pendurada na boca\n-TORRADO,duvidas?\n\n###Obrigado por jogar o labirinto###"};
	cadastrar_no(
        32,
        -1, 
        texto_32,
        0,
		terminal,
        nao_complexo,
        0,
        NULL,
        "./imagens/personagem.txt");

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

void barra_superior(){
    // vida, xp, nivel, ataque, pedras
    //parte de cima
    printf("----------------------------- -----------------------------  -------  ---------  ---------\n");
    
    //vida
    printf("HP: ");
    int frac = vida*17/vida_MAX;
    for(int i=0; i<frac; i++){
        printf("\033[1;32;40m#\033[1;37;40m");
    }
    for(int i=0; i<17-frac; i++){
        printf(" ");
    }
    printf(" %3d/%3d ", vida, vida_MAX);

    //Xp
    printf("XP: ");
    frac = XP*17/100;
    for(int i=0; i<frac; i++){
        printf("\033[1;34;40m#\033[1;37;40m");
    }
    for(int i=0; i<17-frac; i++){
        printf(" ");
    }
    printf(" %3d/%3d  ", XP, 100);

    
    //outras coisas NIVEL ATAQUE PEDRAS
    printf("NIVEL:%1d  ATAQUE:%2d  PEDRAS:%2d", nivel, ataque, pedras);
    //parte de baixo que eh copia da de cima
    printf("\n----------------------------- -----------------------------  -------  ---------  ---------\n");
}

void barra_superior_luta(){
    //parte de cima
    printf("------------------------------ ---------          --------- ------------------------------\n");

    //vida
    printf("HP: ");
    int frac = vida*18/vida_MAX;
    for(int i=0; i<frac; i++){
        printf("\033[1;32;40m#\033[1;37;40m");
    }
    for(int i=0; i<18-frac; i++){
        printf(" ");
    }
    printf(" %3d/%3d ", vida, vida_MAX);

    printf("ATAQUE:%2d          ATAQUE:%2d ", ataque, ataque_inimigo);

    //vida_inimigo
    printf("HP: ");
    frac = vida_inimigo*18/vida_inimigo_MAX;
    for(int i=0; i<frac; i++){
        printf("\033[1;31;40m#\033[1;37;40m");
    }
    for(int i=0; i<18-frac; i++){
        printf(" ");
    }
    printf(" %3d/%3d", vida_inimigo, vida_inimigo_MAX);

    //parte de baixo copia da de cima
    printf("\n------------------------------ ---------          --------- ------------------------------\n");
}

void limpar(){
    system("cls");
}

void pausa(){
    system("pause");
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

void dano_vida_luta(){

}


