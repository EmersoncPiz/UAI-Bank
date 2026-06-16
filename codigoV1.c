//Codigo Teste 1, opçōes dentro do main
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char nome[101];
    int idade;
    double saldo;
} Usuario;

typedef struct {
    Usuario* lista;
    int tamanho;
    int capacidade;
    int proximoId;
} Banco;

void inicializarBanco(Banco* banco) {
    banco->capacidade = 5;
    banco->tamanho = 0;
    banco->proximoId = 1;
    banco->lista = (Usuario*) malloc(banco->capacidade * sizeof(Usuario));
}

void expandirCapacidade(Banco* banco) {
    if (banco->tamanho >= banco->capacidade) {
        banco->capacidade *= 2;
        banco->lista = (Usuario*) realloc(banco->lista, banco->capacidade * sizeof(Usuario));
    }
}

int buscarIndiceUsuario(Banco* banco, int id) {
    for (int i = 0; i < banco->tamanho; i++) {
        if (banco->lista[i].id == id) {
            return i;
        }
    }
    return -1;
}

void inserirUsuario(Banco* banco, char* nome, int idade, double saldo) {
    expandirCapacidade(banco);

    Usuario novoUsuario;
    novoUsuario.id = banco->proximoId;
    strcpy(novoUsuario.nome, nome);
    novoUsuario.idade = idade;
    novoUsuario.saldo = saldo;

    banco->lista[banco->tamanho] = novoUsuario;
    banco->tamanho++;
    banco->proximoId++;
}

void salvarUsuariosEmArquivo(Banco* banco, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo %s.\n", nomeArquivo);
        return;
    }
    
    for (int i = 0; i < banco->tamanho; i++) {
        fprintf(arquivo, "%s, %d, %.2f\n", 
                banco->lista[i].nome, 
                banco->lista[i].idade, 
                banco->lista[i].saldo);
    }
    
    fclose(arquivo);
}

void liberarMemoria(Banco* banco) {
    free(banco->lista);
    banco->lista = NULL;
    banco->tamanho = 0;
    banco->capacidade = 0;
}

//Código

int main() {
    Banco banco;
    inicializarBanco(&banco);

    // Matriz de strings
    char saidas[1000][256];
    int num_saidas = 0;

    int opcao;
    
    while (1) {
        if (scanf("%d", &opcao) == EOF) {
            break;
        }

        //OPÇĀO EXCLUSIVA PARA FINALIZAR O PROGRAMA
        if (opcao == 6) {
            break;
        }


        if (opcao == 1) { //Inserção de um novo usuário:
            char nome[101];
            int idade;
            double saldo;
            
            scanf(" %100[^,], %d, %lf", nome, &idade, &saldo);
            
            if (saldo < 0) {
                strcpy(saidas[num_saidas++], "Erro: O saldo não pode ser negativo.");
            } else {
                int idGerado = banco.proximoId;
                inserirUsuario(&banco, nome, idade, saldo);
                // Guardamos a saida em uma matriz (Para imprimir tudo no final do programa)
                sprintf(saidas[num_saidas++], "Usuário inserido com id %d.", idGerado);
            }
            
        } else if (opcao == 2) { //Inserção de vários usuários: (2, quantidade, )
            int quantidade;
            scanf("%d", &quantidade);
            
            int idInicial = banco.proximoId;
            
            for (int i = 0; i < quantidade; i++) {
                char nome[101];
                int idade;
                double saldo;
                scanf(" %100[^,], %d, %lf", nome, &idade, &saldo);
                inserirUsuario(&banco, nome, idade, saldo);
            }
            
            char temp[256] = "Usuários inseridos com id ";
            char numStr[20];
            for (int i = 0; i < quantidade; i++) {
                if (i > 0 && i < quantidade - 1) {
                    strcat(temp, ", ");
                } else if (i > 0 && i == quantidade - 1) {
                    strcat(temp, " e ");
                }
                sprintf(numStr, "%d", idInicial + i);
                strcat(temp, numStr);
            }
            strcat(temp, ".");
            strcpy(saidas[num_saidas++], temp);
            
        } else if (opcao == 3) {
            int idBusca;
            scanf("%d", &idBusca);
            
            int indice = buscarIndiceUsuario(&banco, idBusca);
            if (indice != -1) {
                sprintf(saidas[num_saidas++], "Usuário %d tem saldo de R$%.2f.", idBusca, banco.lista[indice].saldo);
            } else {
                sprintf(saidas[num_saidas++], "Erro: Usuário %d não encontrado.", idBusca);
            }
            
        } else if (opcao == 4) {
            int idOrigem, idDestino;
            double quantia;
            scanf("%d %d %lf", &idOrigem, &idDestino, &quantia);
            
            int indiceOrigem = buscarIndiceUsuario(&banco, idOrigem);
            int indiceDestino = buscarIndiceUsuario(&banco, idDestino);
            
            if (indiceOrigem == -1 || indiceDestino == -1) {
                strcpy(saidas[num_saidas++], "Erro: Usuário de origem ou destino não encontrado.");
            } else if (banco.lista[indiceOrigem].saldo < quantia) {
                strcpy(saidas[num_saidas++], "Erro: Saldo insuficiente para a transferência.");
            } else if (quantia < 0) {
                strcpy(saidas[num_saidas++], "Erro: A quantia de transferência não pode ser negativa.");
            } else {
                banco.lista[indiceOrigem].saldo -= quantia;
                banco.lista[indiceDestino].saldo += quantia;
                sprintf(saidas[num_saidas++], "Transferência de R$%.2f realizada com sucesso.", quantia);
            }
            
        } else if (opcao == 5) {
            int idRemover;
            scanf("%d", &idRemover);
            
            int indice = buscarIndiceUsuario(&banco, idRemover);
            if (indice != -1) {
                for (int i = indice; i < banco.tamanho - 1; i++) {
                    banco.lista[i] = banco.lista[i + 1];
                }
                banco.tamanho--;
                sprintf(saidas[num_saidas++], "Usuário %d removido com sucesso.", idRemover);
            } else {
                sprintf(saidas[num_saidas++], "Erro: Usuário %d não encontrado.", idRemover);
            }
        }
    }

    // Al presionar 6, se rompe el ciclo y llegamos aquí.
    // Imprimimos TODAS las salidas juntas.
    for (int i = 0; i < num_saidas; i++) {
        printf("%s\n", saidas[i]);
    }

    // Guardamos en archivo y liberamos memoria.
    salvarUsuariosEmArquivo(&banco, "example.txt");
    liberarMemoria(&banco);

    return 0;
}