# ftp-server

Desenvolvido por: Welbert Hime (welberthime@hotmail.com) e Gabriel Lopes (gabriel18.lopes@gmail.com). Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.

Não há necessidade de verificar username, senha e permissões, ou seja, deve-se assumir que o usuário e senha são sempre válidos e que os usuários podem acessar qualquer diretório.

Especificação do protocolo segundo a RFC 959: https://www.ietf.org/rfc/rfc959.txt.

Documentação das principais bibliotecas: [<uninstd.h>](http://pubs.opengroup.org/onlinepubs/007908799/xsh/unistd.h.html), [<sys/socket.h>](https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html).

# Instalação e execução

* Compilando programa do cliente: gcc client*.c -o client.out.

* Compilando programa do servidor: gcc server*.c -o server.out.

Dessa forma os executáveis gerados são ignorados pelo gitignore.

Para execução do cliente e servidor, o servidor deve ser iniciado primeiro, evitando um "421 Service not available, closing control connection." por parte do cliente.

Intruções parcialmente implementadas: USER, PASS, ACCT, CWD, CDUP, SMNT, REIN, QUIT.

# FTP commands

| Access control commands | Transfer parameter commands | Ftp service commands |
| ----------------------- | --------------------------- | -------------------- |
| USER (0) | PORT | RETR |
| PASS (1) | PASV | STOR |
| ACCT (2) | TYPE | STOU |
| CWD (3) | STRU | APPE |
| CDUP (4) | MODE | ALLO |
| SMNT (5) | | REST |
| REIN (6) | | RNFR|
| QUIT (7) | | RNTO |
| | | ABOR |
| | | DELE |
| | | RMV |
| | | MKD |
| | | PWD |
| | | LIST |
| | | NLST |
| | | SITE |
| | | SYST |
| | | STAT |
| | | HELP |
| | | NOOP |
