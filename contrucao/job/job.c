#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *imagens(char *nome_arquivo);
int barra_de_vida(int vida);

int vidamax;

int main(){
    imagens("./imagens/batalhaboss1.txt");
    barra_de_vida(25); 
}

char *imagens(char *nome_arquivo){
    FILE* arquivo;
    char linha[100], *leitura=NULL;

    arquivo=fopen(nome_arquivo, "rt");

    if(arquivo==NULL){
        printf("\nERRO AO ABRIR A IMAGEM!");
        fprintf(arquivo, "\nERRO AO ABRIR A IMAGEM!");
        exit(1); 
    }

    while(!feof(arquivo)){
        leitura=fgets(linha, 100, arquivo);
            if(leitura){
                printf("%s", linha);
            }
    }       
    fclose(arquivo);
}

int barra_de_vida(int vida){
    int i=0;

    printf("\n-----------------------------------\n");
    printf("HP: "); 
    for(i=0; i<vida/4; i++){    
        printf("|");
    }
    for(int j=(vida/4); j<vidamax/4; j++){
        printf(" ");
    }
    printf(" %d/%d\n", vida, vidamax); 
    printf("-----------------------------------\n");
}