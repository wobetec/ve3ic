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
    passar, entrada, sala_item, pedra, saida, fechadura, enigma, luta, rancho, dialogo, caminhos,
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
    switch(ptr_atual->tipo_especifico){
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
        case caminhos:
            return "caminhos";
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
            fprintf(arquivo_saida, "%c vai para no de indice: %3d\n", ptr_atual->opcoes[j].opcao_selecionada, ptr_atual->opcoes[j].indice_proximo_no);
        }
        fprintf(arquivo_saida, "##########################################################################################\n");
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
            cadastrar_no(indice+2, codigo+2, texto, 2, tipo, passar, 3, opcoes, endereco_imagem);
            break;

        case pedra:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, endereco_imagem);
            break;

        case saida:
            cadastrar_no(indice, codigo, texto, 0, tipo, passar, n_opcoes, opcoes, endereco_imagem);
            break;

        case fechadura:
            cadastrar_no(indice, codigo, texto, 0, tipo, caminhos, n_opcoes, opcoes, endereco_imagem);
            cadastrar_no(indice+1, codigo+1, texto, 1, tipo, passar, n_opcoes+1, opcoes, endereco_imagem);
            cadastrar_no(indice+2, codigo+1, texto, 2, tipo, passar, n_opcoes+2, opcoes, endereco_imagem);
            break;

        case enigma:
            cadastrar_no(indice, codigo, texto, 0, tipo, caminhos, n_opcoes, opcoes, endereco_imagem);

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
                cadastrar_no(indice, codigo, texto, 0, tipo, caminhos, 2, opcoes, endereco_imagem);
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

        case caminhos:

            ptr->indice = indice;
            
            //gerando o código daquele nó
            if(code == -1){
                make += (indice*10000);
            }else{
                make += ((indice-(codigo%100))*10000 + codigo);
            }
            ptr->code = make;

            strcpy(ptr->texto, texto[n_textos]);
            ptr->tipo_especifico = caminhos;
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

