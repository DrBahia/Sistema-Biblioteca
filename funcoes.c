#include <stdio.h>
#include "funcoes.h"
#include <stdlib.h>
#include <string.h>

#include <time.h>

Livro      *livros      = NULL;
Usuario    *usuarios    = NULL;
Emprestimo *emprestimos = NULL;


/*Utilitários de terminal*/

void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void limpar_buffer() {
    scanf("%*[^\n]");
    scanf("%*c");
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    limpar_buffer();
}

/* Lê um inteiro; retorna -1 se a entrada for inválida */
int ler_opcao() {
    int opcao;
    if (scanf("%d", &opcao) != 1)
        opcao = -1;
    limpar_buffer();
    return opcao;
}


/*Estruturas de dados*/

#define MAX_LIVROS      500
#define MAX_USUARIOS    500
#define MAX_EMPRESTIMOS 1000

#define ARQ_LIVROS      "livros.csv"
#define ARQ_USUARIOS    "usuarios.csv"
#define ARQ_EMPRESTIMOS "emprestimos.csv"

typedef struct {
    int  codigo;
    char titulo[100];
    char autor[100];
    int  ano;
    int  quantidade;
} Livro;

typedef struct {
    char matricula[20];
    char nome[100];
    char curso[100];
} Usuario;

typedef struct {
    int  codigo_livro;
    char matricula[20];
    int  devolvido;     /* 0 = em aberto, 1 = devolvido */
} Emprestimo;

Livro      livros[MAX_LIVROS];
Usuario    usuarios[MAX_USUARIOS];
Emprestimo emprestimos[MAX_EMPRESTIMOS];

int total_livros      = 0;
int total_usuarios    = 0;
int total_emprestimos = 0;

/* ---------- Persistência em arquivos CSV ---------- */
/* Formato: uma linha de cabeçalho e depois um registro por linha,
   com os campos separados por ';'*/


//Formato: Codigo, Titulo, Autor, Ano, Quantidade
void carregar_livros() {
    FILE *arq = fopen(ARQ_LIVROS, "rb");
    if (arq == NULL) return;
    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) == 1 && qtd > 0) {
        livros = (livros == NULL) ? malloc(qtd * sizeof(Livro)) : realloc(livros, qtd * sizeof(Livro));
        if (livros != NULL) total_livros = fread(livros, sizeof(Livro), qtd, arq);
    }
    fclose(arq);
}

//Formato: Matricula, Nome, Curso
void carregar_usuarios() {
    FILE *arq = fopen(ARQ_USUARIOS, "rb");
    if (arq == NULL) return;
    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) == 1 && qtd > 0) {
        usuarios = (usuarios == NULL) ? malloc(qtd * sizeof(Usuario)) : realloc(usuarios, qtd * sizeof(Usuario));
        if (usuarios != NULL) total_usuarios = fread(usuarios, sizeof(Usuario), qtd, arq);
    }
    fclose(arq);
}

//Formato: Codigo, Matricula, Devolvido(0 = em aberto, 1 = devolvido)
void carregar_emprestimos() {
    FILE *arq = fopen(ARQ_EMPRESTIMOS, "rb");
    if (arq == NULL) return;
    int qtd = 0;
    if (fread(&qtd, sizeof(int), 1, arq) == 1 && qtd > 0) {
        emprestimos = (emprestimos == NULL) ? malloc(qtd * sizeof(Emprestimo)) : realloc(emprestimos, qtd * sizeof(Emprestimo));
        if (emprestimos != NULL) total_emprestimos = fread(emprestimos, sizeof(Emprestimo), qtd, arq);
    }
    fclose(arq);
}

void carregar_dados() {
    carregar_livros();
    carregar_usuarios();
    carregar_emprestimos();
}

/* Retornam 1 se salvou, 0 se não conseguiu abrir o arquivo */
int salvar_livros() {
    FILE *arq = fopen(ARQ_LIVROS, "wb");
    if (arq == NULL) return 0;
    fwrite(&total_livros, sizeof(int), 1, arq);
    if (total_livros > 0) fwrite(livros, sizeof(Livro), total_livros, arq);
    fclose(arq);
    return 1;
}

int salvar_usuarios() {
    FILE *arq = fopen(ARQ_USUARIOS, "wb");
    if (arq == NULL) return 0;
    fwrite(&total_usuarios, sizeof(int), 1, arq);
    if (total_usuarios > 0) fwrite(usuarios, sizeof(Usuario), total_usuarios, arq);
    fclose(arq);
    return 1;
}

int salvar_emprestimos() {
    FILE *arq = fopen(ARQ_EMPRESTIMOS, "wb");
    if (arq == NULL) return 0;
    fwrite(&total_emprestimos, sizeof(int), 1, arq);
    if (total_emprestimos > 0) fwrite(emprestimos, sizeof(Emprestimo), total_emprestimos, arq);
    fclose(arq);
    return 1;
}

