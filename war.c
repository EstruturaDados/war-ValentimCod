// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.
#define QTD_TERRITORIOS 5
#define QTD_MISSOES 3
#define MAX_STR 50
#define MAX_NOME 30
#define MAX_COR 10

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.
typedef struct {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    int tropas;
} Territorio;

// --- Protótipos das Funções ---
// Declarações antecipadas de todas as funções que serão usadas no programa, organizadas por categoria.
// Funções de setup e gerenciamento de memória:
Territorio* alocarMapa(int qtd);
void inicializarTerritorios(Territorio *mapa, int qtd);
void liberarMemoria(Territorio *mapa);

// Funções de interface com o usuário:
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio *mapa, int qtd);
void exibirMissao(int idMissao);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio *mapa, int qtd, const char *corJogador);
void simularAtaque(Territorio *origem, Territorio *destino);
int sortearMissao(void);
int verificarVitoria(const Territorio *mapa, int qtd, int idMissao, const char *corJogador);

// Função utilitária:
void limparBufferEntrada(void);

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.
int main() {
    setbuf(stdout, NULL);
    srand(time(NULL));

    int qtd;
    printf("Quantos territórios deseja cadastrar? ");
    scanf("%d", &qtd);
    limparBufferEntrada();

    Territorio* mapa = alocarMapa(qtd);
    if (!mapa) {
        printf("Erro ao alocar memória!\n");
        return 1;
    }

    inicializarTerritorios(mapa, qtd);

    char corJogador[MAX_STR] = "Azul";
    int missao = sortearMissao();

    int opcao;
    int venceu = 0;

    do {
        exibirMapa(mapa, qtd);
        exibirMissao(missao);
        exibirMenuPrincipal();
        printf("Escolha: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, qtd, corJogador);
                break;
            case 2:
                venceu = verificarVitoria(mapa, qtd, missao, corJogador);
                if (venceu) {
                    printf("\n🎉 Missão cumprida! Você venceu!\n");
                } else {
                    printf("\n⚠️ Ainda não cumpriu sua missão.\n");
                }
                break;
            case 0:
                printf("Encerrando o jogo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }

    } while (opcao != 0 && !venceu);

    liberarMemoria(mapa);
    return 0;
}

// --- Implementação das Funções ---

// alocarMapa():
Territorio* alocarMapa(int tamanho) {
    return (Territorio*) calloc(tamanho, sizeof(Territorio));
}

// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.
// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).
void inicializarTerritorios(Territorio* mapa, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("Digite o nome do território %d: ", i + 1);
        fgets(mapa[i].nome, MAX_NOME, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Digite a cor do exército: ");
        fgets(mapa[i].cor, MAX_COR, stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Digite o número de tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }
}

// liberarMemoria():
void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

// Libera a memória previamente alocada para o mapa usando free.

// exibirMenuPrincipal():
void exibirMenuPrincipal(void) {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1 - Atacar\n");
    printf("2 - Verificar vitória\n");
    printf("0 - Sair\n");
}

// Imprime na tela o menu de ações disponíveis para o jogador.

// exibirMapa():
void exibirMapa(const Territorio* mapa, int tamanho) {
    printf("\n=== ESTADO ATUAL DO MAPA ===\n");
    printf("%-3s | %-25s | %-10s | Tropas\n", "ID", "Nome", "Cor");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < tamanho; i++) {
        printf("%-3d | %-25s | %-10s | %d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.

// exibirMissao():
// Exibe a descrição da missão atual do jogador com base no ID da missão sorteada.
// exibirMissao():
void exibirMissao(int missao) {
    printf("\n=== SUA MISSÃO ===\n");
    switch (missao) {
        case 0: printf("Conquistar 2 territórios.\n"); break;
        case 1: printf("Eliminar todos os exércitos Vermelhos.\n"); break;
        case 2: printf("Controlar pelo menos 15 tropas.\n"); break;
        default: printf("Missão desconhecida.\n");
    }
}

// faseDeAtaque():
void faseDeAtaque(Territorio* mapa, int tamanho, const char* corJogador) {
    int origem, destino;

    printf("\nDigite o índice do território de origem (0-%d): ", tamanho - 1);
    scanf("%d", &origem);
    printf("Digite o índice do território de destino (0-%d): ", tamanho - 1);
    scanf("%d", &destino);
    limparBufferEntrada();

    if (origem < 0 || origem >= tamanho || destino < 0 || destino >= tamanho) {
        printf("Índices inválidos!\n");
        return;
    }

    if (strcmp(mapa[origem].cor, corJogador) != 0) {
        printf("Você só pode atacar de territórios que controla!\n");
        return;
    }

    simularAtaque(&mapa[origem], &mapa[destino]);
}

// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
// Chama a função simularAtaque() para executar a lógica da batalha.

// simularAtaque():
void simularAtaque(Territorio* origem, Territorio* destino)
{
    if (origem->tropas < 2) {
        printf("Território de origem não possui tropas suficientes.\n");
        return;
    }

    printf("\nAtacante: %s (%s) | Tropas: %d\n", origem->nome, origem->cor, origem->tropas);
    printf("Defensor: %s (%s) | Tropas: %d\n", destino->nome, destino->cor, destino->tropas);

    int dadoAtacante = rand() % 6 + 1;
    int dadoDefensor = rand() % 6 + 1;

    printf("Dado do atacante: %d | Dado do defensor: %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        int tropasAntes = destino->tropas;
        destino->tropas--;
        printf("Defensor perdeu 1 tropa! (de %d para %d)\n", tropasAntes, destino->tropas);
        if (destino->tropas <= 0) {
            destino->tropas = 1;
            strcpy(destino->cor, origem->cor);
            printf("Território %s conquistado!\n", destino->nome);
        }
    } else {
        int tropasAntes = origem->tropas;
        origem->tropas--;
        printf("Atacante perdeu 1 tropa! (de %d para %d)\n", tropasAntes, origem->tropas);
    }
}
// Executa a lógica de uma batalha entre dois territórios.
// Realiza validações, rola os dados, compara os resultados e atualiza o número de tropas.
// Se um território for conquistado, atualiza seu dono e move uma tropa.

// sortearMissao():
int sortearMissao(void) {
    return rand() % QTD_MISSOES;
}
// Sorteia e retorna um ID de missão aleatório para o jogador.

// verificarVitoria():
int verificarVitoria(const Territorio* mapa, int tamanho, int missao, const char* corJogador) {
    switch (missao) {
        case 0: {
            int count = 0;
            for (int i = 0; i < tamanho; i++)
                if (strcmp(mapa[i].cor, corJogador) == 0)
                    count++;
            return count >= 2;
        }
        case 1: {
            for (int i = 0; i < tamanho; i++)
                if (strcmp(mapa[i].cor, "Vermelho") == 0)
                    return 0;
            return 1;
        }
        case 2: {
            int total = 0;
            for (int i = 0; i < tamanho; i++)
                if (strcmp(mapa[i].cor, corJogador) == 0)
                    total += mapa[i].tropas;
            return total >= 15;
        }
    }
    return 0;
}
// Verifica se o jogador cumpriu os requisitos de sua missão atual.
// Implementa a lógica para cada tipo de missão (destruir um exército ou conquistar um número de territórios).
// Retorna 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.

// limparBufferEntrada():
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// Função utilitária para limpar o buffer de entrada do teclado (stdin), evitando problemas com leituras consecutivas de scanf e getchar.