//##################################ANDAR 1#####################################//
    opcao opcoes_0[1] = {{'#', 1}};
    char texto_0[1][501] = {"Quinta-feira pela manha, aula de AlgLin que parece nao ter fim, e la esta o Aluno do \nIMEnso instituto, tentando se concentrar na aulinha enquanto nao para de pensar no Empire\nof Dungeons, um jogo que encontrou na internet recentemente e nao ve a hora de comprar.\nEssa aula esta realmente chata e o aluno esta muito concentrado em seus pensamentos,\nimagiando cada detalhe do jogo, quando derrepente um clarao.\n"};
    cadastrar_no(
        0,
        -1, 
        texto_0,
        0,
		raiz,
        entrada,
        1,
        opcoes_0,
        "./imagens/ambientacao/sala.txt");

    opcao opcoes_1[3] = {{'#', 2}, {'#', 3}, {'#', 4}};
    char texto_1[3][501] = {"-Voz misteriosa: Ola, quem vos fala eh o proprio jogo. Por enquanto, voces nao\nsaberao quem sou eu... na verdade, poucos descobrirao. O que esta em jogo agora eh a sua\nvida.\n", "Se conseguir chegar ao ultimo andar da torre de 5 andares que se apresenta adiante,\nresolvendo todos os enigmas necessarios, coletando todos os itens, tornando-se mais forte\ne derrotando os BOSSs de cada andar, saira vivo.\n", "Caso contrario, voce ja sabe. Por que estou fazendo isso? pura diversao. No mais, boa\nsorte!\n"};
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
    char texto_4[1][501] = {"-Voce: Uma bifurcacao, por onde devo ir? \nE-Esquerda D-Direita \nDigite uma opcao: "};
	cadastrar_no(
        4,
        -1, 
        texto_4,
        0,
		nao_terminal,
        caminhos,
        2,
        opcoes_4,
        "./imagens/ambientacao/bifurcacao.txt");

    opcao opcoes_5[4] = {{'#', 6}, {'#', 7}, {'#', 8}, {'#', 9}};
    char texto_5[4][501] = {"-Voce: O que eh aquilo, parece um goblim, ele esta dormindo.\n-Servo: Errado Aluno, nao esta mais.\n","-Goblim: lara ratu peri.\n-Voce: Seja la o que for, eu quem vou ganhar, sou treinado na arte das integrais.\n","-Goblim: baru sate lorem.\n-Voce: morraaaaa!\n", "-Servo: estou impressionado de que voce tenha saido dessa vivo, a maioria morre antes de\nvirar a esquerda.\n-Voz misteriosa: Nao encoraje esse quadrupede, esse eh soh o principio das dores.\n"};
	cadastrar_no(
        5,
        -1, 
        texto_5,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_5,
        "./imagens/andar1/goblim.txt");
    
    opcao opcoes_9[1] = {{'#', 4}};
    char texto_9[1][501] = {"-Voce: Olha, parece so uma pedra, devo pegar?\n-Servo: Nao estou aqui para ser sua baba, faca o que quiser\n-Voce: vou levar comigo.\n-Voz misteriosa: Veja se nao temos um colecionador de pedras aqui, vai mesmo guardar essa\npedra?\n-Voce: Vai que preciso jogar em alguem\n-Voz misteriosa: ou em algo."};
	cadastrar_no(
        9,
        -1, 
        texto_9,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_9,
        "./imagens/ambientacao/gema.txt");

    opcao opcoes_10[4] = {{'E', 11}, {'F', 24}, {'D', 18}, {'V', 4}};
    char texto_10[1][501] = {"-Voce: 3 caminhos agora, estou comecando a me perder aqui.\n-Servo: Hehe, soh continue andando aluno, quanto mais cedo voce morrer, mais cedo estou\nlivre.\nE-Esquerda F-Frente D-Direita V-Voltar \nDigite uma opcao: "};
	cadastrar_no(
        10,
        -1, 
        texto_10,
        0,
		nao_terminal,
        caminhos,
        4,
        opcoes_10,
        "./imagens/ambientacao/trifurcacao.txt");
    
    opcao opcoes_11[5] = {{'L', 12}, {'V', 10}, {'#', 13}, {'#', 14}, {'#', 15}};
    char texto_11[4][501] = {"-Voce(sussurando): Aquilo eh um fantasma?\n-Servo(sussurando): Acorde ele e descubra.\n L-Lutar V-Voltar\nDigite uma opcao: ", "-Servo: agora me diga como acertar um fantasma?\n-Voce: facil, eles sao frageis contra raios, e basta gerar um pouco de atrito que consigo\nproduzir faiscas.\n", "-Fantasma: ihhhhhhhhh.\n-Voce: Morra denovo seu infame.\n", "-Voz misteriosa: Vai ter que fazer muito melhor que isso se quiser sair vivo desse lugar.\n"};
	cadastrar_no(
        11,
        -1, 
        texto_11,
        4,
		nao_terminal,
        luta,
        5,
        opcoes_11,
        "./imagens/andar1/fantasma.txt");
    
    opcao opcoes_15[3] = {{'#', 16}, {'#', 17}, {'#', 10}};
    char texto_15[3][501] = {"-Voce: Uau, que sala incrivel, deve ter alguma armadilha aqui, parece aquelas salas do\ntesouro.\n-Servo: Cala a boca aluno, so joga, voce fala demais.\n-Voce: Olha, tem mesmo um tesouro ali, o que eh aquilo?\n", "-Voz misteriosa: O elmo de Richard Franco, forjado com escama de dragoes nas profundezas\nmasmorra do trem. Eh um objeto indispensavel para derrotar o TC Rock.\n", "-Voce: parece pesado, mas eh muito legal.\n"};
	cadastrar_no(
        15,
        -1, 
        texto_15,
        3,
		nao_terminal,
        sala_item,
        3,
        opcoes_15,
        "./imagens/ambientacao/tesouro.txt");
    
    opcao opcoes_18[9] = {{'A', 19}, {'B', 20}, {'C', 21}, {'D', 22}, {'V', 10}, {'#', 18}, {'#', 23}, {'#', 18}, {'#', 18}};
    char texto_18[5][501] = {"-Enigma: Alto la, sou o Enigma, o primeiro e unico, assim como minha prima distante,\nEsfinge, voce tem que responder para passar: \nQual destes queima azul?\n A-Na B-Cu C-Mg D-K\nV-Voltar \nDigite uma opcao: ", "Qual a ideia, aluno?\n", "Ate que voce sabe alguma coisa.\n", "Soh pode ser brincadeira, neh?\n", "kkkkkkkkkkkk\n"};
	cadastrar_no(
        18,
        -1, 
        texto_18,
        5,
		nao_terminal,
        enigma,
        5,
        opcoes_18,
        "./imagens/ambientacao/enigma.txt");
    
    opcao opcoes_23[1] = {{'#', 10}};
    char texto_23[1][501] = {"-Senhor perdido: Alguem finalmente me libertou, estou preso nessa sala dos enigmas desde\nque comecei a sonhar.\n"};
	cadastrar_no(
        23,
        -1, 
        texto_23,
        0,
		nao_terminal,
        dialogo,
        1,
        opcoes_23,
        "./imagens/ambientacao/velho.txt");
    
    opcao opcoes_24[4] = {{'T', 4025}, {'V', 10}, {'#', 27}, {'#', 24}};
    char texto_24[3][501] = {"-Voce: que porta maneira.\nT-Tentar abrir V-Voltar\n", "-Porta: Eh por sua conta e risco aluno, pode passar.\n", "-Porta: Parece que voce ainda nao conseguiu um elmo, nao posso deixar voce passar. Apesar\nde querer que voce morra logo o show tem que durar mais que 3s.\n"};
	cadastrar_no(
        24,
        -1, 
        texto_24,
        3,
		nao_terminal,
        fechadura,
        2,
        opcoes_24,
        "./imagens/ambientacao/fechadura.txt");
    
    opcao opcoes_27[4] = {{'#', 28}, {'#', 29}, {'#', 30}, {'#', 31}};
    char texto_27[4][501] = {"-TC: haha, veja se nao temos uma criaturinha aqui.\n-Voce: Eh uma pedra?\n-Servo: Voce nao devia ter dito isso.\n-TC: Ninguem me chama de pedraaaa.\n", "-Voce: Entao eh para isso que serve o elmo.\n-TC: Voce nao vai sair dessa vivo seu aluno incopetente, ninguem sobrevive a minha rajada\ndiferencial.\n", "-Voce: Eu tenho o poder da integral comigo.\n-TC: grhhhhh.\n", "-Voz misteriosa: Veja soh se n temos o primeiro guerreiro nos ultimos 229 anos que consegue\nderrotar o TC Rock.\n"};
	cadastrar_no(
        27,
        -1, 
        texto_27,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_27,
        "./imagens/andar1/tcrock.txt");
    
    opcao opcoes_31[1] = {{'#', 50},};
    char texto_31[1][501] = {"-Voz misteriosa: Vamos, suba a escada, quero ver voce ser trucidado pelos proximos\ndesafios. E voce, servo imundo, esta fazendo seu trabalho de forma errada, o infeliz\nainda esta vivo, esta ajudando ele por acaso?\n-Servo: Nao, senhor, estou seguindo suas\nordens.\n"};
	cadastrar_no(
        31,
        -1, 
        texto_31,
        0,
		nao_terminal,
        saida,
        1,
        opcoes_31,
        "./imagens/ambientacao/escada.txt");