int salvar_dados() {
    int teste = 1;

    if (!salvar_livros()) {
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_LIVROS);
        teste = 0;
    }
    if (!salvar_usuarios()) {
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_USUARIOS);
        teste = 0;
    }
    if (!salvar_emprestimos()) {
        printf("\nErro: nao foi possivel gravar %s\n", ARQ_EMPRESTIMOS);
        teste = 0;
    }

    return teste;
}

/*Funções do sistema (a fazer)*/

void cadastrar_livro() {
    livros = (livros == NULL)
        ? malloc(sizeof(Livro))
        : realloc(livros, (total_livros + 1) * sizeof(Livro));
    if (livros == NULL) {
        printf("\nErro: Falha de alocacao de memoria.\n");
        return;
    }
    int temp_codigo;
    printf("Codigo numerico do Livro: ");
    scanf("%d", &temp_codigo);

    for (int i = 0; i < total_livros; i++) {
        if (livros[i].codigo == temp_codigo) {
            printf("\nErro: Ja existe um livro registado com esse codigo\n");
            return;
        }
    }

    livros[total_livros].codigo = temp_codigo;

    printf("Titulo: ");
    scanf(" %99[^\n]", livros[total_livros].titulo);

    printf("Autor: ");
    scanf(" %99[^\n]", livros[total_livros].autor);

    printf("Ano de Publicacao: ");
    scanf("%d", &livros[total_livros].ano);

    printf("Quantidade em Estoque: ");
    scanf("%d", &livros[total_livros].quantidade);

    total_livros++;
    salvar_dados();

    printf("\nLivro cadastrado com sucesso.\n");
}
void cadastrar_usuario() {
    printf("\n[Cadastrar usuario]\n");
    usuarios = (usuarios == NULL)
        ? malloc(sizeof(Usuario))
        : realloc(usuarios, (total_usuarios + 1) * sizeof(Usuario));
    if (usuarios == NULL) {
        printf("\nErro: Falha de alocacao de memoria.\n");
        return;
    }
    char temp_matricula[20];
    printf("Matricula (ex: 2026001): ");
    scanf(" %19[^\n]", temp_matricula);

    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].matricula, temp_matricula) == 0) {
            printf("\nErro: Ja existe um aluno registado com essa matricula\n");
            return;
        }
    }

    strcpy(usuarios[total_usuarios].matricula, temp_matricula);

    printf("Nome do aluno: ");
    scanf(" %99[^\n]", usuarios[total_usuarios].nome);

    printf("Curso: ");
    scanf(" %99[^\n]", usuarios[total_usuarios].curso);

    total_usuarios++;
    salvar_dados();

    printf("\nUsuario cadastrado com sucesso\n");
}
void registrar_emprestimo() {
    printf("\n[Registrar emprestimo]\n");
    emprestimos = (emprestimos == NULL)
        ? malloc(sizeof(Emprestimo))
        : realloc(emprestimos, (total_emprestimos + 1) * sizeof(Emprestimo));
    if (emprestimos == NULL) {
        printf("\nErro: Falha de alocacao de memoria.\n");
        return;
    }
    Emprestimo novo;
    int indice_livro = -1;

    printf("Codigo do livro: ");
    scanf("%d", &novo.codigo_livro);

    for (int i = 0; i < total_livros; i++) {
        if (livros[i].codigo == novo.codigo_livro) {
            indice_livro = i;
            break;
        }
    }
    if (indice_livro == -1) {
        printf("Erro: Codigo do livro nao encontrado no sistema.\n");
        return;
    }
    if (livros[indice_livro].quantidade <= 0) {
        printf("Erro: Nao ha exemplares deste livro disponiveis no momento.\n");
        return;
    }

    printf("Matrícula do Utilizador: ");
    scanf(" %19[^\n]", novo.matricula);

    novo.devolvido = 0;
    emprestimos[total_emprestimos] = novo;
    total_emprestimos++;
    livros[indice_livro].quantidade--;
    printf("\nEmprestimo registrado.\n");
}
void registrar_devolucao() {
    printf("\n[Registrar devolucao]\n");
    int cod_livro;
    char mat_aluno[20];
    int encontrado = 0;

    printf("Código do Livro devolvido: ");
    scanf("%d", &cod_livro);

    printf("Matrícula do Utilizador: ");
    scanf(" %19[^\n]", mat_aluno);

    for(int i = 0; i < total_emprestimos; i++) {
        if (emprestimos[i].codigo_livro == cod_livro && 
            strcmp(emprestimos[i].matricula, mat_aluno) == 0 && 
            emprestimos[i].devolvido == 0) {
            emprestimos[i].devolvido = 1;
            encontrado = 1;

            for (int j = 0; j < total_livros; j++) {
                if(livros[j].codigo == cod_livro) {
                    livros[j].quantidade++;
                    break;
                }
            }
            printf("Devolucao cadastrada com sucesso.");
            break;
        }
    }
    if (encontrado == 0){
        printf("Erro, nenhum emprestimo em aberto condizentes com esses dados.");
    }
}


