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
#define QTD_MISSOES 5
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
void liberarMemoria(Territorio *mapa, char* missao);

// Funções de interface com o usuário:
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio *mapa, int qtd);
void exibirMissao(const char* missao);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio *mapa, int qtd, const char *corJogador);
void simularAtaque(Territorio *origem, Territorio *destino);
int sortearMissao(void);
int verificarVitoria(const Territorio *mapa, int qtd, int idMissao, const char *corJogador);

// Função utilitária:
void limparBufferEntrada(void);
void atribuirMissao(char** destino, const char* missoes[], int totalMissoes);

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

    char corJogador[MAX_COR] = "Azul";
    char* missaoJogador = NULL;
    atribuirMissao(&missaoJogador, missoes, QTD_MISSOES);

    exibirMissao(missaoJogador);

    int opcao;
    int venceu = 0;

    do {
        exibirMapa(mapa, qtd);
        exibirMenuPrincipal();
        printf("Escolha: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1: {
                int origem, destino;
                printf("\nDigite o índice do território de origem (0-%d): ", qtd - 1);
                scanf("%d", &origem);
                printf("Digite o índice do território de destino (0-%d): ", qtd - 1);
                scanf("%d", &destino);
                limparBufferEntrada();

                if (origem < 0 || origem >= qtd || destino < 0 || destino >= qtd) {
                    printf("Índices inválidos!\n");
                    break;
                }
                if (strcmp(mapa[origem].cor, corJogador) != 0) {
                    printf("Você só pode atacar de territórios que controla!\n");
                    break;
                }
                atacar(&mapa[origem], &mapa[destino]);
                break;
            }
            case 2:
                venceu = verificarMissao(missaoJogador, mapa, qtd, corJogador);
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

        // Verificação silenciosa ao final de cada turno
        if (verificarMissao(missaoJogador, mapa, qtd, corJogador)) {
            printf("\n🎉 Missão cumprida! Você venceu!\n");
            break;
        }

    } while (opcao != 0 && !venceu);

    liberarMemoria(mapa, missaoJogador);
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
void liberarMemoria(Territorio* mapa, char* missao) {
    free(mapa);
    free(missao);
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
void exibirMissao(const char* missao) {
    printf("\n=== SUA MISSÃO ===\n%s\n", missao);
}

const char* missoes[QTD_MISSOES] = {
    "Conquistar pelo menos 2 territórios.",
    "Controlar pelo menos 15 tropas.",
    "Conquistar 2 territórios consecutivos.",
    "Ter pelo menos 1 território de cada cor cadastrada.",
    "Ser dono de todos os territórios.",
    "Ter um território com mais de 10 tropas.",
    "Não perder nenhum território durante o jogo.",
    "Ter apenas territórios de uma única cor.",
    "Conquistar o território com maior número de tropas.",
    "Ficar com pelo menos 3 territórios com 5 ou mais tropas."
};

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
        printf("Atacante venceu!\n");
        strcpy(destino->cor, origem->cor);
        int tropasTransferidas = origem->tropas / 2;
        destino->tropas = tropasTransferidas;
        origem->tropas -= tropasTransferidas;
        if (origem->tropas < 1) origem->tropas = 1;
        printf("Defensor agora tem %d tropas e mudou de cor para %s.\n", destino->tropas, destino->cor);
    } else {
        origem->tropas--;
        printf("Defensor resistiu! Atacante perdeu 1 tropa (agora tem %d).\n", origem->tropas);
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
            // Conquistar pelo menos 2 territórios
            int count = 0;
            for (int i = 0; i < tamanho; i++)
                if (strcmp(mapa[i].cor, corJogador) == 0)
                    count++;
            return count >= 2;
        }
        case 1: {
            // Controlar pelo menos 15 tropas
            int total = 0;
            for (int i = 0; i < tamanho; i++)
                if (strcmp(mapa[i].cor, corJogador) == 0)
                    total += mapa[i].tropas;
            return total >= 15;
        }
        case 2: {
            // Conquistar 2 territórios consecutivos
            for (int i = 0; i < tamanho - 1; i++)
                if (strcmp(mapa[i].cor, corJogador) == 0 && strcmp(mapa[i+1].cor, corJogador) == 0)
                    return 1;
            return 0;
        }
        case 3: {
            // Ter pelo menos 1 território de cada cor cadastrada
            int temCor[MAX_COR] = {0};
            int qtdCores = 0;
            for (int i = 0; i < tamanho; i++) {
                int jaTem = 0;
                for (int j = 0; j < qtdCores; j++) {
                    if (strcmp(mapa[i].cor, mapa[j].cor) == 0) {
                        jaTem = 1;
                        break;
                    }
                }
                if (!jaTem) {
                    temCor[qtdCores++] = 1;
                }
            }
            int temTodas = 1;
            for (int i = 0; i < qtdCores; i++) {
                int achou = 0;
                for (int j = 0; j < tamanho; j++) {
                    if (strcmp(mapa[j].cor, mapa[i].cor) == 0 && strcmp(mapa[j].cor, corJogador) == 0) {
                        achou = 1;
                        break;
                    }
                }
                if (!achou) {
                    temTodas = 0;
                    break;
                }
            }
            return temTodas;
        }
        case 4: {
            // Ser dono de todos os territórios
            for (int i = 0; i < tamanho; i++)
                if (strcmp(mapa[i].cor, corJogador) != 0)
                    return 0;
            return 1;
        }
        default:
            return 0;
    }
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

// Sorteia uma missão e aloca dinamicamente a string
void atribuirMissao(char** destino, const char* missoes[], int totalMissoes) {
    int sorteada = rand() % totalMissoes;
    *destino = (char*)malloc(strlen(missoes[sorteada]) + 1);
    strcpy(*destino, missoes[sorteada]);
}
