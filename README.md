# ftp-server

Desenvolvido por: Welbert Hime (welberthime@hotmail.com) e Gabriel Lopes (gabriel18.lopes@gmail.com). Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.

Não há necessidade de verificar username, senha e permissões, ou seja, deve-se assumir que o usuário e senha são sempre válidos e que os usuários podem acessar qualquer diretório.

Especificação do protocolo segundo a RFC 959: https://www.ietf.org/rfc/rfc959.txt.

Documentação das principais bibliotecas: [<uninstd.h>](http://pubs.opengroup.org/onlinepubs/007908799/xsh/unistd.h.html), [<sys/socket.h>](https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html).

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
