/**
*  Name: Porfirio Mohabir
 * @file 
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html
 */

#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>
#include "debug.h"
#include "history.h"

#ifndef ARG_MAX
#define ARG_MAX 4096
#endif 



/**
* Struct command line
* Command Line  = tokens
* bool for pipe
* char for file output
*/
struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
};


int status = 0;
int count = 0;
bool sig_flag = false;

/** next_token prototype */
char *next_token(char **str_ptr, const char *delim);
/** build_in prototype */
bool built_in_cmnds(char *args[]);
/** execute_cmd prototype */
bool execute_cmd(char *line, bool flag);
/** signit_handler prototype */
void sigint_handler(int signo);
/** execute_pipline prototype */
bool execute_pipeline(char **args,int tokens);
/** print_promt prototype */
void print_prompt(int status, int count);



/**
 * This Fucntion Performs Piping in/out IPC. 
 * Takes in @param args and @param tokens (size of args)
 * Also performs redirections to output files
 * when '>' is found in args
 * Fucntion is a bool and return true
 * if pipes and redirections is possible
 */
bool execute_pipeline(char **args, int tokens){
    
    struct command_line cmd[tokens];

    int cmd_count = 0;
    char **starts = &args[0];
    bool flag_redir = false;

    for(int i = 0; i<tokens; i++){
    
      
        if(strcmp("|", args[i])==0){
         
            args[i] = NULL;
            cmd[cmd_count].tokens = starts;
            starts = &args[i+1];
            cmd[cmd_count].stdout_pipe = true; 
            cmd[cmd_count].stdout_file= NULL;
          
            cmd_count++;
        }
        else if(strcmp(">", args[i])==0){
            flag_redir = true;
            args[i]= NULL;
            cmd[cmd_count].tokens = starts;
            cmd[cmd_count].stdout_file = args[i+1];
            break;
        } 
        if(!flag_redir){
            cmd[cmd_count].tokens = starts;
            cmd[cmd_count].stdout_file= NULL;
        }
    }
    
    cmd[cmd_count].stdout_pipe = false;


    int i = 0;
    while (cmd[i].stdout_pipe){
        /* Creates a pipe. */
        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe");
            
            break;
        }

        pid_t child = fork();
        if(child == -1){
            perror("fork");
          
            break;
        }
        else if (child == 0) {
            /* Child */
            if(dup2(fd[1], STDOUT_FILENO) == -1){
                perror("dup2");
            }

            close(fd[0]);
            int exe = execvp(cmd[i].tokens[0], cmd[i].tokens);
            if(exe == -1){
                perror("oh no! execvp");
              
                break;
            }
        } else {
            /* Parent */
            if(dup2(fd[0], STDIN_FILENO)==-1){
                perror("dup2");
            }
            close(fd[1]);
            i++;
        }
    }

    if(!cmd[i].stdout_pipe){
        
        int open_flags = O_RDWR | O_CREAT |O_TRUNC;


        if(cmd[i].stdout_file != NULL){
            int fd = open(cmd[i].stdout_file, open_flags, 0644);
            if(dup2(fd, fileno(stdout))==-1){
                perror("dup2");
            }
            int exe = execvp(cmd[i].tokens[0], cmd[i].tokens);
            if(exe == -1){
                perror("oh no! execvp");
            }
        }else{
            int exe = execvp(cmd[i].tokens[0], cmd[i].tokens);
            if(exe == -1){
                perror("oh no! execvp");
            }
        }
        return true;
    }


    return false;
}


/**
 * Sigint_handler prevent "CTRL + C" to quit program
 *
 */
void sigint_handler(int signo) {
    if(isatty(STDIN_FILENO)){
        printf("\n");
    }
    if(sig_flag){
        fflush(stdout);
    }
}

/**
 * Fucntion is a boolean
 * Takes in @param line and @param flag
 * Funtion executes a user gerenated command in shell
 * First the function tokenizes to determmine the specific command
 * If command successfull executes it will return ture to the main
 * Otherwise, faile to execture will result to a return false 
 *
 */
bool execute_cmd(char *line, bool flag){
        char *line_dup = strdup(line);
        char *args[ARG_MAX]; 
        int tokens = 0;
        char *next_tok = line;
       

        char *curr_tok;
   

        while ((curr_tok = next_token(&next_tok, " \t\r\n")) != NULL && tokens<ARG_MAX) {
         
            if(start("#", curr_tok)){
                // comment = true;
                if(tokens == 0){
                    tokens++;
                }
                break;
            }
            if(start("$", curr_tok)){
                //char *token_S = next_token(&curr_tok, " $\n");
                char *dollar = getenv(next_token(&curr_tok, " $\n"));
                curr_tok = dollar;
            }
            args[tokens++] = curr_tok;
        }

        args[tokens] = 0;

        if (args[0] == 0){
            return false;
        }

        if(strcmp("exit", args[0]) == 0){
            
            free_history();
            exit(0);
        }

        if(flag){
            add_entry(line_dup, count);
          
        }

  
        
        if(!built_in_cmnds(args)){
            pid_t child = fork();
            if(child == -1){
                perror("fork");
            }
            else if(child == 0){
    
                
                    bool ret = execute_pipeline(args, tokens);
                    if(!ret){
                        perror("execvp");
                    }
                
                close(STDIN_FILENO);
                close(STDOUT_FILENO);
                close(STDERR_FILENO);
                exit(EXIT_FAILURE);
            }else{
                
                /* We're the parent, look at how great we are at parenting */
                 waitpid(child, &status, 0);
                
            }
        }

    free(line_dup);

    return true;

}



