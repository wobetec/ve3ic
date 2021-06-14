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
int fechadura_boss = 0;

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
    imagem("./imagens/ambientacao/bemvindo.txt");
    char nome_jogador[100];
    strcpy(nome_jogador, ler_nome_jogador(NOME_ARQUIVO_ENTRADA));
    printf("Bem-vindo a masmorra da casa do trem, %s!\n", nome_jogador);
    fprintf(arquivo_saida, "Bem-vindo a masmorra da casa do trem, %s!\n", nome_jogador);
    pausa();

    salas_bloqueadas[0]=0;

    //carregar o no zero
    ptr_atual = buscar_no(200);

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
                                fechadura_boss++;
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
                                fechadura_boss++;
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
                                contador  = 0;
                                atualizar_vida(1, 20);

                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_esperar(&indice_proximo_no);
                                break;
                            
                            case entrada:
                                contador = 0;
                                fechadura_boss = 0;
                                
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
                                contador = 0;
                                limpar();
                                barra_superior();
                                printar_imagem();
                                printar_opcao(&opcao, &indice_proximo_no);

                                if(indice_proximo_no >= 10000){
                                    int n = indice_proximo_no/10000;
                                    if(pedras == n){
                                        indice_proximo_no = indice_proximo_no%1000;
                                    }else{
                                        indice_proximo_no = indice_proximo_no%1000 + 1;
                                    }
                                }else{
                                    if(indice_proximo_no/1000 == fechadura_boss && fechadura_boss != 0){
                                        indice_proximo_no = indice_proximo_no %1000;
                                        fechadura_boss = 0;
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
                imagem("./imagens/ambientacao/caminho_bloqueado.txt");
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
    opcao opcoes_200[1] = {{'#', 201}};
    char texto_200[1][501] = {"-Servo: Eh, eu ate poderia te deixar seguir nesse ultimo andar sozinho... mas quem iria te\nenterrar? Precisa ter um inutil como eu ao seu lado para fazer esse servico sujo..."};
	cadastrar_no(
        200,
        -1, 
        texto_200,
        0,
		nao_terminal,
        entrada,
        1,
        opcoes_200,
        "./imagens/ambientacao/corredor.txt");
    
    opcao opcoes_201[3] = {{'E', 202}, {'D', 210},{'F',211}};
    char texto_201[1][501] ={"-Voce: Uma nova trifurcacao? Sera essa a ultima?\n-Servo: Nao importa aluno, pare de fazer perguntas retoricas o tempo todo. Escolha que\ncaminho deseja seguir:\nE-Esquerda D-Direita F-Frente\nDigite uma opcao:"};
	cadastrar_no(
        201,
        -1, 
        texto_201,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_201,
        "./imagens/ambientacao/trifurcacao.txt");
    
    opcao opcoes_202[9] = {{'A', 203}, {'B', 204}, {'C', 205},{'E',206},{'V',201}, {'#',202},{'#',207},{'#',202},{'#',202}};
    char texto_202[1][501] = {"-Enigma: Eh aluno. Esse eh o ultimo enigma que tera que resolver para sobreviver nessa\ntorre. Talvez voce se considere um genio, mas esse enigma foi escrito a milhares de anos \natras pelo criador desse jogo, Duvido que conseguira acertar.\nint x=10;\nint y=10;\nreturn x+y Qual o nome do filme?\nA-Missao impossivel B-O Codigo da Vinte C-Indiana Jones D-Dez eh pouco, vinte eh demais \nV-Voltar\nDigite uma opcao: "};
	cadastrar_no(
        202,
        -1, 
        texto_202,
        0,
		nao_terminal,
        enigma,
        5,
        opcoes_202,
        "./imagens/ambientacao/enigma.txt");

    opcao opcoes_207[3] = {{'#', 208}, {'#', 209},{'#',201}};
    char texto_207[3][501] = {"-Voce: Parece que essas pedras tem algo de importante, eu sinto isso", "-Servo: Vamos logo aluno, soh pegue essa pedra, nao precisa falar muito.", "-Voce: Agora que eu percebi, essa daqui parece ter algum numero, parece um V.\n-Voz misteriosa: Voce eh muito lento aluno."};
	cadastrar_no(
        207,
        -1, 
        texto_207,
        3,
		nao_terminal,
        pedra,
        3,
        opcoes_207,
        "./imagens/ambientacao/gema.txt");

    opcao opcoes_210[1] = {{'#',201}};
    char texto_210[1][501] = {"-Voce: Que cheiro bom, de onde estah vindo?\n-Taberneiro: Bem-vindo a Taberna! Recupere suas forcas com alguns petiscos e um bom vinho!\n"};
	cadastrar_no(
        210,
        -1, 
        texto_210,
        0,
		nao_terminal,
        rancho,
        1,
        opcoes_210,
        "./imagens/ambientacao/taverna.txt");
    
    opcao opcoes_211[3] = {{'E', 212}, {'D', 219}, {'V', 201}};
    char texto_211[1][501] = {"-Servo: Como voce eh insuportavel, eu vou falar! Se voce fizer a escolha certa, essa sera\na ultima bifurcacao desse jogo. Escolha que caminho deseja seguir:\nE-Esquerda D-Direita V-Voltar\nDigite uma opcao:"};
	cadastrar_no(
        211,
        -1, 
        texto_211,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_211,
        "./imagens/ambientacao/bifurcacao.txt");

    opcao opcoes_212[5] = {{'A', 213},{'B', 211}, {'#', 214}, {'#', 215},{'#', 216}};
    char texto_212[4][501] = {"-Voz misteriosa: Sera esse o ultimo monstro desse andar? Tem porte para isso? the Gleam\nEyes eh um monstro demoniaco parecendo um touro,com musculos tao tensos quanto as cordas\nondulando sua forma onipotente. sua pele tem um tom azul profundo e seus olhos tem essa\nchama ardente que envolve essa sala. Gostou dessa descricao? pena que sua morte nao sera\ntao bela assim. Para prosseguir tera que enfrenta-lo. Deseja lutar com ele?\nA-Lutar B-Voltar\nDigite uma opcao: ", "-Voce(gritando): Eu vou mata-lo! nem que essa seja a ultima coisa que eu faca!\n","-Touro: \"Aaahhh!!\"\n-Voce: Seu ser malditoooooo! Eu vou te matar!.\n","-Voz misteriosa: Vai morrer agora nos acrescimos do segundo tempo? no ultimo andar?\n"};
	cadastrar_no(
        212,
        -1, 
        texto_212,
        4,
		nao_terminal,
        luta,
        5,
        opcoes_212,
        "./imagens/andar5/touro.txt");

    opcao opcoes_216[3] = {{'#', 217}, {'#', 218}, {'#', 211}};
    char texto_216[3][501] = {"-Servo: Parabens! Ate que voce nao eh inutil! Voce coletou a ponta do chifre desse touro\ncomo recompensa por essa vitoria.\n", "-Voz misteriosa: Antes mesmo de que eu fosse criado, os grandes deuses utilizavam chifres\ndesses touros mitologicos para conjurar legioes de guerreiros, quem sabe nao lhe possa ser\nutil.\n", "-Voce: Isso quer dizer que agora sou um deus?\n-Servo: Vamos logo com isso, aluno, quero te enterrar logo.\n"};
	cadastrar_no(
        216,
        -1, 
        texto_216,
        3,
		nao_terminal,
        sala_item,
        3,
        opcoes_216,
        "");

    opcao opcoes_219[4] = {{'A', 4220}, {'V', 211}, {'#', 221}, {'#', 211}};
    char texto_219[3][501] = {"-Voce: Nossa! Que hall legal! Nunca vi um corredor tao longo, para onde sera que ele leva?\n Uma porta sem fechadura? Mas como posso abri-la?\nA-Tentar abrir V-Voltar\nDigite uma opcao: ", "-Porta: Voce possui todas as joias que pertencem a poderosa deusa, idealizadora desta\ntorre, pode passar!\n", "-Porta: Acho que esta faltando alguma joia, ache-as e podera passar.\n"};
	cadastrar_no(
        219,
        -1, 
        texto_219,
        3,
		nao_terminal,
        fechadura,
        3,
        opcoes_219,
        "./imagens/ambientacao/porta");
    
    opcao opcoes_221[4] = {{'#', 222}, {'#', 223}, {'#', 224}, {'#', 225}};
    char texto_221[4][501] = {"Gabiqueen:Uau, pensei que nunca me conheceria. Que privilegio aluno! Agora voce sabe que\nnao foi um Deus que criou esse mundo,mas sim uma deusa.Como me diverti com voces colocando\nsuas vidas em risco apenas porque eu quis assim. Vamos direto ao ponto...Te derrotarei de\numa vez por todas com o poder da computacao!\n-Voce: Lutei bastante para chegar ate aqui! Nao vou deixar uma deusa sequer me derrotar!.\n", "-Voce: Vou honrar as joias que coletei!\n-Gabiqueen: Mais um que se acha diferente. Todas as joias sempre foram minhas, aluno!\n", "\n-Voce: NInguem pode me parar!.\n-Gabiqueen: Ninguem nunca passou por mim!\n","-Servo: Eh, voce nao era qualquer um...\n"};
	cadastrar_no(
        221,
        -1, 
        texto_221,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_221,
        "");
    
    opcao opcoes_225[1] = {{'#', 226}};
    char texto_225[1][501] = {"\nVoz misteriosa: Primeiramente, morra servo imundo, nao consegue nem matar um aluno.\n-Servo: Naaaaaaao - e morreu.\n-Voz misteriosa: Em segundo lugar, parabens aluno, nunca vi ninguem derrotar minha\ncriadora, nem sei o que acontecera comigo, mas sei que voce esta livre, pode atravessar o portal para seu mundo de novo.\n-Voce: Nunca vou me esquecer do que aconteceu aqui\n-assim o aluno corre em direcao ao portal-\n"};
	cadastrar_no(
        225,
        -1, 
        texto_225,
        0,
		nao_terminal,
        nao_complexo,
        1,
        opcoes_225,
        "./imagens/ambientacao/cenariofinal.txt");

    opcao opcoes_226[1] = {{'#', 227}};
    char texto_226[1][501] = {"Cap: Dormindo em aula, aluno?"};
	cadastrar_no(
        226,
        -1, 
        texto_226,
        0,
		nao_terminal,
        saida,
        1,
        opcoes_226,
        "");
    
    char texto_227[1][501] = {"Duvidas?"};
	cadastrar_no(
        227,
        -1, 
        texto_227,
        0,
		terminal,
        nao_complexo,
        0,
        NULL,
        "");
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
        
        printf("\nERRO 05: NO NAO ENCONTRADO NA LISTA. %d", indice);
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
