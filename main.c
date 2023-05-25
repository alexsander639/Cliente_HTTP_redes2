/*	Redes II - Cliente HTTP
	Aluno: Alex Sander S. Araujo Junior		RGA: 2019.0743.005-9
	Aluno: Vinícius Silva		        	RGA:
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int criar_socket();

int main(int argc, char const *argv[])
{
	char url[100];
	int socket_desc = criar_socket();

	printf("O socket é: %d\n", socket_desc);

	printf("Informe a URL completa: ");
	scanf("%s", url);
	printf("A url é: %s\n", url);

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
		printf("Não foi possível criar o socket");
		exit(0);
	}
	return socket_novo;
}