//##################################ANDAR 2#####################################//

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
        caminhos,
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
        "./imagens/andar2/morcego.txt");
    
    opcao opcoes_56[1] = {{'#', 57}};
    char texto_56[1][501] = {"-Voce: Mas o que e isso? Um caminho sem saida? O que eh esse muro engracado? E o que eh\nessa placa escrita... Beco Diagonal??\n-Servo: Parece que voce nasceu ontem? Voce eh muito burro. Esse eh um muro magico, deixe\nque eu abro ele.\n"};
	cadastrar_no(
        56,
        -1, 
        texto_56,
        0,
		nao_terminal,
        dialogo,
        1,
        opcoes_56,
        "./imagens/ambientacao/beco_diagonal.txt");

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
        "./imagens/ambientacao/tesouro.txt");
    
    opcao opcoes_65[3] = {{'E', 72}, {'D', 66}, {'V', 51}};
    char texto_65[1][501] = {"-Voce: E ca estamos em mais uma decisao de caminho.\nE-Esquerda D-Direita V-Voltar\nDigite uma opcao: "};
	cadastrar_no(
        65,
        -1, 
        texto_65,
        0,
		nao_terminal,
        caminhos,
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
        rancho,
        1,
        opcoes_71,
        "./imagens/ambientacao/velho_morto.txt");
    
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

