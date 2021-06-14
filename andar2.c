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
    ptr_atual = buscar_no(50);

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
    opcao opcoes_50[1] = {{'#', 51}};
    char texto_50[1][501] = {"-Servo: Vamos ver como sobrevive a esse andar, voce nao eh pario para esses monstros,\nhahaha.\n"};
	cadastrar_no(
        50,
        -1, 
        texto_50,
        0,
		nao_terminal,
        entrada,
        1,
        opcoes_50,
        "./imagens/ambientacao/corredor.txt");
    
    opcao opcoes_51[3] = {{'E', 52}, {'F', 65}, {'D', 58}};
    char texto_51[1][501] = {"-Voce: Uma trifurcacao, que caminho sera o melhor? Algo me diz que eh bom ir para a\ndireita, mas qual sera que eh melhor?\nE-Esquerda F-Frente D-Direita\nDigite uma opcao: "};
	cadastrar_no(
        51,
        -1, 
        texto_51,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_51,
        "./imagens/ambientacao/trifurcacao.txt");
    
    opcao opcoes_52[5] = {{'L', 53}, {'V', 51}, {'#', 54}, {'#', 55}, {'#', 56}};
    char texto_52[4][501] = {"-Servo: Veja se nao temos um conhecido se aproximando, quanto tempo que nao o encontro. \n-Voce: Esse animal parece ser fraco, mas sinto uma aura magica.\nL-Lutar V-Voltar\nDigite uma opcao: ", "-Morcego: Guerreiro, quer tentar passar? So por cima do meu cadaver.\n-Voce: Isso ai ser facil e rapido!\n", "-Morcego: Hahahaha, vou te dar o prazer de sentir um pouco das minhas habilidades. Golpe\nVida Sanguessuga!\n-Voce: Arg! Minha vitalidade? Morcego maldito, o que voce estah fazendo com minha vida?\nPelos poderes concedido por esse elmo, tome essa capacetada no meio dessa tua boca\nvenenosa!\n", "-Morcego: Maldito humano, dessa vez vou fugir, mas saiba que isso eh soh por hoje.\n"};
	cadastrar_no(
        52,
        -1, 
        texto_52,
        4,
		nao_terminal,
        luta,
        5,
        opcoes_52,
        "./imagens/batalha3.txt");
    
    opcao opcoes_56[1] = {{'#', 57}};
    char texto_56[1][501] = {"-Voce: Mas o que e isso? Um caminho sem saida? O que eh esse muro engracado? E o que eh\nessa placa escrita... Beco Diagonal??\n-Servo: Parece que voce nasceu ontem? Voce eh muito burro. Esse eh um muro magico, deixe\nque eu abro ele."};
	cadastrar_no(
        56,
        -1, 
        texto_56,
        0,
		nao_terminal,
        dialogo,
        1,
        opcoes_56,
        "");

    opcao opcoes_57[1] = {{'#', 51}};
    char texto_57[1][501] = {"-Voce: Hmmm. Uma pedra esquisita com um encaixe atras e um saco com Essencia Solar? Isso\nestava escondido atras desses tijolos!? Nao consigo nem acreditar que itens raros desses\nestavam escondidos nesse buraco. Vou guardar.\n-Voz misteriosa: Eu sabia que tinhamos um colecionador de pedras."};
	cadastrar_no(
        57,
        -1, 
        texto_57,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_57,
        "./imagens/ambientacao/gema.txt");
    
    opcao opcoes_58[5] = {{'L', 59}, {'V', 51}, {'#', 60}, {'#', 61}, {'#', 62}};
    char texto_58[4][501] = {"-Voce(sussurando): Um lobo monstruoso se aproxima, melhor sacar a minha arma para me\ndefender, porem posso fugir, o que fazer??\n-Servo(sussurando): Fugir e claro, voce soh foge. Desde que virei seu guia voce so sabe\nfugir dos teus problemas.\nL-Lutar V-Voltar\nDigite uma opcao: ", "-Lobo: Auuuuu! Humano maldito, voce nao eh pareo para o meu poder! Sua morte sera rapida.\n-Voce: Hamm! Um lobo falante, isso eh novidade vou te queimar ate morrer!\n", "-Lobo: Auuuuuuu! Nao vou deixar voce passar tao facil. Experimente um pouco das minhas\ngarras afiadas!!!\n-Voce: Deixa de ser bobo, seu lobo infeliz! Vou te calar agora mesmo.\n", "-Lobo: Humano insolente, auuuuuu! Sua escoria, vera o que acontecera quando meus irmaos\nchegarem. Auuuuuu!\n-Voce: Pode deixar eles virem."};
	cadastrar_no(
        58,
        -1, 
        texto_58,
        4,
		nao_terminal,
        luta,
        5,
        opcoes_58,
        "./imagens/andar2/lobo.txt");
    
    opcao opcoes_62[3] = {{'#', 63}, {'#', 64}, {'#', 51}};
    char texto_62[3][501] = {"-Voce: Uau, uma armadura incrivel, o que eh isso que estah escrito na parede??\n", "-Servo: Criada com a fibra varonil de um nobre Engenheiro Militar, a Couraca do RF e mais\nresistente que placas de aco e mais leve que um manto de seda. Por isso, muitas vezes e\ncomparada com as melhores armaduras do mundo.\n-Servo: guerreiro tolo, nao sabe nem ler em elfico.?\n", "-Voce: Uau, ela eh realmente leve.?"};
	cadastrar_no(
        62,
        -1, 
        texto_62,
        3,
		nao_terminal,
        sala_item,
        3,
        opcoes_62,
        "");
    
    opcao opcoes_65[3] = {{'E', 72}, {'D', 66}, {'V', 51}};
    char texto_65[1][501] = {"-Voce: E ca estamos em mais uma decisao de caminho.\nE-Esquerda D-Direita V-Voltar\nDigite uma opcao: "};
	cadastrar_no(
        65,
        -1, 
        texto_65,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_65,
        "./imagens/ambientacao/bifurcacao.txt");
    
    opcao opcoes_66[9] = {{'A', 67}, {'B', 68}, {'C', 69}, {'D', 70}, {'V', 65}, {'#', 66}, {'#', 71}, {'#', 66}, {'#', 66}};
    char texto_66[5][501] = {"-Enigma: Mais um viajante perdido, vamos ver se alem de conseguir sobreviver ate aqui\nvoce eh esperto: dy/dx = y\nEntao qual funcao representa y?\nA-logx B-e^x  C-x^x D-lnx\nV-Voltar\nDigite uma opcao: ", "-Enigma: Serio isso aluno? nao estudou log no cursinho?\n", "-Enigma: Bom, quase um Euler ja.\n", "-Enigma: Ai voce esta de brincadeira.\n", "-Enigma: Passou muito longe, aluno.\n"};
	cadastrar_no(
        66,
        -1, 
        texto_66,
        5,
		nao_terminal,
        enigma,
        5,
        opcoes_66,
        "./imagens/ambientacao/enigma.txt");
    
    opcao opcoes_71[1] = {{'#', 65}};
    char texto_71[1][501] = {"-Voce: Um guerreiro morto, sera que posso encontrar algo de util na mochila dele? \n-Servo: Hum, um humano morto no caminho,  isso e normal? \n-Voce: Essa pocao vermelha deve ser suficiente para recuperar um pouco da minha vida.\n"};
	cadastrar_no(
        71,
        -1, 
        texto_71,
        0,
		nao_terminal,
        dialogo,
        1,
        opcoes_71,
        "");
    
    opcao opcoes_72[4] = {{'T', 4073}, {'V', 65}, {'#', 75}, {'#', 72}};
    char texto_72[3][501] = {"-Voce: Essa porta emana algo diferete\nT-Tentar abrir V-Voltar\nDigite uma opcao: ", "-Porta: Boa sorte, mortal.\n", "-Porta: Ha perigos inimaginaveis a frente. Acho bom voce se preparar melhor.\n"};
	cadastrar_no(
        72,
        -1, 
        texto_72,
        3,
		nao_terminal,
        fechadura,
        2,
        opcoes_72,
        "./imagens/ambientacao/fechadura.txt");

    opcao opcoes_75[4] = {{'#', 76}, {'#', 77}, {'#', 78}, {'#', 79},};
    char texto_75[4][501] = {"-Voce: O que isso caindo do teto?\n", "-Monstro: Kikikkikiki. Um mero mortal entrando no\nmeu territorio?! Eu sou o Vampirozao, o vampiro do sangue de ferro, Sangue Quente e\nSangue Frio. Vivo nesse teto faz mais de dois milenios, nunca deixei um inseto passar.\n-Voce: Prazer, o heroi que vai te derrator! Saia desse teto seu chupa sangue maldito.\n", "-Vampirozao: Inseto maldito. Esse teu sangue vai ser meu lanchinho, experimente meu golpe\nperpendicular!!!\n-Voce: Defesa RETA! - Bradou colocando seus bracos, no formato de uma reta, frente ao\ncorpo.\n", "-Voce: eureca - rapidamente retirou do bolso a essencia solar que havia encontrado.\n-Vampirozao: Aaaahhhhh! Inseto infeliz, voce eh soh um mero mortal. O que eh isso?\n-Voce: Isso eh um pouquinho do sol para essa sua cara palida.\n-Vampirozao: Naaaaaaooooo.-Servo: Quanta sorte voce teve mestre, mas se depender dela\nvai morrer cedo."};
	cadastrar_no(
        75,
        -1, 
        texto_75,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_75,
        "./imagens/andar2/zao.txt");
    
    opcao opcoes_79[1] = {{'#', 100}};
    char texto_79[1][501] = {"-Voz misteriosa: Conseguiu sobreviver a mais esse andar, quero ver quantos mais voce\nconseguir, mas aposto que nao passa do terceiro.\n"};
	cadastrar_no(
        79,
        -1, 
        texto_79,
        0,
		nao_terminal,
        saida,
        1,
        opcoes_79,
        "./imagens/ambientacao/escada.txt");



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
