#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "driver_so.h"

void ioctl_set_position(int descritor, char * mensagem)
{
	int ret;

	ret = ioctl(descritor, IOCTL_SET_POSITION, mensagem);

	if (ret < 0)
	{
		printf("IOCTL_SET_POSITION erro\n");
		exit(-1);
	}
}

void ioctl_get_position(int descritor)
{
	int ret;
	char mensagem[BUF_MSG];

	ret = ioctl(descritor, IOCTL_GET_POSITION, mensagem);

	if (ret < 0)
	{
		printf("IOCTL_GET_POSITION erro\n");
		exit(-1);
	}

	printf("get position: %s\n", mensagem);
}

int main(int argc, char **argv) 
{
	int fp, ret;

	fp = open("/dev/so", O_RDWR);

	if (fp < 0)
	{
		perror("Nao foi possivel acessar\n");
		exit(0);
	}

	//char * msg = "12234\n";
	//ioctl_set_position(fp, msg);
	ioctl_set_position(fp, argv[1]);
	ioctl_get_position(fp);

	close(fp);
} 
