/*	Redes II - Cliente HTTP
	Aluno: Alex Sander S. Araujo Junior		RGA: 2019.0743.005-9
	Aluno: Vinícius Silva		        	RGA:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int criar_socket();
struct sockaddr_in preeche_dados_servidor(struct hostent *host);
char *gerar_requisicao_http(char* endereco_site, char* local_no_site);

int main(int argc, char const *argv[])
{
	char url[100];
	char *endereco_site, *local_no_site, *requisicao;
	int socket_desc = criar_socket();
	struct hostent *host;
	struct sockaddr_in server;

	printf("Informe a URL completa: ");
	scanf("%s", url);

	endereco_site = strtok(url,"http://%99[^/]"); /*Remove o http:// e salva o endereço do site*/
    local_no_site = strtok(NULL,"[^\n]"); /*Salva onde exatamente no site*/

	printf("Site: %s\nLocal no site: %s\n", endereco_site, local_no_site);

	host = gethostbyname(endereco_site);

	printf("IP: %s \n", inet_ntoa(*(struct in_addr*)host->h_addr_list));
	printf("O socket é: %d\n", socket_desc);

	server = preeche_dados_servidor(host);
	
	printf("Server: %d \n", server.sin_addr.s_addr);

	requisicao = gerar_requisicao_http(endereco_site, local_no_site);
	printf("Requisição: \n%s\n", requisicao);

	/*Conectar ao servidor*/
	if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Falha na conexão com o servidor\n");
        return 0;
    }
	
	return 0;
}

int criar_socket()
{
	int socket_novo = socket(AF_INET, SOCK_STREAM, 0); /*Primeiro argumento especifica um domínio de comunicação,
														selecionando a família de protocolos que será utilizada
														para comunicação, AF_INET = Protocolo IPv4;
														Segundo argumento define a forma como o socket vai se
														comunicar, SOCK_STREAM = cliente e servidor ficam
														trocando informações até a que a conexão seja encerrada;
														Terceiro argumento define qual protocolo será usado
														para comunicação, porém definido o primeiro e segundo
														argumento, pode deixar o valor em 0*/
	if (socket_novo == -1)
	{
		printf("Não foi possível criar o socket\n");
		exit(0);
	}
	return socket_novo;
}

struct sockaddr_in preeche_dados_servidor(struct hostent *host)
{
	struct sockaddr_in server;
	/* Definir os detalhes do servidor*/
	server.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)host->h_addr_list)); /*Endereço IP do servidor*/
	server.sin_family = AF_INET;
	server.sin_port = htons(80); /*Porta HTTP padrão*/
	return server;
}

char *gerar_requisicao_http(char* endereco_site, char* local_no_site)
{
	/*Monta a requisição HTTP*/
    char *requisicao = (char*)malloc(100);
	
	sprintf(requisicao,"GET / %s HTTP/1.1\r\nHost: %s\r\n\r\n",
			 local_no_site, endereco_site);

	return requisicao;
}