/**
 * Function Return bool to determine if global count increments
 * Funcntion takes in @param args is tokenized in the execute_cmd fucntion
 * Multiple if conditionals that will etermine which built-in comman will execute
 */
bool built_in_cmnds(char *args[]){
    if(strcmp("cd", args[0])==0){
        struct passwd *x = getpwuid(getuid());
        const char *dir = x->pw_dir;
        if(args[1] != NULL){
            int ch;
            ch = chdir(args[1]);
            if(ch !=0){
                perror("CD COMMAND");
            }
        }
        else{
            chdir(dir);
            status = 0;
        }
        return true;
    }

    if(strcmp("setenv", args[0])==0){
      if((args[1] != NULL) && (args[2]!= NULL)){
           char *get_env = getenv(args[1]);

            if(get_env == NULL){
                status = setenv(args[1], args[2], 0);
            }
            else{
                status = setenv(args[1], args[2], 1);
            }
        }

        return true;
    }

    if(strcmp("history", args[0])==0){
        print_history();
        return true;
    }

    if(strcmp("!!", args[0])==0){
        char *last_cmd = bang_bang_cmd();

        bool flag = execute_cmd(last_cmd,true);
        return flag;

        LOG("LAST COMMAND %s\n", last_cmd);
        
        free(last_cmd);
        return false; 
        
    }

    if(start("!", args[0])){

        char *new_str = args[0] + 1; 

        bool flag = true;

        for(int i = 0; i<strlen(new_str); i++){
            if(isdigit(new_str[i]) == 0){
                flag = false; 
                break; 
            }
        }

        char *last_cmd = search(new_str, flag);
   
        if(last_cmd !=NULL){
            bool flag_exc = execute_cmd(last_cmd, true);
            return flag_exc;
        }else{
            fprintf(stderr, "NULL COMMAND\n");
        }
        return false; 
    }

    return false;
}



/**
 * Funtion return true if @param str starts with @param pre
 * false otherwise.
 */
bool start(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}


/**
 * Function creates and prints prompt for user
 * Emoji states the @param status of the user command
 * @count increments when user enters a command
 */
void print_prompt(int status, int count){

    const char smiley[5] = {0xF0, 0x9F, 0x99, 0x82, '\0'};
    const char vomit[5] = {0xF0, 0x9F, 0xA4, 0xAE, '\0'};
  
    if(status == 0){
        printf("[%s]-", smiley);
    }
    else{
        printf("[%s]-", vomit);

    }

    printf("[%d]-", count);

    char hostname[1024];
    char username[1024];

    gethostname(hostname, 1024);
    getlogin_r(username, 1024);


    printf("[%s@%s: ",username, hostname);

    char cwd[1024];

    getcwd(cwd, 1024);

    char temp_cwd[1024];

    if(start("/home", cwd)){
        strcpy(temp_cwd, "~/");
    }

    int tokens = 0;
    char *next_tok = cwd;
    char *curr_tok;

    while((curr_tok = next_token(&next_tok, "/"))!= NULL){
        if(tokens > 1){
            strcat(temp_cwd, curr_tok);
        }
        tokens++;
    }

    if(start("~", temp_cwd)){
        printf("%s]", temp_cwd);
    }
    else{
        printf("%s]", cwd);
    }


    fflush(stdout);
}


/**
* Tokenization function
*/
char *next_token(char **str_ptr, const char *delim){
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}

/**
* Main program. Program starts by getting user input and begins proccess
* of ecexecuton of the commands in shell
*/
int main(void){
    LOGP("Initializing shell\n");
    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");
    bool flag = true; 
    

    signal(SIGINT, sigint_handler);

    while(true){
        if (isatty(STDIN_FILENO)) {
            print_prompt(status, count);
        } 
    
        char* line = NULL;
        size_t line_sz = 1000;
        int err = getline(&line, &line_sz, stdin);
        if(err == -1){
            free(line);
            break;
        }
        if(execute_cmd(line, flag)){
            count++; 
            sig_flag = true;
        }

        
        free(line);
    }

    return 0;
 }
