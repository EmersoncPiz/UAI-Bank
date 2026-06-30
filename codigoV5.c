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
    int proximoId;
} Banco;

// Limpador de buffer
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Lê uma linha inteira e usa sscanf — elimina todos os problemas de buffer residual
// Retorna 1 se leu com sucesso, 0 caso contrário
int lerDadosUsuario(char* nome, int* idade, double* saldo) {
    char linha[256];

    // Lê a linha inteira (fgets consome o '\n' também)
    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        return 0;
    }

    // Tenta parsear no formato esperado: "nome, idade, saldo"
    if (sscanf(linha, " %100[^,], %d, %lf", nome, idade, saldo) != 3) {
        return 0;
    }

    return 1;
}

// PARTE DE ALOCAÇÃO DE MEMÓRIA

void aumentarMemoriaUm(Banco* banco) {
    Usuario* temp = (Usuario*) realloc(banco->lista, (banco->tamanho + 1) * sizeof(Usuario));
    if (temp == NULL) {
        printf("Erro: Falha na alocação de memória.\n");
        exit(1);
    }
    banco->lista = temp;
}

void aumentarMemoriaVarios(Banco* banco, int n) {
    Usuario* temp = (Usuario*) realloc(banco->lista, (banco->tamanho + n) * sizeof(Usuario));
    if (temp == NULL) {
        printf("Erro: Falha na alocação de memória.\n");
        exit(1);
    }
    banco->lista = temp;
}

void diminuirMemoriaUm(Banco* banco) {
    if (banco->tamanho > 0) {
        Usuario* temp = (Usuario*) realloc(banco->lista, banco->tamanho * sizeof(Usuario));
        if (temp == NULL && banco->tamanho > 0) {
            printf("Erro crítico: Falha na realocação de memória.\n");
            exit(1);
        }
        banco->lista = temp;
    } else {
        free(banco->lista);
        banco->lista = NULL;
    }
}

// Busca Binária por id
int buscaBinaria(Banco* banco, int id) {
    int inicio = 0;
    int fim = banco->tamanho - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;

        if (banco->lista[meio].id == id) {
            return meio;
        }
        if (banco->lista[meio].id < id) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    return -1;
}

// OPÇÕES DO MENU

// Inserir um usuário
void opcao1(Banco* banco) {
    char nome[101];
    int idade;
    double saldo;

    // Consome o resto da linha (incluindo '\n') que ficou após ler a opção do menu
    limparBuffer();

    if (!lerDadosUsuario(nome, &idade, &saldo)) {
        printf("Erro: Entrada inválida. Formato esperado: nome, idade, saldo\n");
        return;
    }

    if (idade <= 0) {
        printf("Erro: A idade deve ser maior que zero.\n");
        return;
    }

    if (saldo < 0) {
        printf("Erro: O saldo não pode ser negativo.\n");
        return;
    }

    aumentarMemoriaUm(banco);

    Usuario novoUsuario;
    novoUsuario.id = banco->proximoId;
    strcpy(novoUsuario.nome, nome);
    novoUsuario.idade = idade;
    novoUsuario.saldo = saldo;

    banco->lista[banco->tamanho] = novoUsuario;
    banco->tamanho++;
    banco->proximoId++;

    printf("Usuário inserido com id %d.\n", novoUsuario.id);
}

// Inserir vários usuários
void opcao2(Banco* banco) {
    int n;
    printf("Quantidade de usuários a registrar:\n");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Erro: Entrada inválida. Formato esperado: (numero inteiro positivo)\n");
        limparBuffer();
        return;
    }

    // Aumentamos a memória de uma só vez para os N usuários
    aumentarMemoriaVarios(banco, n);

    // Consome o '\n' que ficou no buffer após ler n
    limparBuffer();

    for (int i = 0; i < n; i++) {
        char nome[101];
        int idade;
        double saldo;

        printf("Digite os dados do usuário %d:\n", i + 1);

        if (!lerDadosUsuario(nome, &idade, &saldo)) {
            printf("Erro: Entrada inválida. Formato esperado: nome, idade, saldo\n");
            i--; // Repetir leitura deste usuário
            continue;
        }

        // FIX: validação de idade que faltava em opcao2
        if (idade <= 0) {
            printf("Erro: A idade deve ser maior que zero.\n");
            i--;
            continue;
        }

        if (saldo < 0) {
            printf("Erro: O saldo não pode ser negativo.\n");
            i--;
            continue;
        }

        Usuario novoUsuario;
        novoUsuario.id = banco->proximoId;
        strcpy(novoUsuario.nome, nome);
        novoUsuario.idade = idade;
        novoUsuario.saldo = saldo;

        banco->lista[banco->tamanho] = novoUsuario;
        banco->tamanho++;
        banco->proximoId++;

        printf("Usuário inserido com id %d.\n", novoUsuario.id);
    }
}

