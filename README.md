# Sistema de Biblioteca

Trabalho da disciplina de Programação Estruturada (UNICAP — 2026.2).

## Integrantes

- João Gomes de Barros Carnaúba
- João Pedro de Almeida Santos

## Estrutura do projeto

| Arquivo     | Descrição                                                          |
|-------------|--------------------------------------------------------------------|
| `main.c`    | Ponto de entrada: carrega os dados, abre o menu e libera a memória |
| `funcoes.c` | Implementação de todas as funcionalidades do sistema               |
| `funcoes.h` | Declarações das funções                                            |

Os dados são salvos em arquivos binários (`livros.dat`, `usuarios.dat` e `emprestimos.dat`), criados automaticamente na pasta onde o programa é executado. 

## Requisitos

- Compilador C — recomendado o **GCC**
  -[MSYS2](https://www.msys2.org/) (pacote `mingw-w64-ucrt-x86_64-gcc`) ou MinGW


Para conferir se o GCC está instalado:

```sh
gcc --version
```

## Compilação

Abra um terminal na pasta do projeto e compile **os dois arquivos `.c` juntos**:

```sh
gcc main.c funcoes.c -o biblioteca.exe
```


## Execução


```sh
.\biblioteca.exe
```

