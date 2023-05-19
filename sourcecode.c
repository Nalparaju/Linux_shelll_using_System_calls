include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
//Batch mode function
int batch(int argc, char* argv[])
{
int f;
char *fn;
for(int i=1;i<argc;i++) //Loop the argument from second one
{
fn=argv[i]; //Store it in a pointer variable called "fn"
f = open(fn, O_RDONLY);
if(f == -1)
{
printf("Could not open file or invalid filename\n"); //Checking if file is existed if not print error and comeout of the program
return 0;
}
}
//Intialising the variables to store buffer for reading lines, array to store commands, current command counter, to store process id, flag for quit and command count
char line[512];
char *commands[100];
int i, status;
pid_t pid;
int quitTrue=0;
int comTotal = 0;
printf("Lines in file: \n"); //This line prints the lines of commands from the input file

int r=read(f,line,sizeof(line));
while(r>0)
{
printf("%.*s",r,line);
r=read(f,line,sizeof(line));
}
close(f);
f = open(fn, O_RDONLY);
while(r = read(f,line,sizeof(line))>0)
{
commands[comTotal] = strtok(line,";\n"); //Tokenizing the commands using delimiter for aindvidual commands
while (commands[comTotal]!=NULL) //itterate till it reaches end
{
comTotal++;
commands[comTotal]=strtok(NULL,";\n"); //Tokenize to pass as a individual arguments
}

for(i=0;i<comTotal;i++) //each argument is looped
{
pid = fork(); //child to execute current command

if(pid == 0)
{
char *argv[100]; //to store arguments for command
int j=0;
argv[j]=strtok(commands[i], " "); //tokenize commands to arguments
while (argv[j]!=NULL)
{
j++;
argv[j]=strtok(NULL," "); //Tokenize to separate arguments abd check execvp sends negative - if recives say that command notfound
}
if(execvp(argv[0],argv)== -1)
{
printf("Command not found: %s\n",commands[i]);
exit(1);
}
}
else
{  //This is Parent Process and wait for child process to complete
waitpid(pid,&status,0); 
if(strcmp(commands[i], "quit")==0) //when current command is true raise the flag and exit after all commands are executed and close the file
{
quitTrue=1;
}
}
}
if(quitTrue)
{
break;
}
}
close(f);
}

//Interactive mode
void interactive()
{
//Intialising the variables to store buffer for reading lines, array to store commands, current command counter, to store process id, flag for quit and command count
char line[512];
char *commands[100];
int comTotal;
int i,status;
pid_t pid;

int quitTrue=0;

while(1) //Infinate loop is intiated
{
printf(">PROMPT>"); //Prints >PROMPT> until sees a break command
if(fgets(line,sizeof(line), stdin)==NULL)
{
break;
}
comTotal = 0;
commands[comTotal] = strtok(line,";\n");
while (commands[comTotal]!=NULL)
{
comTotal++;
commands[comTotal]=strtok(NULL,";\n");
}
for(i=0;i<comTotal;i++)
{
pid = fork(); //Created child process
if(pid == 0)
{
char *argv[100]; //Child process is here and an array to store arguments for command
int j=0;
argv[j]=strtok(commands[i], " ");
while (argv[j]!=NULL)
{
j++; //Increementing the argument counter
argv[j]=strtok(NULL," ");
}
if(execvp(argv[0],argv)== -1) //Checks and if command returns -1, this handles notfound
{
printf("Command not found: %s\n",commands[i]);
exit(1);
}
}
else
{ //Parent  process is here and waits till child process completes.
waitpid(pid,&status,0);
if(strcmp(commands[i], "quit")==0)
{
quitTrue=1;
}
}
}
if(quitTrue)
{
break;
}
}
}

//Program starts here and check the arguments to call particular function
int main(int argc, char* argv[])
{
if(argc==1)
{
interactive();
}
else if(argc==2)
{
batch(argc,argv);
}
return 0;
}