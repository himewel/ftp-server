# ftp-server

Desenvolvido por: Welbert Hime (welberthime@hotmail.com) e Gabriel Lopes (gabriel18.lopes@gmail.com). Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.

Não há necessidade de verificar username, senha e permissões, ou seja, deve-se assumir que o usuário e senha são sempre válidos e que os usuários podem acessar qualquer diretório.

Especificação do protocolo segundo a RFC 959: https://www.ietf.org/rfc/rfc959.txt.

Documentação das principais bibliotecas: [<uninstd.h>](http://pubs.opengroup.org/onlinepubs/007908799/xsh/unistd.h.html), [<sys/socket.h>](https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html).

# FTP commands

| Access control commands | Transfer parameter commands | Ftp service commands |
| ----------------------- | --------------------------- | -------------------- |
| USER | PORT | RETR |
| PASS | PASV | STOR |
| ACCT | TYPE | STOU |
| CWD | STRU | APPE |
| CDUP | MODE | ALLO |
| SMNT | | REST |
| REIN | | RNFR|
| QUIT | | RNTO |
| PASS | | ABOR |
| ACCT | | DELE |
| CWD | | RMV |
| CDUP | | MKD |
| SMNT | | PWD |
| REIN | | LIST |
| QUIT | | NLST |
| | | SITE |
| | | SYST |
| | | STAT |
| | | HELP |
| | | NOOP |
