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
void menu_principal(){};

    



