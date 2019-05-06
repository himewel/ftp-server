# ftp-server

Desenvolvido por: Welbert Hime (welberthime@hotmail.com) e Gabriel Lopes (gabriel18.lopes@gmail.com). Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.

Não há necessidade de verificar username, senha e permissões, ou seja, deve-se assumir que o usuário e senha são sempre válidos e que os usuários podem acessar qualquer diretório.

Especificação do protocolo segundo a RFC 959: https://www.ietf.org/rfc/rfc959.txt.

Documentação das principais bibliotecas: [<uninstd.h>](http://pubs.opengroup.org/onlinepubs/007908799/xsh/unistd.h.html), [<sys/socket.h>](https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html).

## Instalação e execução

Dê as permissões necessárias para execução do compile.sh e o execute.

Para execução do cliente e servidor, o servidor deve ser iniciado primeiro, evitando um "421 Service not available, closing control connection.".

A porta padrão utilizada é a 2300, a conexão inicialmente é realizada pelo localhost.

Intruções parcialmente implementadas: USER, PASS, ACCT, CWD, CDUP, SMNT, REIN, QUIT, PWD, LIST, MKD, RMD, NOOP.

Instruções adaptadas para o cliente FTP padrão: USER, PASS, CWD, CDUP, PWD, LIST, SYST, PORT.

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
| USER (0) | PORT (14) | RETR |
| PASS (1) | PASV | STOR |
| ACCT (2) | TYPE | STOU |
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
