#include <stdio.h>
#include "funcoes.h"


typedef struct{
    int codigo;
    char titulo[50];
    char autor[50];
    int ano;
    int quantidade;
}Livro;

/*Utilitários de terminal*/

void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Descarta tudo que sobrou na linha, inclusive o ENTER */
void limpar_buffer(void) {
    scanf("%*[^\n]");
    scanf("%*c");
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    limpar_buffer();
}

/* Lê um inteiro; retorna -1 se a entrada for inválida */
int ler_opcao(void) {
    int opcao;

    if (scanf("%d", &opcao) != 1)
        opcao = -1;

    limpar_buffer();
    return opcao;
}
/*Funções do sistema (a implementar) */

void cadastrar_livro()        { printf("\n[Cadastrar livro]\n"); }
void cadastrar_usuario()      { printf("\n[Cadastrar usuario]\n"); }
void registrar_emprestimo()   { printf("\n[Registrar emprestimo]\n"); }
void registrar_devolucao()    { printf("\n[Registrar devolucao]\n"); }
void listar_livros()          { printf("\n[Listar livros]\n"); }
void listar_usuarios()        { printf("\n[Listar usuarios]\n"); }
void listar_emprestimos()     { printf("\n[Listar emprestimos]\n"); }
void buscar_por_titulo()      { printf("\n[Buscar livro por titulo]\n"); }
void buscar_por_autor()       { printf("\n[Buscar livro por autor]\n"); }
void buscar_por_matricula()   { printf("\n[Buscar usuario por matricula]\n"); }
void carregar_dados()         { /* ler livros.csv, usuarios.csv, emprestimos.csv */ }
void salvar_dados()           { /* gravar livros.csv, usuarios.csv, emprestimos.csv */ }


/*Menu Principal*/
void menu_principal(void) {
    int opcao;

    do {
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
                printf("\nDados salvos. Encerrando o sistema.\n");
                break;
            default:
                printf("\nOpcao invalida! Digite um numero de 0 a 10.\n");
        }

        if (opcao != 0)
            pausar();

    } while (opcao != 0);
}

    



