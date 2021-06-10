////////////////////////INCLUSAO DE BIBLIOTECAS///////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void imagem(char *nome_arquivo){
    FILE* arquivo;
    char linha[100], *leitura=NULL;

    arquivo=fopen(nome_arquivo, "rt");

    if(arquivo==NULL){
        printf("\nERRO 07: ERRO AO ABRIR A IMAGEM.");
        fprintf(arquivo, "\nERRO 07: ERRO AO ABRIR A IMAGEM.");
        exit(1); 
    }

    while(!feof(arquivo)){
        leitura=fgets(linha, 100, arquivo);
            if(leitura){
                printf("%s", linha);
            }
    }      
    printf("\n"); 
    fclose(arquivo);
}
int vida = 100;
int vida_MAX = 100;
int pedras = 0;
int XP = 40;
int nivel = 0;
int ataque = 14;

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

void barra_superior_luta(int vida_inimigo, int vida_inimigo_MAX, int ataque_inimigo){
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

int main(){
    system("MODE con cols=90 lines=35 ");
    system("cls");
    barra_superior_luta(100, 100, 16);
    imagem("./imagens/batalha1.txt");
    system("pause");
    return 0;
}