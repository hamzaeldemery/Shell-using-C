#include <sys/wait.h>
/*
    waitpid() and associated macros 
*/
#include <sys/types.h>
#include <unistd.h>
/*
    chdir()
    fork()
    exec()
    pid_t
*/
#include <stdlib.h>
/*
    malloc()
    realloc()
    free()
    exit()
    execvp()
    EXIT_SUCCESS, EXIT_FAILURE
*/
#include <stdio.h>
/*
    fprintf()
    printf()
    stderr
    getchar()
    perror()
*/
#include <string.h>
/*
    strcmp()
    strtok()
*/
#include <libgen.h>


void loop_shell();
char* read_line();
char** split_line(char*);
int excute(char**);
int lunch(char**);

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_cdb(char **args);

char *builtin_str[]={
    "cd",
    "help",
    "exit",
    "cd.."
};

int (*builtin_func[])(char**)={
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_cdb
};

int lsh_cdb(char **args){
    chdir("..");
    return 1;
}

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args){
    if(args[1]==NULL){
        chdir("/");
        
    }else{
        if(chdir(args[1])!=0){
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args){
    int i;
    printf("Stephen Brennan's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");
    for(i = 0;i < lsh_num_builtins(); ++i){
        printf("%s\n",builtin_str[i]);
    }
    printf("Use the man command for information on other commands.");
    return 1;
}
int lsh_exit(char **args){
    return 0;
}



void loop_shell(){
    int stat;
    char *line;
    char **args;
    do{
        char buff[FILENAME_MAX];
        getcwd( buff, FILENAME_MAX );
        printf("user@user:~%s$ ",buff);
        
        line = read_line();
        args = split_line(line);
        stat = excute(args);

        free(line);
        free(args);

    }while(stat);
}

#define CHAR_ALLOC 1024;

char* read_line(){
    char *line = NULL;
    ssize_t bufsize = 0; // have getline allocate a buffer for us
    getline(&line, &bufsize, stdin);
    return line;
}


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char** split_line(char* line){
    int token_size = LSH_TOK_BUFSIZE,pos=0;
    char** tokens = malloc(sizeof(char*)*token_size);
    char* token;

    if(!tokens){
        fprintf(stderr,"lsh:Allocation error");
        exit(EXIT_FAILURE);
    }
    token=strtok(line,LSH_TOK_DELIM);
    while(token != NULL){
        tokens[pos++] = token;
        if(pos > token_size){
            token_size += LSH_TOK_BUFSIZE;
            tokens=realloc(tokens,sizeof(char*)*token_size);
            if(!tokens){
                fprintf(stderr,"lsh:Allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token=strtok(NULL,LSH_TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}
int excute(char** args){
    
    int i;
    if(args[0] == NULL){
        return 1;
    }
    for(i = 0;i < lsh_num_builtins();++i){
        if(strcmp(args[0],builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    return lunch(args);
}

int lunch(char** args){
    pid_t pid,wpid;
    int stat;
    pid = fork();
    if(pid == 0){
        if(execvp(args[0],args) == -1){
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }else if(pid < 0){
        perror("lsh");
    }else{
        do{
            wpid=waitpid(pid,&stat,WUNTRACED);
        }while(!WIFEXITED(stat)&&!WIFSIGNALED(stat));
    }
    return 1;
}



int main(int argc,char** argv){

    loop_shell();

    return EXIT_SUCCESS;
}