//Funções de listagem
void listar_livros() {
    printf("\n[Listar livros]\n");

    /* Grava o que estiver em memoria para nao perder cadastros recentes */
    salvar_dados();

    /* Zera os contadores antes de recarregar para nao duplicar registros */
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_livros == 0) {
        printf("\nNenhum livro cadastrado em %s.\n", ARQ_LIVROS);
        return;
    }

    printf("\n%-6s | %-35s | %-25s | %-4s | %-5s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Qtd");
    printf("-------+-------------------------------------+---------------------------+------+------\n");

    for (int i = 0; i < total_livros; i++) {
        printf("%-6d | %-35.35s | %-25.25s | %-4d | %-5d\n",
               livros[i].codigo, livros[i].titulo, livros[i].autor,
               livros[i].ano, livros[i].quantidade);
    }

    printf("\nTotal de livros cadastrados: %d\n", total_livros);
}

void listar_usuarios(){
    printf("\n[Listar usuarios]\n");

    /* Grava o que estiver em memoria para nao perder cadastros recentes */
    salvar_dados();

    /* Zera os contadores antes de recarregar para nao duplicar registros */
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_usuarios == 0) {
        printf("\nNenhum usuario cadastrado em %s.\n", ARQ_USUARIOS);
        return;
    }

    printf("\n%-12s | %-35s | %-25s\n",
           "Matricula", "Nome", "Curso");
    printf("-------------+-------------------------------------+---------------------------\n");

    for (int i = 0; i < total_usuarios; i++) {
        printf("%-12s | %-35s | %-25s\n",
            usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso);
    }

    printf("\nTotal de usuarios cadastrados: %d\n", total_usuarios);
}


void listar_emprestimos(){
    printf("\n[Listar emprestimos]\n");

    /* Grava o que estiver em memoria para nao perder cadastros recentes */
    salvar_dados();

    /* Zera os contadores antes de recarregar para nao duplicar registros */
    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    if (total_emprestimos == 0) {
        printf("\nNenhum emprestimo cadastrado em %s.\n", ARQ_EMPRESTIMOS);
        return;
    }

    
    printf("\n%-16s | %-12s | %-11s\n",
           "Codigo do livro", "Matricula", "Devolucao");
    printf("-----------------+-------------+-------------\n");
    for (int i = 0; i < total_emprestimos; i++) {
        char *status = (emprestimos[i].devolvido == 1) ? "FEITA" : "PENDENTE";
        printf("%-16d | %-12s | %-11s\n",
            emprestimos[i].codigo_livro, emprestimos[i].matricula, status);
    }

    printf("\nTotal de emprestimos cadastrados: %d\n", total_emprestimos);
}

