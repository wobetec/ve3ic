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
    ptr_atual = buscar_no(100);

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
    opcao opcoes_100[3] = {{'#', 101}};
    char texto_100[1][501] = {"-Servo: Acho que meu senhor esta certo, desse voce nao sai, nao com vida pelo menos.\n"};
	cadastrar_no(
        100,
        -1, 
        texto_100,
        0,
		nao_terminal,
        entrada,
        1,
        opcoes_100,
        "./imagens/ambientacao/corredor.txt");

    opcao opcoes_101[3] = {{'E', 102}, {'F', 115}, {'D', 108}};
    char texto_101[1][501] = {"-Voce: Uma trifurcacao, que caminho sera o melhor…? Sinto um bom chiero vindo da esquerda,\nsera que eh alguma armadilha? Vamos decidir isso logo.\nE-Esquerda F-Frente D-Direita\nDigite uma opcao: "};
	cadastrar_no(
        101,
        -1, 
        texto_101,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_101,
        "./imagens/ambientacao/trifurcacao.txt");

    opcao opcoes_102[5] = {{'L', 103}, {'V', 101}, {'#', 104}, {'#', 105}, {'#', 106}};
    char texto_102[4][501] = {"-Voce: O que eh aquilo? Parece um virus… mas como pode ser tao grande?\n-Voz misteriosa: Esta com medo? Deseja seguir em frente e lutar com ele?\nL-Lutar V-Voltar\nDigite uma opcao: ", "-Voce: Pode vir bicho esquisito. Nao eh uma gripezinha que vai me matar.\n-Virus: Voce acha que sou apenas um virus qualquer? Eu ja matei varios aventureiros\nigual a voce. Os humanos sao tao fracos, e, em se tratando do sistema imunologico  sao\nmais fracos ainda. Mua hahahahaha! :p\n", "-Voce: Pare de brincadeira seu virus.\n-Virus: Gosta de uma tosse? Entao vem pra cima!\n", "-Voce: Eh esse aqui nao aguentou meus socos. Talvez seja por que tomo vitamina C todos\nos dias kkkkk.\n-Servo: Nao seja tolo, seu burro. Isso foi apenas sorte\n"};
	cadastrar_no(
        102,
        -1, 
        texto_102,
        4,
		nao_terminal,
        luta,
        5,
        opcoes_102,
        "./imagens/andar3/virus.txt");

    opcao opcoes_106[1] = {{'#', 107}};
    char texto_106[1][501] = {"-Voce: Uma caixa de pratas! Vou estar rico quando sair daqui! Sao dez moedas.\n-Servo: Esse dinheiro soh pode estar amaldicoado, aluno idiota."};
	cadastrar_no(
        106,
        -1, 
        texto_106,
        0,
		nao_terminal,
        dialogo,
        1,
        opcoes_106,
        "");

    opcao opcoes_107[1] = {{'#', 101}};
    char texto_107[1][501] = {"-Senhor perdido: Pelas barbas de Odin, que dor de cabeça. Parece que dormi faz tanto tem-\npo, as agora nao me lembro de nada. Preciso dar uma caminhada para me lembrar dos fatos.\n-Voce(susurro): Que homem estranho."};
	cadastrar_no(
        107,
        -1, 
        texto_107,
        0,
		nao_terminal,
        dialogo,
        1,
        opcoes_107,
        "");

    opcao opcoes_108[4] = {{'#', 109}, {'#', 110}, {'#', 111}, {'#', 112}};
    char texto_108[4][501] = {"-Você: Eh um lobo com aura verde, so pode ser o Wolf-Champion.\n-Servo: Agora eh o fim da linha para sua historia de heroi, duvido que voce o derrote.\n", "-Voce: Indentifique-se lobo maldito!\n-Fenrir: Humano, acha que um mero humano pode me tratar assim? Eu sou Fenrir, sou o lendario\nFenrir, aquele que arrancou a mao de um deus, sou filho de Loki!\n", "-Voce: Acha que isso me assusta? Com ajuda do elmo de escamas de um dragao, com essa\nforca insana que recebi experimente meu soco melhorado. Haia!!!\n-Lobo: Minha mordida vai te dilacerar raca inferior.\n", "-Voce: -rapidamente se esquivou da tentativa da mordida, e logo desfere um golpe\ncerteiro-.\n-Lobo: Como isso aconteceu? Meu nivel eh muito superior. Hm, o que te espera a frente faz\nparte da minha colecao de trofeus. Essa batalha, nao se engane, isso foi apenas um\ndeslize da minha atencao.\n"};
	cadastrar_no(
        108,
        -1, 
        texto_108,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_108,
        "./imagens/andar2/lobo.txt");

    opcao opcoes_112[3] = {{'#', 113}, {'#', 114}, {'#', 115}};
    char texto_112[3][501] = {"-Voce: outra sala do tesouro, o que sera que tem aqui?\n", "-Voce: Um anel, o que eh esse anel sera?\n-Servo: E eu vou saber? Pare de falar sozinho seu humano burro.\n", "-Voz misteriosa: Esse eh um anel que emana poder magico, expandindo as capacidades de\nquem o equipa. Dizem que o preco a ser pago por tanto poder afeta diretamente a alma."};
	cadastrar_no(
        112,
        -1, 
        texto_112,
        3,
		nao_terminal,
        sala_item,
        3,
        opcoes_112,
        "");

    opcao opcoes_115[3] = {{'E', 116}, {'D', 123}, {'V', 101}};
    char texto_115[1][501] = {"-Voce: Uma bifurcacao, que caminho sera o melhor?\n-Servo: arggggg, voce me da nos nervos.\nF-Frente E-Esquerda\nDigite uma opcao: "};
	cadastrar_no(
        115,
        -1, 
        texto_115,
        0,
		nao_terminal,
        nao_complexo,
        3,
        opcoes_115,
        "./imagens/ambientacao/bifurcacao.txt");

    opcao opcoes_116[9] = {{'A', 117}, {'B', 118}, {'C', 119}, {'D', 120}, {'V', 115}, {'#', 116}, {'#', 121}, {'#', 116}, {'#', 116}};
    char texto_116[5][501] = {"-Enigma: Vejo que chegou ate esse andar, porem lamento te informar que a partir daqui sua\naventura so vai ficar mais e mais dificil. Sem mais delongas, vamos ao enigma:\nTres resistores de 33 ohms sao ligados em paralelo a um cofre. Qual eh o nome do filme?\nA-Titanic B-11 ohms e um segredo C-Missao Impossivel D-Jogos Vorazes\nV-Voltar\nDigite uma opcao: ", "-Enigma: Acho que esse eh aquele do barco.\n", "-Enigma: hahahaha, essa foi boa, nao?\n", "-Enigma: Essa dai passou longe, aluno.\n", "-Enigma: Os resistores sao assassinos agora?\n"};
	cadastrar_no(
        116,
        -1, 
        texto_116,
        5,
		nao_terminal,
        enigma,
        5,
        opcoes_116,
        "./imagens/ambientacao/enigma.txt");

    opcao opcoes_121[1] = {{'#', 122}};
    char texto_121[1][501] = {"-Taberneiro sagaz: Bem-vindo a Taberna! Hoje temos um otimo ensopado de peixe e uma\nsalada especial, feita com ingredientes coletados no bosque imperial, cultivado por\naldeoes elficos.-Voce: Nossa, agora meu poder parece estar recuperado!"};
	cadastrar_no(
        121,
        -1, 
        texto_121,
        0,
		nao_terminal,
        rancho,
        1,
        opcoes_121,
        "./imagens/ambientacao/taverna.txt");

    opcao opcoes_122[1] = {{'#', 115}};
    char texto_122[1][501] = {"-Voce: Mais uma pedra, essa ja eh a terceira, acho que vou sair daqui com muitas."};
	cadastrar_no(
        122,
        -1, 
        texto_122,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_122,
        "./imagens/labirinto2.txt");

    opcao opcoes_123[4] = {{'T', 4124}, {'V', 115}, {'#', 126}, {'#', 123}};
    char texto_123[3][501] = {"-Voce: Outra porta, vamos ver...\nT-Tentar abrir V-Voltar\nDigite uma opcao: ", "-Porta: Espero que goste do inferno.\n", "-Porta: Se prepare melhor antes de embarcar na Highway to Hell"};
	cadastrar_no(
        123,
        -1, 
        texto_123,
        3,
		nao_terminal,
        fechadura,
        2,
        opcoes_123,
        "./imagens/ambientacao/fechadura.txt");

    opcao opcoes_126[4] = {{'#', 127}, {'#', 128}, {'#', 129}, {'#', 130}};
    char texto_126[4][501] = {"-Voce: Pelas barbas de Odin, que calor eh esse? Parece que estamos chegando no inferno.\n-Servo: Voce continua falando demais, melhor voce se proteger, olha aquela figura com\ncara de diabo te esperando.\n", "\n-Demonio: Morraaaaa!\n-Voce: Calma la, mal cheguei e o anfitriao me recebe assim? Saudades de quando as pessoas\nainda possuiam respeito pelas outras.\n-Demonio: Eu vivo nesse labirinto a anos, e nunca deixei nem um humano passar. Sou mais\nantigo que qualquer ser que voce viu ate agora. Meus poderes superam tudo, meu corpo eh\ntao poderoso e resistente que nem uma armadura chega a ter a mesma resistencia. Um mero\nhumano nao chega a nem fazer cocegas em mim. Isso nunca vai acontecer.\n", "-Voce: Aaaaaaaaaaaaaahhhhh! Se liga nesse anel! O anel expansivo vai me dar o poder\nmagico que preciso para acabar com esse monstro.\n-Demonio: Ta legal, esse anel nao fara nem cocegas em mim, ja te avisei.\n-Voce: -voce colocou o anel no dedo e apontou para o demonio-.\n-Demonio: Nao pode ser, esse anel esta encantado com propriedades sagradas! Saia ja\ndaqui. Seu verme, quem te deu isso?!\n-Voce: Entao isso te afeta, esse eh o seu fim!\n", "-Voz misteriosa: Como foi possivel!? O Demonio foi derrotado? Isso nao vai sair barato,\nos inimigos a frente vao te dar uma licao… Soh espere...\n-Voce: Esse bicho foi dificil de matar, mas por pouco consegui. E essa espada que ele\nusava... parecia interessante.\n"};
	cadastrar_no(
        126,
        -1, 
        texto_126,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_126,
        "./imagens/andar3/batalhaboss3.txt");
    
    opcao opcoes_130[3] = {{'#', 131}, {'#', 132}, {'#', 133}};
    char texto_130[3][501] = {"-Voce: Caramba, ele deixou cair a espada.\n-Voz misteriosa: Vamos-la, vai deixa-la para tras?\n", "-Voce: Ele eh muit quente.\n-Voz misteriosa: Obvio, esse eh o pau de fogo, a arma mais poderosa do submundo. Nao sei\nnem explicar como voce, um mero mortal conseguiu sobreviver ao implacavel poder dela\n", "-Voce: Agora ninguem consegue me parar.\n"};
	cadastrar_no(
        130,
        -1, 
        texto_130,
        3,
		nao_terminal,
        sala_item,
        3,
        opcoes_130,
        "./imagens/andar3/paudefogo.txt");

    opcao opcoes_133[1] = {{'E', 3}};
    char texto_133[1][501] = {"-Servo: Nao acredito que voce ainda esta vivo, ser repugnante.Vamos suba logo, voce tem que morrer."};
	cadastrar_no(
        133,
        -1, 
        texto_133,
        0,
		nao_terminal,
        saida,
        1,
        opcoes_133,
        "./imagens/ambientacao/escada.txt");



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