// Buscar usuário por id
void opcao3(Banco* banco) {
    int idBusca;
    printf("Digite o id do usuário:\n");
    if (scanf("%d", &idBusca) != 1) {
        printf("Erro: Entrada inválida. Formato esperado: (numero inteiro)\n");
        limparBuffer();
        return;
    }
    limparBuffer();

    int indice = buscaBinaria(banco, idBusca);
    if (indice != -1) {
        printf("Usuário %d tem saldo de R$%.2f.\n", idBusca, banco->lista[indice].saldo);
    } else {
        printf("Erro: Usuário %d não encontrado.\n", idBusca);
    }
}

// Realizar transferência
void opcao4(Banco* banco) {
    int idOrigem, idDestino;
    double quantia;

    printf("Dados da transferência:\n");
    if (scanf("%d %d %lf", &idOrigem, &idDestino, &quantia) != 3) {
        printf("Erro: Entrada inválida. Formato esperado: <id origem> <id destino> <quantia>\n");
        limparBuffer();
        return;
    }
    // Descarta qualquer resto na linha
    limparBuffer();

    if (idOrigem == idDestino) {
        printf("Erro: Não é possível transferir para o mesmo usuário.\n");
        return;
    }

    int indiceOrigem  = buscaBinaria(banco, idOrigem);
    int indiceDestino = buscaBinaria(banco, idDestino);

    if (indiceOrigem == -1 || indiceDestino == -1) {
        printf("Erro: Usuário de origem ou destino não encontrado.\n");
    } else if (quantia <= 0) {
        printf("Erro: A quantia de transferência deve ser maior que zero.\n");
    } else if (banco->lista[indiceOrigem].saldo < quantia) {
        printf("Erro: Saldo insuficiente para a transferência.\n");
    } else {
        banco->lista[indiceOrigem].saldo  -= quantia;
        banco->lista[indiceDestino].saldo += quantia;
        printf("Transferência de R$%.2f realizada com sucesso.\n", quantia);
        printf("Usuário %d tem saldo de R$%.2f.\n", idOrigem,  banco->lista[indiceOrigem].saldo);
        printf("Usuário %d tem saldo de R$%.2f.\n", idDestino, banco->lista[indiceDestino].saldo);
    }
}

// Remover usuário
void opcao5(Banco* banco) {
    int idRemover;
    printf("Digite o id do usuário a remover:\n");
    if (scanf("%d", &idRemover) != 1) {
        printf("Erro: Entrada inválida. Formato esperado: (numero inteiro)\n");
        limparBuffer();
        return;
    }
    // Descarta qualquer resto na linha
    limparBuffer();

    int indice = buscaBinaria(banco, idRemover);
    if (indice != -1) {
        for (int i = indice; i < banco->tamanho - 1; i++) {
            banco->lista[i] = banco->lista[i + 1];
        }
        banco->tamanho--;

        diminuirMemoriaUm(banco);

        printf("Usuário %d removido com sucesso.\n", idRemover);
    } else {
        printf("Erro: Usuário %d não encontrado.\n", idRemover);
    }
}

// Salvar arquivo
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

// Menu
void imprimirMenu() {
    printf("\n===== MENU =====\n");
    printf("1. Inserir um novo usuario: nome, idade, saldo\n");
    printf("2. Inserir varios usuarios: numero de usuarios\n");
    printf("3. Buscar usuario por id: <id>\n");
    printf("4. Transferencia entre usuarios: <id origem> <id destino> <quantia>\n");
    printf("5. Remover usuario por id: <id>\n");
    printf("6. Sair\n");
    printf("Escolha uma opcao: \n");
}

int main() {
    Banco banco;
    banco.lista     = NULL;
    banco.tamanho   = 0;
    banco.proximoId = 1;

    int opcao;

    imprimirMenu();

    while (1) {
        if (scanf("%d", &opcao) != 1 || opcao < 1 || opcao > 6) {
            printf("Erro: Opção inválida.\n");
            limparBuffer();
            imprimirMenu();
            continue;
        }

        if (opcao == 6) {
            break;
        }

        switch (opcao) {
            case 1: opcao1(&banco); break;
            case 2: opcao2(&banco); break;
            case 3: opcao3(&banco); break;
            case 4: opcao4(&banco); break;
            case 5: opcao5(&banco); break;
        }
    }

    salvarUsuariosEmArquivo(&banco, "example.txt");

    if (banco.lista != NULL) {
        free(banco.lista);
        banco.lista = NULL;
    }

    return 0;
}