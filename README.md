# ftp-server

Desenvolvido por: Welbert Hime (welberthime@hotmail.com) e Gabriel Lopes (gabriel18.lopes@gmail.com). Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.

Não há necessidade de verificar username, senha e permissões, ou seja, deve-se assumir que o usuário e senha são sempre válidos e que os usuários podem acessar qualquer diretório.

Especificação do protocolo segundo a RFC 959: https://www.ietf.org/rfc/rfc959.txt.

## Instalação e execução

Dê as permissões necessárias para execução do compile.sh e o execute.

Para execução do cliente e servidor, o servidor deve ser iniciado primeiro, evitando um "421 Service not available, closing control connection.".

Se não especificado por argumentos, a interface padrão para início do servidor é a loopback, enquanto a porta para canal de controle é a 2300, já o número de conexões simultâneas é definido como 20.

Para especificação da interface e porta desejadas, deve se seguir a seguinte sintaxe:

    ./compile.sh [interface de rede] [porta desejada] [número de threads/conexões]

O script utiliza o GCC como compilador padrão seguindo o seguinte comando:

    gcc server*.c -o server.out

Caso se deseje utilizar diretamente o arquivo compilado:

    ./server.out [interface de rede] [porta desejada] [número de threads/conexões]

## Exemplos de utilização

Utilizando script de compilação e especificando interface de rede, porta para conexão e número máximo de conexões:

    $ ./compile.sh wlp2s0 2300 5
    Compilando...
    Gotcha!
    --------------------------------------------------------------------------------
    Info: Interface selecionada: wlp2s0.
    Info: Utilizando interface selecionada.
    Info: Porta selecionada: 2300.
    Info: Número máximo de clientes conectados simultaneamente: 5.
    Info: Socket criado com sucesso.
    Info: Rodando servidor em: 192.168.1.166:2300.
    --------------------------------------------------------------------------------

Utilizando diretamente arquivo compila pelo gcc e executando servidor com interface, porta e número de conexões padrão:

    $ ./server.out
    --------------------------------------------------------------------------------
    Info: Interface não informada, utilizando interface padrão: lo.
    Info: Porta padrão selecionada: 2300.
    Info: Número máximo de clientes conectados simultaneamente não informado, utilizando valor padrão: 20.
    Info: Socket criado com sucesso.
    Info: Rodando servidor em: 127.0.0.1:2300.
    --------------------------------------------------------------------------------

Ao conectar com um cliente, uma mensagem com o endereço e porta do cliente será exibida, assim como o número atualizado de conexões servidas:

    --------------------------------------------------------------------------------
    Info: Conexão estabelecida com: 127.0.0.1:48830.
    Info: Número de conexões atualmente: 1.
    Send: 220 Service ready for new user.
    --------------------------------------------------------------------------------

## Minimum Implementation

In order to make FTP workable without needless error messages, the following minimum implementation is required for all servers:

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

## Comandos implementados

Todos os comandos listados na implementação mínima e seus valores padrão foram implementados, enquanto para o restante dos comandos listados na RFC 959, é exibida a seguinte mensagem:

    202 Command not implemented, superfluous at this site.

Outros comandos que trazem uma experiência do usuário também foram implementados, como o LIST, CD, CDUP, PWD, MKD, RMD e SYST.