//##################################ANDAR 3#####################################//

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
    char texto_101[1][501] = {"-Voce: Uma trifurcacao, que caminho sera o melhor? Sinto um bom chiero vindo da esquerda,\nsera que eh alguma armadilha? Vamos decidir isso logo.\nE-Esquerda F-Frente D-Direita\nDigite uma opcao: "};
	cadastrar_no(
        101,
        -1, 
        texto_101,
        0,
		nao_terminal,
        caminhos,
        3,
        opcoes_101,
        "./imagens/ambientacao/trifurcacao.txt");

    opcao opcoes_102[5] = {{'L', 103}, {'V', 101}, {'#', 104}, {'#', 105}, {'#', 106}};
    char texto_102[4][501] = {"-Voce: O que eh aquilo? Parece um virus mas como pode ser tao grande?\n-Voz misteriosa: Esta com medo? Deseja seguir em frente e lutar com ele?\nL-Lutar V-Voltar\nDigite uma opcao: ", "-Voce: Pode vir bicho esquisito. Nao eh uma gripezinha que vai me matar.\n-Virus: Voce acha que sou apenas um virus qualquer? Eu ja matei varios aventureiros\nigual a voce. Os humanos sao tao fracos, e, em se tratando do sistema imunologico  sao\nmais fracos ainda. Mua hahahahaha! :p\n", "-Voce: Pare de brincadeira seu virus.\n-Virus: Gosta de uma tosse? Entao vem pra cima!\n", "-Voce: Eh esse aqui nao aguentou meus socos. Talvez seja por que tomo vitamina C todos\nos dias kkkkk.\n-Servo: Nao seja tolo, seu burro. Isso foi apenas sorte\n"};
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
        "./imagens/ambientacao/tesouro.txt");

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
        "./imagens/ambientacao/velho.txt");

    opcao opcoes_108[4] = {{'#', 109}, {'#', 110}, {'#', 111}, {'#', 112}};
    char texto_108[4][501] = {"-Voce: Eh um lobo com aura verde, so pode ser o Wolf-Champion.\n-Servo: Agora eh o fim da linha para sua historia de heroi, duvido que voce o derrote.\n", "-Voce: Indentifique-se lobo maldito!\n-Fenrir: Humano, acha que um mero humano pode me tratar assim? Eu sou Fenrir, sou o lendario\nFenrir, aquele que arrancou a mao de um deus, sou filho de Loki!\n", "-Voce: Acha que isso me assusta? Com ajuda do elmo de escamas de um dragao, com essa\nforca insana que recebi experimente meu soco melhorado. Haia!!!\n-Lobo: Minha mordida vai te dilacerar raca inferior.\n", "-Voce: -rapidamente se esquivou da tentativa da mordida, e logo desfere um golpe\ncerteiro-.\n-Lobo: Como isso aconteceu? Meu nivel eh muito superior. Hm, o que te espera a frente faz\nparte da minha colecao de trofeus. Essa batalha, nao se engane, isso foi apenas um\ndeslize da minha atencao.\n"};
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
        "./imagens/ambientacao/tesouro.txt");

    opcao opcoes_115[3] = {{'E', 116}, {'D', 123}, {'V', 101}};
    char texto_115[1][501] = {"-Voce: Uma bifurcacao, que caminho sera o melhor?\n-Servo: arggggg, voce me da nos nervos.\nE-Esquerda D-Direita V-Voltar\nDigite uma opcao: "};
	cadastrar_no(
        115,
        -1, 
        texto_115,
        0,
		nao_terminal,
        caminhos,
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
        "./imagens/ambientacao/gema.txt");

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
    char texto_126[4][501] = {"-Voce: Pelas barbas de Odin, que calor eh esse? Parece que estamos chegando no inferno.\n-Servo: Voce continua falando demais, melhor voce se proteger, olha aquela figura com\ncara de diabo te esperando.\n", "\n-Demonio: Morraaaaa!\n-Voce: Calma la, mal cheguei e o anfitriao me recebe assim? Saudades de quando as pessoas\nainda possuiam respeito pelas outras.\n-Demonio: Eu vivo nesse labirinto a anos, e nunca deixei nem um humano passar. Sou mais\nantigo que qualquer ser que voce viu ate agora. Meus poderes superam tudo, meu corpo eh\ntao poderoso e resistente que nem uma armadura chega a ter a mesma resistencia. Um mero\nhumano nao chega a nem fazer cocegas em mim. Isso nunca vai acontecer.\n", "-Voce: Aaaaaaaaaaaaaahhhhh! Se liga nesse anel! O anel expansivo vai me dar o poder\nmagico que preciso para acabar com esse monstro.\n-Demonio: Ta legal, esse anel nao fara nem cocegas em mim, ja te avisei.\n-Voce: -voce colocou o anel no dedo e apontou para o demonio-.\n-Demonio: Nao pode ser, esse anel esta encantado com propriedades sagradas! Saia ja\ndaqui. Seu verme, quem te deu isso?!\n-Voce: Entao isso te afeta, esse eh o seu fim!\n", "-Voz misteriosa: Como foi possivel!? O Demonio foi derrotado? Isso nao vai sair barato,\nos inimigos a frente vao te dar uma licao Soh espere...\n-Voce: Esse bicho foi dificil de matar, mas por pouco consegui. E essa espada que ele\nusava... parecia interessante.\n"};
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
    char texto_130[3][501] = {"-Voce: Caramba, ele deixou cair a espada.\n-Voz misteriosa: Vamos-la, vai deixa-la para tras?\n", "-Voce: Ele eh muito quente.\n-Voz misteriosa: Obvio, esse eh o pau de fogo, a arma mais poderosa do submundo. Nao sei\nnem explicar como voce, um mero mortal conseguiu sobreviver ao implacavel poder dela.\n", "-Voce: Agora ninguem consegue me parar.\n"};
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

    opcao opcoes_133[1] = {{'#', 150}};
    char texto_133[1][501] = {"-Servo: Nao acredito que voce ainda esta vivo, ser repugnante. Vamos suba logo, voce tem que morrer."};
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

