# proxy_server_-TR2-
Intrucões para executar:
	
	*Criar uma pasta e, dentro dela, colocar a  pasta proxy_server_-TR2-

	*Na pasta criada, colocar o comando no terminal: cmake proxy_server_-TR2-

	*Após isso, digitar no terminal: make
	
	*Caso esteja utilizando o firefox:
		*Achar opção para configurar o proxy
		
		*Nela, escolher configuração manual do proxy

		*Na parte de HTTP proxy:
			
			*locaslhost para IP 

			*Porta utilizada no proxy para número de porta

		*Clicar em OK

	*Para executar, ir no temrinal na pasta criada e jogar o comando: ./proxy_server numero_porta 


Estrutura do package:


	->client.c

	->client.h

	->common.c: source (fonte) para definir as coisas comuns entre todos os outros módulos

	->common.h: header(cabeçalho) para definir as coisas comuns entre todos os outros módulos

	->server.c

	->server.h

	->main.c

	->LICENSE: arquivo de licença de uso ( GNU GENERAL PUBLIC LICENSE)

	->CMakeList.txt: Arquivo cmake para compilar o projeto

	->Especificações_Trabalho_TR2.pdf:

	->README.md: Este arquivo


Checklist:

	[X] Repasse - funcionando mas não é otimizado.
	
	[~] Blacklist, whitelist, termos banidos - em desenvolvimento.
	
	[~] Mensagens de resposta do servidor em caso de erro - em desenvolvimento.
	
	[ ] Cache
	
	[ ] Interface para a inspeção e modificação dos campos do cabeçalho HTTP
	
	[ ] Retirada de funções que não estão sendo utilizadas
	
	[ ] Modularização

