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

// ================= FUNÇÕES DE GESTÃO DE MEMÓRIA =================

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

// ================= FUNÇÕES AUXILIARES =================

// OPTIMIZAÇÃO 1: Busca Binária O(log n) em vez de Busca Linear O(n)
/* Na busca linear procura no primeiro e se nāo for o primeiro, procura no segundo e assim por diante.
entāo se a gente manda procurar o elemento 1000 vai fazer 1000 vezes. */

/*Na busca binaria pega o total e divide por dois, se estiver da metade pra cima
entāo divide a metade superior en dois de novo e assim por diante, em promedio vai procurar acho q umas 9-10 vezes*/

int buscarIndiceUsuario(Banco* banco, int id) { // usado para as opções 3, 4 e 5
    int inicio = 0;
    int fim = banco->tamanho - 1;
    
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        
        if (banco->lista[meio].id == id) {
            return meio; // Usuário encontrado
        }
        if (banco->lista[meio].id < id) {
            inicio = meio + 1; // Buscar na metade superior
        } else {
            fim = meio - 1; // Buscar na metade inferior
        }
    }
    return -1; // Usuário não encontrado
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

void salvarUsuariosEmArquivo(Banco* banco, const char* nomeArquivo) { // criação do arquivo no final do programa
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

// ================= FUNÇÕES DAS OPÇÕES DO MENU =================

void operacaoInserirUsuario(Banco* banco, char saidas[][256], int* num_saidas) { // opção 1
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

void operacaoInserirVariosUsuarios(Banco* banco, char saidas[][256], int* num_saidas) { // opção 2
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

void operacaoBuscarUsuario(Banco* banco, char saidas[][256], int* num_saidas) { // opção 3
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

void operacaoTransferencia(Banco* banco, char saidas[][256], int* num_saidas) { // opção 4
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

void operacaoRemoverUsuario(Banco* banco, char saidas[][256], int* num_saidas) { // opção 5
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

// ================= FUNÇÃO PRINCIPAL =================

int main() {
    Banco banco;
    inicializarBanco(&banco);

    char saidas[1000][256];
    int num_saidas = 0;

    int opcao;
    
    // O menu principal idêntico ao fluxograma
    while (1) {
        if (scanf("%d", &opcao) == EOF || opcao == 6) {
            break; // 6 vai primeiro pra economizar tempo de execução. Menos comparações
        }

        switch (opcao) {
            case 1:
                operacaoInserirUsuario(&banco, saidas, &num_saidas);
                break;
            case 2:
                operacaoInserirVariosUsuarios(&banco, saidas, &num_saidas);
                break;
            case 3:
                operacaoBuscarUsuario(&banco, saidas, &num_saidas);
                break;
            case 4:
                operacaoTransferencia(&banco, saidas, &num_saidas);
                break;
            case 5:
                operacaoRemoverUsuario(&banco, saidas, &num_saidas);
                break;
            default:
                break;
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