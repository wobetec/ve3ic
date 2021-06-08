#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *arquivo;
    char linha[100], *leitura;

    arquivo=fopen("C\\Users\\Pedro\\Dropbox\\My PC (LAPTOP-06JBDO7V)\\Documents\\IME\\curso\\IC\\jogo VE3\\bemvindo.txt", "rt"); 

    if(arquivo==NULL){
        printf("Erro na abertura do arquivo!");
        return;
    }

    while(!feof(arquivo)){
        leitura=fgets(linha, 100, arquivo);
        if(leitura){
            printf("%s", linha);
        }
    }
}