//##################################ANDAR 4#####################################//

opcao opcoes_150[1] = {{'#', 151}};
    char texto_150[1][501] = {"-Servo: Eh, ate parece que voce eh um bom jogador. Pena que soh tera essa fama por pouco\ntempo. Vamos ver se sobrevive a mais esse andar... \n"};
	cadastrar_no(
        150,
        -1, 
        texto_150,
        0,
		nao_terminal,
        entrada,
        1,
        opcoes_150,
        "./imagens/ambientacao/corredor.txt");
    
    opcao opcoes_151[3] = {{'E', 152}, {'D', 158}, {'F', 164}};
    char texto_151[1][501] = {"-Voce: Uma nova trifurcacao? nunca sei para onde devo ir. Vou escolher na sorte dessa vez.\n-Servo: Nao importa qual dos 3 caminhos voce escolha, a morte eh certa, eh apenas questao\nde tempo. Ela tem te acompanhado desde o inicio do jogo, a espera do momento perfeito.\nE-Esquerda F-Frente D-Direita\nDigite uma opcao: "};
	cadastrar_no(
        151,
        -1, 
        texto_151,
        0,
		nao_terminal,
        caminhos,
        3,
        opcoes_151,
        "./imagens/ambientacao/trifurcacao.txt");
    
    opcao opcoes_152[9] = {{'A', 153}, {'B', 154}, {'C', 155}, {'D', 156}, {'V',151}, {'#',152}, {'#',152},  {'#',152}, {'#',157}};
    char texto_152[5][501] = {"-Enigma: Talvez voce ate tenha sobrevivido a ele la embaixo,\nmas agora tera que saber uma das figuras mais poderosas que ja existiu e que nao se manteve\nintacta lutando com esse monstro. Qual dos deuses listados abaixo perdeu o braco direito para\no lobo Fenris?\nA-Thor B-Loki C-Frea D-Tyr V-Voltar \nDigite uma opcao: ", "Aquele loiro forte com uma martelo? nao.\n", "O irmao do loiro bonitao? definitivamente nao.\n", "Freya nao perderia o braco para um lobo qualquer\n", "Muito bem! Andou estudando mitologia nordica aluno? Agora que acertou teras acesso a um pre\ncioso tesouro...\n"};
	cadastrar_no(
        152,
        -1,
        texto_152,
        5,
		nao_terminal,
        enigma,
        5,
        opcoes_152,
        "./imagens/ambientacao/enigma.txt");
    
    opcao opcoes_157[1]={{'#', 151}}; 
    char texto_157[1][501] = {"Voce: Olha la, outra daquelas pedras, essa brilha mais que as outras.\nServo: Eu ja disse que voce fala demais?"};
	cadastrar_no(
        157,
        -1, 
        texto_157,
        0,
		nao_terminal,
        pedra,
        1,
        opcoes_157,
        "./imagens/ambientacao/gema.txt");

    opcao opcoes_158[1]={{'#', 159}}; 
    char texto_158[1][501] = {"-Voce: Que cheiro bom, de onde estah vindo?\n-Taberneiro: Bem-vindo a Taberna! Recupere suas forcas com alguns petiscos e um bom vinho!\n"};
	cadastrar_no(
        158,
        -1, 
        texto_158,
        0,
		nao_terminal,
        rancho,
        1,
        opcoes_158,
        "./imagens/ambientacao/taverna.txt");

    opcao opcoes_159[5]={{'A', 160}, {'B', 151}, {'#', 161}, {'#', 162},{'#',163}};
    char texto_159[4][501] = {"-Voce(sussurando): Algo com uma forma arredondada estranha estah aproximando... espere, mas\n ele tem uma boca gigante.\n-Servo(sussurando): Pela sua cara, parece que voce o conhece, nao eh mesmo? Para prosse\nguir tera que enfrenta-lo. Deseja seguir em frente e lutar com ele?\nA-Lutar B-Voltar\n Digite sua escolha: ", "-PACMAN: Croc! Croc! Crunch!\n-Voce: Ele quer me comer mas nao vai conseguir de modo algum!\n", "-PACMAN: Croc! Croc! Crunch!\n-Voce: Voce nao cansa seu bicho maldito!? Eu vou te matar!\n","-Voz misteriosa: Quase que ele te transformava num fantasminha, vai ter que melhorar\nainda, aluno...\n"};
	cadastrar_no(
        159,
        -1, 
        texto_159,
        4,
		nao_terminal,
        luta,
        5,
        opcoes_159,
        "./imagens/andar4/pac_man.txt");

    opcao opcoes_163[1]={{'#', 151}}; 
    char texto_163[1][501] = {"-Voce: O senhor parece onipresente nesse jogo\n-Senhor perdido: Enquanto o peao continuar girando, eu estarei aqui.\n"};
	cadastrar_no(
        163,
        -1, 
        texto_163,
        0,
		nao_terminal,
        dialogo,
        1,
        opcoes_163,
        "./imagens/ambientacao/velho.txt");

    opcao opcoes_164[3]={{'E', 165}, {'D', 172}, {'V', 151}}; 
    char texto_164[1][501] = {"-Voce: Agora uma bifurcacao, por onde devo ir?\nD-Direita E-Esquerda V-Voltar\nDigite uma opcao: "};
	cadastrar_no(
        164,
        -1, 
        texto_164,
        0,
		nao_terminal,
        caminhos,
        3,
        opcoes_164,
        "./imagens/ambientacao/bifurcacao.txt");
    
    opcao opcoes_165[4]={{'#', 166}, {'#', 167}, {'#', 168},{'#',169}};
    char texto_165[4][501] = {"-Voz misteriosa: Entre mortos e feridos, um morto-vivo se apresenta diante de voce. Para\nnao perecer como os demais jogadores que passaram por aqui, tera que enfrentar aquele que\nanda apos a morte, um zumbi Draugr. Para prosseguir tera que enfrenta-lo. Vamos ver se\nsobrevivera?\n-Voce(gritando): Eu vou mata-lo de qualquer jeitooooo!.\n", "-Zumbi: \"Aaaahhh!!\"\n-Voce: Ele quer me devorar mas nao vai conseguir!!!\n", "-Zumbi: \"Aaahhh!!\"\n-Voce: Seu ser malditoooooo! Eu vou te matar!\n","-Voz misteriosa: Vai ter que fazer muito melhor que isso se nao quiser morrer agora!\n"};
	cadastrar_no(
        165,
        -1, 
        texto_165,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_165,
        "./imagens/andar4/zumbi.txt");
    
    opcao opcoes_169[3]={{'#', 170},{'#', 171},{'#', 164}}; 
    char texto_169[3][501] = {"Voce acabou de ganhar um anel de esmeralda.\n","O anel carrega uma gigantesca e rara joia, a qual serviu de adorno para as mais belas deu\nsas, incluindo a Deusa Freyja.\n","Cuidado para nao perde-la!\n"};
	cadastrar_no(
        169,3
        -1, 
        texto_169,
        3,
		nao_terminal,
        sala_item,
        3,
        opcoes_169,
        "./imagens/ambientacao/tesouro.txt");

    opcao opcoes_172[4]={{'A', 4173}, {'B', 164}, {'#', 175}, {'#', 164}}; 
    char texto_172[3][501] = {"-Voce: Outra porta, essa parece emanar uma aura estranha.\nA-Tentar abrir B-Voltar\nDigite uma opcao: ", "-Porta: Eh por sua conta em risco aluno, pode passar.\n","-Porta: So queria te dizer que talvez voce possa estar deixando algo de muito precioso\npara a sua sobrevivencia nesse jogo.\n" };
	cadastrar_no(
        172,
        -1, 
        texto_172,
        3,
		nao_terminal,
        fechadura,
        2,
        opcoes_172,
        "./imagens/ambientacao/fechadura.txt");

    opcao opcoes_175[4]={{'#', 176}, {'#', 177}, {'#',178}, {'#', 179}}; 
    char texto_175[4][501] = {"-Cerquinha: Nao sou um piadista\nqualquer. Te derrotarei com o poder da matemagica!\n-Voce: Outro que acha que vai me derrotar com umas integrais quaisquer!.\n", "-Voce: Vou honrar as joias que coletei!\n-Cerquinha: Mais um que se acha diferente. Voce eh igual aos outros, aluno!\n","-Voce: Ninguem pode me parar!.\n-Cerquinha: hahahah!\n" ,"-Voz misteriosa: Veja soh. Resistiu ao poderoso Cerquinha. Sua morte ja eh certa aluno,\nnao resistira na proxima. Se chegar la, neh?\n"};
	cadastrar_no(
        175,
        -1, 
        texto_175,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_175,
        "./imagens/andar4/cerqueira.txt");
    
    opcao opcoes_179[1]={{'#',200}}; 
    char texto_179[1][501] = {"-Voz misteriosa: Vamos, suba! essa eh a ultima escada. Depois disso, vem a queda hahaha.\n"};
	cadastrar_no(
        179,
        -1, 
        texto_179,
        1,
		nao_terminal,
        sala_item,
        1,
        opcoes_179,
        "./imagens/ambientacao/escada.txt");

