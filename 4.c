#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

void generate(void)
{
	srand(time(NULL));
	while(1) 
	{
		int c=rand()%128;
		putchar(c);
	}
}

void main(int argc,char *argv[])
{
	FILE *key;
	FILE *res_f=fopen(argv[3],"w");
	int p_chiper[2];
	int p_file[2];
	int chip,result = 0;
	char data;
	size_t n1,n2;
	
	if(strcmp(argv[1],"1")==0)
	{
		key=fopen("key","w");
	}

	/* Создание труб */
	
	if((pipe(p_chiper)!=0)||(pipe(p_file)!=0))
	{
		printf("Ошибка pipes\n");
		exit(1);
	}

	switch(fork())
	{
		case -1:
			printf("Ошибка fork()\n");
			exit(1);
		case 0:
			close(1);		//Закрыли поток, будем перенаправлять
			dup(p_chiper[1]);	//Перенаправление вывода одного конца трубы
			close(p_chiper[0]);
			close(p_chiper[1]);
			close(p_file[0]);
			close(p_file[1]);
			
			if(strcmp(argv[1],"1")==0)
				generate();		//Данная функция передает сгенерированный ключ
								//На стандартный вывод, коем является p_chiper[1]
			if(strcmp(argv[1],"2")==0)
				execlp("cat","cat","key",NULL);
		default:
			switch(fork())
			{
				case -1:
					printf("Ошибка fork()\n");
					exit(1);
				case 0:
					close(1);
					dup(p_file[1]);		//Перенаправление вывода
					close(p_file[0]);
					close(p_file[1]);
					close(p_chiper[0]);
					close(p_chiper[1]);
					execlp("cat","cat",argv[2],NULL);
				default:
					close(p_file[1]);
					close(p_chiper[1]);
					while((n2=read(p_file[0],&data,1))>0)
					{
						n1=read(p_chiper[0],&chip,1);
						result=data^chip;
						fputc(result,res_f);
						if(strcmp(argv[1],"1")==0)
							fputc(chip,key);
					}
					if (strcmp(argv[1],"1")==0) 
					fclose(key);
					fclose(res_f);
			}
	}
}
