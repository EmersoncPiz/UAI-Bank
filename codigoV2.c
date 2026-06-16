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

// ================= FUNÇŌES DE GESTĀO DE MEMORIA =================

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

void liberarMemoria(Banco* banco) {
    free(banco->lista);
    banco->lista = NULL;
    banco->tamanho = 0;
    banco->capacidade = 0;
}

// ================= FUNÇŌES AUXILIARES =================

int buscarIndiceUsuario(Banco* banco, int id) { //usado para a opçāo 3 y 5
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

void salvarUsuariosEmArquivo(Banco* banco, const char* nomeArquivo) { //criaçāo do arquivo no final do programa
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

// ================= FUNCIONES DE LAS OPCIONES DEL MENÚ =================

void operacaoInserirUsuario(Banco* banco, char saidas[][256], int* num_saidas) { //opçāo 1
    char nome[101];
    int idade;
    double saldo;
    
    scanf(" %100[^,], %d, %lf", nome, &idade, &saldo);
    
    if (saldo < 0) {
        strcpy(saidas[*num_saidas], "Erro: O saldo não pode ser negativo.");
    } else {
        int idGerado = banco->proximoId;
        inserirUsuario(banco, nome, idade, saldo);
        sprintf(saidas[*num_saidas], "Usuário inserido com id %d.", idGerado);
    }
    (*num_saidas)++;
}

void operacaoInserirVariosUsuarios(Banco* banco, char saidas[][256], int* num_saidas) { //opçāo 2
    int quantidade;
    scanf("%d", &quantidade);
    
    int idInicial = banco->proximoId;
    
    for (int i = 0; i < quantidade; i++) {
        char nome[101];
        int idade;
        double saldo;
        scanf(" %100[^,], %d, %lf", nome, &idade, &saldo);
        inserirUsuario(banco, nome, idade, saldo);
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
    strcpy(saidas[*num_saidas], temp);
    (*num_saidas)++;
}

void operacaoBuscarUsuario(Banco* banco, char saidas[][256], int* num_saidas) { //opçāo 3
    int idBusca;
    scanf("%d", &idBusca);
    
    int indice = buscarIndiceUsuario(banco, idBusca);
    if (indice != -1) {
        sprintf(saidas[*num_saidas], "Usuário %d tem saldo de R$%.2f.", idBusca, banco->lista[indice].saldo);
    } else {
        sprintf(saidas[*num_saidas], "Erro: Usuário %d não encontrado.", idBusca);
    }
    (*num_saidas)++;
}

void operacaoTransferencia(Banco* banco, char saidas[][256], int* num_saidas) { //opçāo 4
    int idOrigem, idDestino;
    double quantia;
    scanf("%d %d %lf", &idOrigem, &idDestino, &quantia);
    
    int indiceOrigem = buscarIndiceUsuario(banco, idOrigem);
    int indiceDestino = buscarIndiceUsuario(banco, idDestino);
    
    if (indiceOrigem == -1 || indiceDestino == -1) {
        strcpy(saidas[*num_saidas], "Erro: Usuário de origem ou destino não encontrado.");
    } else if (banco->lista[indiceOrigem].saldo < quantia) {
        strcpy(saidas[*num_saidas], "Erro: Saldo insuficiente para a transferência.");
    } else if (quantia < 0) {
        strcpy(saidas[*num_saidas], "Erro: A quantia de transferência não pode ser negativa.");
    } else {
        banco->lista[indiceOrigem].saldo -= quantia;
        banco->lista[indiceDestino].saldo += quantia;
        sprintf(saidas[*num_saidas], "Transferência de R$%.2f realizada com sucesso.", quantia);
    }
    (*num_saidas)++;
}

void operacaoRemoverUsuario(Banco* banco, char saidas[][256], int* num_saidas) { //opçāo 5
    int idRemover;
    scanf("%d", &idRemover);
    
    int indice = buscarIndiceUsuario(banco, idRemover);
    if (indice != -1) {
        for (int i = indice; i < banco->tamanho - 1; i++) {
            banco->lista[i] = banco->lista[i + 1];
        }
        banco->tamanho--;
        sprintf(saidas[*num_saidas], "Usuário %d removido com sucesso.", idRemover);
    } else {
        sprintf(saidas[*num_saidas], "Erro: Usuário %d não encontrado.", idRemover);
    }
    (*num_saidas)++;
}

// ================= FUNÇĀO PRINCIPAL =================

int main() {
    Banco banco;
    inicializarBanco(&banco);

    char saidas[1000][256];
    int num_saidas = 0;

    int opcao;
    
    // El menú principal idéntico ao fluxograma
    while (1) {
        if (scanf("%d", &opcao) == EOF || opcao == 6) {
            break; // 6 vai primero pra economizar tempo de execuçāo. Menos comparaçōes
        }

        if (opcao == 1) {
            operacaoInserirUsuario(&banco, saidas, &num_saidas);
        } 
        else if (opcao == 2) {
            operacaoInserirVariosUsuarios(&banco, saidas, &num_saidas);
        } 
        else if (opcao == 3) {
            operacaoBuscarUsuario(&banco, saidas, &num_saidas);
        } 
        else if (opcao == 4) {
            operacaoTransferencia(&banco, saidas, &num_saidas);
        } 
        else if (opcao == 5) {
            operacaoRemoverUsuario(&banco, saidas, &num_saidas);
        }
    }

    // Imprimir todas as saídas armazenadas
    for (int i = 0; i < num_saidas; i++) {
        printf("%s\n", saidas[i]);
    }

    // Finalizar programa
    salvarUsuariosEmArquivo(&banco, "example.txt");
    liberarMemoria(&banco);

    return 0;
}