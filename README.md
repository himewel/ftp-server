# ftp-server

Desenvolvido por: Welbert Hime (welberthime@hotmail.com) e Gabriel Lopes (gabriel18.lopes@gmail.com). Este projeto foi desenvolvido integralmente pela equipe, sem ajuda não autorizada de alunos não membros do projeto no processo de codificação.

Não há necessidade de verificar username, senha e permissões, ou seja, deve-se assumir que o usuário e senha são sempre válidos e que os usuários podem acessar qualquer diretório.

Especificação do protocolo segundo a RFC 959: https://www.ietf.org/rfc/rfc959.txt.

## Instalação e execução

Dê as permissões necessárias para execução do compile.sh e o execute.

Para execução do cliente e servidor, o servidor deve ser iniciado primeiro, evitando um "421 Service not available, closing control connection.".

Se não especificado por argumentos, a interface padrão para início do servidor é a loopback, enquanto a porta para canal de controle é a 2300, já o número de conexões simultâneas é definido como 20 e a cota máxima de transmissão do servidor fica definida como 2000000 B (2 MB).

Para especificação da interface e porta desejadas, deve se seguir a seguinte sintaxe:

    ./compile.sh [interface de rede] [porta desejada] [número de threads/conexões] [cota máxima de transmissão do servidor]

O script utiliza o GCC como compilador padrão seguindo o seguinte comando:

    gcc server*.c -o server.out -lpthread

Caso se deseje utilizar diretamente o arquivo compilado:

    ./server.out [interface de rede] [porta desejada] [número de threads/conexões] [cota máxima de transmissão do servidor]

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
    Info: Utilizando limite total da taxa de transmissão padrão: 2000000.
    Info: Socket criado com sucesso.
    Info: Arquivo de configuração não encontrado, utilizando taxa padrão para todos os usuários.
    Info: Rodando servidor em: 192.168.1.166:2300.
    --------------------------------------------------------------------------------

Utilizando diretamente arquivo compila pelo gcc e executando servidor com interface, porta e número de conexões padrão:

    $ ./server.out
    --------------------------------------------------------------------------------
    Info: Interface não informada, utilizando interface padrão: lo.
    Info: Porta padrão selecionada: 2300.
    Info: Número máximo de clientes conectados simultaneamente não informado, utilizando valor padrão: 20.
    Info: Utilizando limite total da taxa de transmissão padrão: 2000000.
    Info: Socket criado com sucesso.
    Info: Arquivo de configuração não encontrado, utilizando taxa padrão para todos os usuários.
    Info: Rodando servidor em: 127.0.0.1:2300.
    --------------------------------------------------------------------------------

Ao conectar com um cliente, uma mensagem com o endereço e porta do cliente será exibida, assim como o número atualizado de conexões servidas:

    --------------------------------------------------------------------------------
    Info: Conexão estabelecida com: 127.0.0.1:48830.
    Info: Número de conexões atualmente: 1.
    Info: Taxa de transmissão reservada/Taxa máxima definida: 100000/2000000.
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

Outros comandos que trazem uma melhor experiência do usuário também foram implementados, como o LIST, CD, CDUP, PWD, MKD, RMD e SYST.

## QoS

Na ausência de um arquivo nomeado *config.ini* especificando taxas para os endereços IP específicos, todos os clientes recebem uma taxa padrão definida pela razão entre cota de transmissão do servidor e o número máximo de clientes.

Ao realizar uma transferência uma mensagem semelhante essa será exibida com o tamanho do arquivo, o tempo de processamento e relação entre o tamanho e o tempo de processamento.

    --------------------------------------------------------------------------------
    Recv: RETR config.ini
    Info: Arquivo selecionado: config.ini.
    Send: 150 File status okay; about to open data connection.
    Info: Tamanho do arquivo: 73B.
    Info: Tempo de processamento: 7.001747s.
    Info: Taxa de processamento: 10.43B/s.
    Send: 250 Requested file action okay, completed.
    --------------------------------------------------------------------------------

Nesse exemplo, a taxa de transmissão para o cliente foi 10 B. Distorções entre o tempo e taxas exibidas pelo cliente FTP são atribuídas ao fato do cálculo do tempo também considerar comandos que não tem efeito na rede, como manipulação de contadores ou o processo disputando processamento.

Distorções maiores ocorrem no STOR, aparentemente o cliente envia todos os dados de uma vez, porém o servidor não efetua o processamento dos dados da mesma maneira.

### Configuração

Todos os dados referentes a transferência de dados, mesmo não exibindo a unidade em todas mensagens de log, estão em Bytes. O arquivo de configuração tem suporte as grandezas GB, MB, KB e B, porém estes são descritos apenas com a primeira letra após um espaço da taxa definida.

    172.16.14.88 1 M
    172.16.14.55 2 M

Clientes com taxas que superem a cota máxima do servidor tem sua conexão rejeitada. Só são aceitas conexões em que o cliente tem sua taxa de transmissão garantida.