//Funções de busca
void buscar_por_titulo(){
    printf("\n[Buscar livro por titulo]\n"); 
    salvar_dados();

    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char titulo[100];
    printf("Titulo:\n");
    scanf("%99[^\n]", &titulo);
    limpar_buffer();

    for (int i =0; i<total_livros- 1; i++) {
        for (int j=0; j <total_livros -i- 1; j++) {
            if (strcmp(livros[j].titulo, livros[j + 1].titulo) > 0) {
                unsigned char temp[sizeof(livros[0])];
                memcpy(temp, &livros[j], sizeof(livros[0]));
                livros[j] = livros[j + 1];
                memcpy(&livros[j + 1], temp, sizeof(livros[0]));
            }
        }
    }
    int inicio =0;
    int fim =total_livros- 1;
    int encontrado = -1;
    while (inicio<= fim) {
        int meio= inicio + (fim - inicio) / 2;
        int cmp =strcmp(livros[meio].titulo, titulo);
        if (cmp==0) {
            encontrado = meio;
            break;
        } else if (cmp < 0) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    int n = 0;
    printf("\n%-6s | %-35s | %-25s | %-4s | %-5s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Qtd");
    printf("-------+---------------------------+---------------------------+------------\n");
}


void buscar_por_autor()       {
    printf("\n[Buscar livro por autor]\n"); 
    salvar_dados();

    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char autor[100];
    printf("Titulo:\n");
    scanf("%99[^\n]", &autor);
    limpar_buffer();

    for (int i=0;i< total_livros-1; i++) {
        for (int j=0; j< total_livros -i- 1; j++) {
            if (strcmp(livros[j].autor, livros[j+1].autor) > 0) {
                Livro temp = livros[j];
                livros[j] = livros[j+1];
                livros[j+1] = temp;
            }
        }
    }
    int inicio = 0;
    int fim = total_livros - 1;
    int encontrado = -1;

    while (inicio <= fim) {
        int meio = inicio + (fim-inicio) / 2;
        int cmp = strcmp(livros[meio].autor, autor);

        if (cmp == 0) {
            encontrado = meio;
            break;
        } else if (cmp < 0) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    int n = 0;
    printf("\n%-6s | %-35s | %-25s | %-4s | %-5s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Qtd");
    printf("-------+-------------------------------------+---------------------------+------+------\n");
    if (encontrado != -1) {
        // Volta para o primeiro livro caso o autor tenha mais de um livro cadastrado
        while (encontrado > 0 && strcmp(livros[encontrado - 1].autor, autor) == 0) {
            encontrado--;
        }
        for (int i = encontrado; i < total_livros && strcmp(livros[i].autor, autor) == 0; i++) {
            printf("%-6d | %-35.35s | %-25.25s | %-4d | %-5d\n",
                   livros[i].codigo, livros[i].titulo, livros[i].autor,
                   livros[i].ano, livros[i].quantidade);
            n++;
        }
    }

    if (n == 0) {
        printf("Nenhum livro encontrado\n");
    }
}
void buscar_por_matricula()   { 
    printf("\n[Buscar usuario por matricula]\n");
    salvar_dados();

    total_livros      = 0;
    total_usuarios    = 0;
    total_emprestimos = 0;
    carregar_dados();

    char matricula[100];
    printf("Matricula:\n");
    scanf("%99[^\n]", &matricula);
    limpar_buffer();

    for (int i=0;i< total_usuarios-1; i++) {
        for (int j=0; j< total_usuarios -i- 1; j++) {
            if (strcmp(usuarios[j].matricula, usuarios[j+1].matricula) > 0) {
                Usuario temp = usuarios[j];
                usuarios[j] = usuarios[j + 1];
                usuarios[j + 1] = temp;
            }
        }
    }
    int inicio = 0;
    int fim = total_usuarios - 1;
    int encontrado = -1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        int cmp = strcmp(usuarios[meio].matricula, matricula);

        if (cmp == 0) {
            encontrado = meio;
            break;
        } else if (cmp < 0) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    printf("\n%-12s | %-35s | %-25s\n",
           "Matricula", "Nome", "Curso");
    printf("-------------+-------------------------------------+---------------------------\n");

    if (encontrado != -1) {
        printf("%-12s | %-35s | %-25s\n",
               usuarios[encontrado].matricula,
               usuarios[encontrado].nome,
               usuarios[encontrado].curso);
    } else {
        printf("Nenhum usuario encontrado\n");
    }  
}





/*Menu Principal*/
void menu_principal(){
    int opcao = -1;
    while (opcao != 0){
        limpar_tela();
        printf("          SISTEMA DE BIBLIOTECA\n");
        printf("=============================================\n");
        printf("  CADASTROS\n");
        printf("   1 - Cadastrar livro\n");
        printf("   2 - Cadastrar usuario\n");
        printf("   3 - Registrar emprestimo\n");
        printf("   4 - Registrar devolucao\n");
        printf("---------------------------------------------\n");
        printf("  LISTAGENS\n");
        printf("   5 - Listar livros\n");
        printf("   6 - Listar usuarios\n");
        printf("   7 - Listar emprestimos\n");
        printf("---------------------------------------------\n");
        printf("  BUSCAS\n");
        printf("   8 - Buscar livro por titulo\n");
        printf("   9 - Buscar livro por autor\n");
        printf("  10 - Buscar usuario por matricula\n");
        printf("---------------------------------------------\n");
        printf("   0 - Salvar e sair\n");
        printf("=============================================\n");
        printf("Escolha uma opcao: ");

        opcao = ler_opcao();

        switch (opcao) {
            case 1:  cadastrar_livro();      break;
            case 2:  cadastrar_usuario();    break;
            case 3:  registrar_emprestimo(); break;
            case 4:  registrar_devolucao();  break;
            case 5:  listar_livros();        break;
            case 6:  listar_usuarios();      break;
            case 7:  listar_emprestimos();   break;
            case 8:  buscar_por_titulo();    break;
            case 9:  buscar_por_autor();     break;
            case 10: buscar_por_matricula(); break;
            case 0:
                salvar_dados();
                free(livros);
                free(usuarios);
                free(emprestimos);
                printf("\nDados salvos. Encerrando o sistema\n");
                break;
            default:
                printf("\nOpcao invalida! Digite um numero de 0 a 10.\n");
        }

        if (opcao != 0)
            pausar();

    };
}