//##################################ANDAR 5#####################################//

opcao opcoes_200[1] = {{'#', 201}};
    char texto_200[1][501] = {"-Servo: Eh, eu ate poderia te deixar seguir nesse ultimo andar sozinho... mas quem iria te\nenterrar? Precisa ter um inutil como eu ao seu lado para fazer esse servico sujo...\n"};
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
    char texto_201[1][501] ={"-Voce: Uma nova trifurcacao? Sera essa a ultima?\n-Servo: Nao importa aluno, pare de fazer perguntas retoricas o tempo todo. Escolha que\ncaminho deseja seguir:\nE-Esquerda D-Direita F-Frente\nDigite uma opcao: "};
	cadastrar_no(
        201,
        -1, 
        texto_201,
        0,
		nao_terminal,
        caminhos,
        3,
        opcoes_201,
        "./imagens/ambientacao/trifurcacao.txt");
    
    opcao opcoes_202[9] = {{'A', 203}, {'B', 204}, {'C', 205},{'E',206},{'V',201}, {'#',202},{'#',207},{'#',202},{'#',202}};
    char texto_202[1][501] = {"-Enigma: Eh aluno. Esse eh o ultimo enigma que tera que resolver para sobreviver nessa\ntorre. Talvez voce se considere um genio, mas esse enigma foi escrito a milhares de anos \natras pelo criador desse jogo, Duvido que conseguira acertar.\nint x=10;\nint y=10;\nreturn x+y Qual o nome do filme?\nA-Missao impossivel B-O Codigo da Vinte\nC-Indiana Jones D-Dez eh pouco, vinte eh demais \nV-Voltar\nDigite uma opcao: "};
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
    char texto_211[1][501] = {"-Servo: Como voce eh insuportavel, eu vou falar! Se voce fizer a escolha certa, essa sera\na ultima bifurcacao desse jogo. Escolha que caminho deseja seguir:\nE-Esquerda D-Direita V-Voltar\nDigite uma opcao: "};
	cadastrar_no(
        211,
        -1, 
        texto_211,
        0,
		nao_terminal,
        caminhos,
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
        "./imagens/ambientacao/chifre.txt");

    opcao opcoes_219[4] = {{'A', 50220}, {'V', 211}, {'#', 222}, {'#', 211}};
    char texto_219[3][501] = {"-Voce: Nossa! Que hall legal! Nunca vi um corredor tao longo, para onde sera que ele leva?\n Uma porta sem fechadura? Mas como posso abri-la?\nA-Tentar abrir V-Voltar\nDigite uma opcao: ", "-Porta: Voce possui todas as joias que pertencem a poderosa deusa, idealizadora desta\ntorre, pode passar!\n", "-Porta: Acho que esta faltando alguma joia, ache-as e podera passar.\n"};
	cadastrar_no(
        219,
        -1, 
        texto_219,
        3,
		nao_terminal,
        fechadura,
        2,
        opcoes_219,
        "./imagens/ambientacao/porta.txt");
    
    opcao opcoes_222[4] = {{'#', 223}, {'#', 224}, {'#', 225}, {'#', 226}};
    char texto_222[4][501] = {"-Gabiqueen: Uau, pensei que nunca me conheceria. Que privilegio aluno! Agora voce sabe que\nnao foi um Deus que criou esse mundo,mas sim uma deusa.Como me diverti com voces colocando\nsuas vidas em risco apenas porque eu quis assim. Vamos direto ao ponto...Te derrotarei de\numa vez por todas com o poder da computacao!\n-Voce: Lutei bastante para chegar ate aqui! Nao vou deixar uma deusa sequer me derrotar!.\n", "-Voce: Vou honrar as joias que coletei!\n-Gabiqueen: Mais um que se acha diferente. Todas as joias sempre foram minhas, aluno!\n", "\n-Voce: NInguem pode me parar!.\n-Gabiqueen: Ninguem nunca passou por mim!\n","-Servo: Eh, voce nao era qualquer um...\n"};
	cadastrar_no(
        222,
        -1, 
        texto_222,
        4,
		nao_terminal,
        luta,
        4,
        opcoes_222,
        "./imagens/andar5/gabiqueen.txt");
    
    opcao opcoes_226[1] = {{'#', 227}};
    char texto_226[1][501] = {"\n-Voz misteriosa: Primeiramente, morra servo imundo, nao consegue nem matar um aluno.\n-Servo: Naaaaaaao - e morreu.\n-Voz misteriosa: Em segundo lugar, parabens aluno, nunca vi ninguem derrotar minha\ncriadora, nem sei o que acontecera comigo, mas sei que voce esta livre, pode atravessar o portal para seu mundo de novo.\n-Voce: Nunca vou me esquecer do que aconteceu aqui\n-assim o aluno corre em direcao ao portal-\n"};
	cadastrar_no(
        226,
        -1, 
        texto_226,
        0,
		nao_terminal,
        passar,
        1,
        opcoes_226,
        "./imagens/ambientacao/cenariofinal.txt");

    opcao opcoes_227[1] = {{'#', 228}};
    char texto_227[1][501] = {"-Cap: Dormindo em aula, aluno?\n"};
	cadastrar_no(
        227,
        -1, 
        texto_227,
        0,
		nao_terminal,
        saida,
        1,
        opcoes_227,
        "./imagens/andar5/salacap.txt");
    
    char texto_228[1][501] = {"                                         Duvidas?                                         \nOBRIGADO POR TER JOGADO CodeDungeon!\n"};
	cadastrar_no(
        228,
        -1, 
        texto_228,
        0,
		terminal,
        caminhos,
        0,
        NULL,
        "./imagens/ambientacao/TORRADO.txt");

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

