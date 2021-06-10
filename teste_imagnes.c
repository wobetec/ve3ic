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


int main(){
    imagem("./imagens/batalha1.txt");

    return 0;
}