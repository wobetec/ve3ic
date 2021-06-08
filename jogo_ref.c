
int main() { 

	//laco de controle do jogo
	while(1) {
		//Se no nao eh terminal, apresentar texto e ler a opcao selecionada
		if(ptr_atual->tipo != terminal) {
			int indice_proximo_no = -1;
			while(indice_proximo_no == -1) {
				char opcao;
				printf("%s", ptr_atual->texto);
				fprintf(arquivo_saida, "%s", ptr_atual->texto);
				scanf(" %c", &opcao);
				fprintf(arquivo_saida, "%c\n", opcao);

				//Ler proximo no a partir da opcao
				indice_proximo_no = ler_indice_proximo_no(opcao);
				if(indice_proximo_no == -1) {
					printf("OPCAO INVALIDA!\n");
					fprintf(arquivo_saida, "OPCAO INVALIDA!\n");
				}
			}
			//Se o acesso ao proximo no selecionado esta liberado, ler no, atualizar posicao atual e criterios globais
			if(verificar_criterio_acesso(indice_proximo_no)) {
				no *ptr_proximo_no = buscar_no(indice_proximo_no);
				ptr_atual = ptr_proximo_no;
				atualizar_criterios_globais(indice_proximo_no);
			}
		}
		//Se no eh terminal, apresentar texto e finalizar programa
		else {
			printf("%s", ptr_atual->texto);
			fprintf(arquivo_saida, "%s", ptr_atual->texto);
			break;
		}
	}

}
