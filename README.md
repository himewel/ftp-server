# ftp-server

Desenvolvido por: Welbert Hime (welberthime@hotmail.com) e Gabriel Lopes (gabriel18.lopes@gmail.com). Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.

Não há necessidade de verificar username, senha e permissões, ou seja, deve-se assumir que o usuário e senha são sempre válidos e que os usuários podem acessar qualquer diretório.

Especificação do protocolo segundo a RFC 959: https://www.ietf.org/rfc/rfc959.txt.

## Instalação e execução

Dê as permissões necessárias para execução do compile.sh e o execute.

Para execução do cliente e servidor, o servidor deve ser iniciado primeiro, evitando um "421 Service not available, closing control connection.".

Se não especificado por argumentos, a interface padrão para início do servidor é a loopback, enquanto a porta para canal de controle é a 2300.

Para especificação da interface e porta desejadas, deve se seguir a seguinte sintaxe:

    ./compile.sh [interface de rede] [porta desejada]

O script utiliza o GCC como compilador padrão seguindo o seguinte comando:

    gcc server*.c -o server.out

Caso se deseje utilizar diretamente o arquivo compilado:

    ./server.out [interface de rede] [porta desejada]



## Exemplos de utilização

Utilizando script de compilação e especificando interface de rede e porta desejada:

    $ ./compile.sh wlp2s0 2300
    Compilando...
    Gotcha!
    --------------------------------------------------------------------------------
    Info: Interface selecionada: wlp2s0.
    Info: Utilizando interface selecionada.
    Info: Porta selecionada: 2300.
    Info: Socket criado com sucesso.
    Info: Rodando servidor em: 192.168.1.166:2300.
    --------------------------------------------------------------------------------

Utilizando diretamente arquivo compila pelo gcc e executando servidor com interface e porta padrões:

    $ ./server.out
    --------------------------------------------------------------------------------
    Info: Interface não informada, utilizando interface padrão: lo.
    Info: Porta padrão selecionada: 2300.
    Info: Socket criado com sucesso.
    Info: Rodando servidor em: 127.0.0.1:2300.
    --------------------------------------------------------------------------------



## Minimum Implementation

In order to make FTP workable without needless error messages, the
following minimum implementation is required for all servers:

    TYPE - ASCII Non-print
    MODE - Stream
    STRUCTURE - File, Record
    COMMANDS - USER, QUIT, PORT,
          TYPE, MODE, STRU,
            for the default values
          RETR, STOR,
          NOOP.

The default values for transfer parameters are:

    TYPE - ASCII Non-print
    MODE - Stream
    STRU - File

## FTP commands

| Access control commands | Transfer parameter commands | Ftp service commands |
| ----------------------- | --------------------------- | -------------------- |
| USER (0) | PORT (14) | RETR (17) |
| PASS (1) | PASV (16) | STOR (18) |
| ACCT (2) | TYPE (15) | STOU |
| CWD (3) | STRU | APPE |
| CDUP (4) | MODE | ALLO |
| SMNT (5) | | REST |
| REIN (6) | | RNFR|
| QUIT (7) | | RNTO |
| | | ABOR |
| | | DELE |
| | | RMD (11)|
| | | MKD (10)|
| | | PWD (9)|
| | | LIST (8) |
| | | NLST |
| | | SITE |
| | | SYST (13) |
| | | STAT |
| | | HELP |
| | | NOOP (12) |
