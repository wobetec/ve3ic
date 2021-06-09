#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(){
    imagens("./imagens/batalhaboss1.txt");
}