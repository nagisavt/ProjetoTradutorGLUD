#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ESTADOS 20
#define MAX_SIMBOLOS 20
#define MAX_TRANSICOES 20

typedef struct {
    int numEstados;
    int numSimbolos;
    char alfabeto[MAX_SIMBOLOS];
    int transicao[MAX_ESTADOS][MAX_SIMBOLOS][MAX_TRANSICOES];
    int numTransicoes[MAX_ESTADOS][MAX_SIMBOLOS];
    int estadoFinal[MAX_ESTADOS];
    int estadoInicial;
} AF_NO;

typedef struct {
    char producao[MAX_ESTADOS * MAX_SIMBOLOS][20];
    int numProducao;
} GLUD_NO;

void iniciaAF(AF_NO *af);
void leituraAF(AF_NO *af);
void printAF(AF_NO *af);
void converteGLUD(AF_NO *af, GLUD_NO *glud);
void printGLUD(GLUD_NO *glud, AF_NO *af);

int main() {
    AF_NO af;
    GLUD_NO glud;

    iniciaAF(&af);
    leituraAF(&af);
    printAF(&af);
    converteGLUD(&af, &glud);
    printGLUD(&glud, &af);

    return 0;
}

void iniciaAF(AF_NO *af) {
    for (int i = 0; i < MAX_ESTADOS; i++) {
        af->estadoFinal[i] = 0;
        for (int j = 0; j < MAX_SIMBOLOS; j++) {
            af->numTransicoes[i][j] = 0;
            for (int k = 0; k < MAX_TRANSICOES; k++) {
                af->transicao[i][j][k] = -1;
            }
        }
    }
}

void leituraAF(AF_NO *af) {
    printf("Informe o numero de estados (max %d): ", MAX_ESTADOS);
    scanf("%d", &af->numEstados);
    if (af->numEstados > MAX_ESTADOS){
        printf("Numero de estados maior que o permitido, convertendo para o valor maximo\n");
        af->numEstados = MAX_ESTADOS;
    }

    printf("Informe o numero de simbolos de entrada (max %d): ", MAX_SIMBOLOS);
    scanf("%d", &af->numSimbolos);
    if (af->numSimbolos > MAX_SIMBOLOS){
        printf("Numero de simbolos maior que o permitido, convertendo para o valor maximo\n");
        af->numSimbolos = MAX_SIMBOLOS;
    }

    printf("Informe os simbolos de entrada: ");
    for (int i = 0; i < af->numSimbolos; i++) {
        scanf(" %c", &af->alfabeto[i]);
    }

    printf("Informe o estado inicial: ");
    scanf("%d", &af->estadoInicial);

    printf("Informe as transacoes (ex: estado simbolo proximoEstado, -1 para parar): \n");
    int est, proxEst;
    char simb;
    while (scanf("%d %c %d", &est, &simb, &proxEst) == 3 && est != -1) {
    if ((est >= 0 && est < af->numEstados) && (simb >= 'a' && simb < 'a' + af->numSimbolos)) {
        int simbIndex = simb - 'a';
        int t = af->numTransicoes[est][simbIndex];
        if (simbIndex < af->numSimbolos && t < MAX_TRANSICOES) {
            af->transicao[est][simbIndex][t] = proxEst;
            af->numTransicoes[est][simbIndex]++;
            printf("Transicao adicionada: De %d, com %c, para %d\n", est, simb, proxEst);
            }
        } else {
            printf("Erro ao declarar transicao, por favor, verifique o estado e simbolo\n");
        }
    }

    printf("Informe os estados finais (-1 para parar): ");
    int f;
    while (scanf("%d", &f) && f != -1) {
        if (f >= 0 && f < af->numEstados) {
            af->estadoFinal[f] = 1;
        }
    }
}

void printAF(AF_NO *af) {
    printf("\nDefinicao formal do Automato Finito a ser traduzido:\n");
    printf("M = (E, Q, y, q0, F)\n");

    printf("E = {");
    for (int i = 0; i < af->numSimbolos; i++) {
        printf("%c", af->alfabeto[i]);
        if (i < af->numSimbolos - 1)
            printf(", ");
    }
    printf("}\n");

    printf("Q = {");
    for (int i = 0; i < af->numEstados; i++) {
        printf("Q%d", i);
        if (i < af->numEstados - 1)
            printf(", ");
    }
    printf("}\n");

    printf("y = {\n");
    for (int i = 0; i < af->numEstados; i++) {
        for (int j = 0; j < af->numSimbolos; j++) {
            if (af->numTransicoes[i][j] > 0) {
                printf("    y(Q%d, %c) -> {", i, af->alfabeto[j]);
                for (int k = 0; k < af->numTransicoes[i][j]; k++) {
                    printf("Q%d", af->transicao[i][j][k]);
                    if (k < af->numTransicoes[i][j] - 1)
                        printf(", ");
                }
                printf("}\n");
            }
        }
    }
    printf("q0 = Q%d\n", af->estadoInicial);

    printf("F = {");
    int prim = 1;
    for (int i = 0; i < af->numEstados; i++) {
        if (af->estadoFinal[i]) {
            if (!prim)
                printf(", ");
            printf("Q%d", i);
            prim = 0;
        }
    }
    printf("}\n");
}

void converteGLUD(AF_NO *af, GLUD_NO *glud) {
    printf("\nConvertendo AF em GLUD....\n");
    glud->numProducao = 0;

    snprintf(glud->producao[glud->numProducao++], sizeof(glud->producao[0]), "S -> Q%d", af->estadoInicial);

    for (int i = 0; i < af->numEstados; i++) {
        for (int j = 0; j < af->numSimbolos; j++) {
            for (int k = 0; k < af->numTransicoes[i][j]; k++) {
                int proxEstado = af->transicao[i][j][k];
                if (proxEstado != -1) {
                    snprintf(glud->producao[glud->numProducao++], sizeof(glud->producao[0]), "Q%d -> %cQ%d ", i, af->alfabeto[j], proxEstado);
                }
            }
        }
        if (af->estadoFinal[i]) {
            snprintf(glud->producao[glud->numProducao++], sizeof(glud->producao[0]), "Q%d -> e", i);
        }
    }
}

void printGLUD(GLUD_NO *glud, AF_NO *af) {
    printf("\nDefinicao formal da Gramatica Linear Unitaria a Direita:\n");
    printf("G = (V, T, P, S)\n");

    printf("V = {S");
    for (int i = 0; i < af->numEstados; i++) {
        printf(", Q%d", i);
    }
    printf("}\n");

    printf("T = {");
    for (int i = 0; i < af->numSimbolos; i++) {
        printf("%c", af->alfabeto[i]);
        if (i < af->numSimbolos - 1)
            printf(", ");
    }
    printf("}\n");

    printf("P = {\n");
    for (int i = 0; i < glud->numProducao; i++) {
        printf("   %s\n", glud->producao[i]);
    }
    printf("}\n");

    printf("S = S\n");
}
