#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <signal.h>
#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_NUM_BACKGROUND 64
int p=-1;
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

int backgroundprocessreaping(int a[],int l)
{
 int i,b[MAX_NUM_BACKGROUND],id,k=0;
 for(i=0;i<l;i++)
 {
  id=waitpid(a[i],NULL,WNOHANG);
  if(id!=0)
  {
   printf("Shell: Background process finished\n");
  }
  else
  {
   b[k]=a[i];
   k=k+1;
  }
 }
 for(i=0;i<k;i++)
 {
  a[i]=b[i];
 }
 return k;
}

void changedirectory(char* tokens[])
{
 if(tokens[2]!=NULL)
 {
  printf("Shell: Incorrect command\n");
 }
 else
 {
  int x=chdir(tokens[1]);
  if(x!=0)
  {
   printf("Shell: Incorrect command\n");
  }
 }
}

void exitcommand(int a[],int l)
{
 int i,id;
 for(i=0;i<l;i++)
 {
  id=waitpid(a[i],NULL,WNOHANG);
  if(id<=0)
  {
   kill(a[i],SIGINT);
  }
 }
 return;
}

void signalinterrupt()
{
 kill(-p,SIGINT);
 return;
}
int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i,bg=0,bgp[MAX_NUM_BACKGROUND],j=0;
        signal(SIGINT,signalinterrupt);

	while(1){
	        j=backgroundprocessreaping(bgp,j);
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		bg=0;
		int a=strlen(line)-1;
               if((line[a])=='&')
               {
                if(j==64)
                {
                 printf("Can't create background process\n");
                }
                else
                {
                 bg=1;
                 line[a-1] = '\n';
                 line[a]='\0';
                }
               }
               else
               {
                line[strlen(line)]='\n';
               }
		tokens = tokenize(line);
               if(tokens[0]==NULL)
               {
                continue;
               }
               if(strcmp(tokens[0],"cd")==0)
               {
                changedirectory(tokens);
               }
               else if(strcmp(tokens[0],"exit")==0)
               {
                exitcommand(bgp,j);
                for(i=0;tokens[i]!=NULL;i++)
	        {
		 free(tokens[i]);
	        }
	        free(tokens);
	        return 0;
               }
               else
               {
                int fc=fork();
                if(fc<0)
                {
                 fprintf(stderr,"%s\n","unable to create child process\n");
                }
                else if(fc==0)
                {
                 execvp(tokens[0],tokens);
                 printf("Command execution failed\n");
                 exit(1);
                }
                else
                {
                 if(bg==0)
                 {
                   setpgid(fc,fc);
                   p=getpgid(fc);
                   waitpid(fc,NULL,0);
                 }
                 else
                 {
                  setpgid(fc,fc);
                  bgp[j]=fc;
                  j=j+1;
                  printf("[%d]\t%d\n",j,fc);	
                 }
                }
               }
	       for(i=0;tokens[i]!=NULL;i++)
	       {
		free(tokens[i]);
	       }
	      free(tokens);
	     }
	return 0;
}
