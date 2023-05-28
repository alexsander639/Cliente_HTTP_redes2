/*	Redes II - Cliente HTTP
	Aluno: Alex Sander S. Araujo Junior	RGA: 2019.0743.005-9
	Aluno: Vinícius Silva de Paula		RGA:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int criar_socket();
struct sockaddr_in preeche_dados_servidor(struct hostent *host);
char *gerar_requisicao_http(char *endereco_site, char *local_no_site);
FILE *abre_arquivo();
char *remover_header(char *resposta);
int verificar_header(char *resposta);
int verificar_status(int status);

#define MAX_BUFFER_SIZE 4096

int main(int argc, char const *argv[])
{
	char url[100], resposta[MAX_BUFFER_SIZE];
	char *endereco_site, *local_no_site, *requisicao;
	int socket_desc = criar_socket(), verificou_header = 1;
	struct hostent *host;
	struct sockaddr_in server;
	FILE *arquivo;

	if (argc == 2){
		strcpy(url, argv[1]);
	}
	else{
		printf("Informe a URL completa: ");
		scanf("%s", url);
	}

	endereco_site = strtok(url, "http://%99[^/]"); /*Remove o http:// e salva o domínio do site*/
	local_no_site = strtok(NULL, "[^\n]"); /*Salva o recurso específico do domínio*/

	host = gethostbyname(endereco_site);
	server = preeche_dados_servidor(host);
	requisicao = gerar_requisicao_http(endereco_site, local_no_site);

	/*Conectar ao servidor*/
	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
		printf("Falha na conexão com o servidor!\n");
		return 0;
	}
	else{
		printf("Conexão feita com o servidor!\n");
	}

	/*Enviar a requisição para o servidor*/
	if (send(socket_desc, requisicao, strlen(requisicao), 0) < 0){
		printf("Falha ao enviar a requisição!\n");
		return 1;
	}
	else{
		printf("Requisição enviada ao servidor!\n");
	}

	/*Verifica se requisição foi aceita e salva página no arquivo*/
	memset(resposta, '\0', sizeof(resposta));
	while (recv(socket_desc, resposta, (sizeof(resposta) - 1), 0) > 0){
		if (verificou_header){ /*Executado apenas no primeiro loop*/
			verificou_header = verificar_header(resposta);
			strcpy(resposta, remover_header(resposta));
			arquivo = abre_arquivo();
		}
		/*Salvando em arquivo*/
		fprintf(arquivo, "%s", resposta);
		memset(resposta, '\0', sizeof(resposta));
	}

	printf("Página salva no arquivo com sucesso!\n");
	fclose(arquivo);
	return 0;
}

int criar_socket()
{
	/*Primeiro argumento especifica um domínio de comunicação, selecionando a família
de protocolos que será utilizada para comunicação, AF_INET = Protocolo IPv4;
	Segundo argumento define a forma como o socket vai se comunicar,
SOCK_STREAM = cliente e servidor ficam trocando informações até a que a conexão seja encerrada;
	Terceiro argumento define qual protocolo será usado para comunicação,
porém definido o primeiro e segundo argumento, pode deixar o valor em 0*/
	int socket_novo = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_novo == -1){
		printf("Não foi possível criar o socket\n");
		exit(0);
	}
	return socket_novo;
}

struct sockaddr_in preeche_dados_servidor(struct hostent *host)
{
	struct sockaddr_in server;
	/* Detalhes do servidor*/
	server.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*(host->h_addr_list))); /*Endereço IP do servidor*/
	server.sin_family = AF_INET;
	server.sin_port = htons(80); /*Porta HTTP padrão*/

	return server;
}

char *gerar_requisicao_http(char *endereco_site, char *local_no_site)
{
	/*Montar a requisição HTTP*/
	char *requisicao = (char *)malloc(100);
	sprintf(requisicao, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", local_no_site, endereco_site);
	/*GET: obtem um recurso específico do servidor
	Host: especifica o nome de domínio do servidor, o destino da requisição
	Connection: controla a conexão entre o cliente e o servidor. O "close" significa que a conexão será encerrada
depois de receber toda a resposta.*/
	return requisicao;
}

FILE *abre_arquivo()
{
	FILE *arquivo = fopen("PaginaSalva.html", "w");
	if (arquivo == NULL){
		printf("Erro ao criar o arquivo!\n");
		fclose(arquivo);
		exit(0);
	}
	else{
		return arquivo;
	}
}

int verificar_header(char *resposta)
{
	char status_requisicao[MAX_BUFFER_SIZE];
	strcpy(status_requisicao, resposta); /*Copio os primeiros conteúdos recuperados da página*/
	strtok(status_requisicao, "\n"); /*Salvo somente a primeira linha, referente ao status da requisição HTTP*/
	printf("Status da Requisição: %s\n", status_requisicao);
	if (verificar_status(atoi(strtok(status_requisicao, "HTTP/1.1 ")))){ /*Verifica o número desse status*/
		exit(0); /*Caso seja um erro, finaliza a execução do programa*/
	}
	return 0;
}

char *remover_header(char *resposta){
	char *content = strstr(resposta, "\r\n\r\n");
	if (content != NULL){
		content += 4; /*Deslocar em 4 bytes para o início do conteúdo*/
	}
	else{
		content = resposta; /*Se o fim do cabeçalho não foi encontrado, escreva tudo*/
	}
	return content;
}

int verificar_status(int status)
{
	if (status != 200){
		printf("Error!!\n");
		return 1;
	}
	else{
		printf("Requisição aceita com sucesso!\n");
		return 0;
	}
}