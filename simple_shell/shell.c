#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include<signal.h>

#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIMITER " \t\r\n\a"

volatile sig_atomic_t stop;

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
void shell_loop(void);
char *shell_read_line(void);
char **shell_split_line(char *line);
int shell_launch(char **args);
void shell_dir(void);
void shell_extractIpAddress(unsigned char *sourceString,short *ipAddress);
int shell_ip_class(char **args);
int shell_get_age(char **args);
int shell_compound_interest(char **args);
int shell_clock(char **args);


char *builtin_str[] = {
	"cd",
	"help",
	"exit",
	"ipclass",
	"age",
	"cmpinterest",
	"clock"
};

int (*builtin_func[]) (char **) = {
	&shell_cd,
	&shell_help,
	&shell_exit,
	&shell_ip_class,
	&shell_get_age,
	&shell_compound_interest,
	&shell_clock
};

int shell_num_builtins() {
	return sizeof(builtin_str) / sizeof(char*);
}

int isLeapYear(int year, int mon) 
{
    int flag = 0;
    if (year % 100 == 0) 
    {
		if (year % 400 == 0) 
		{
			if (mon == 2) 
			{
					flag = 1;
			}
		}
    } 
    else if (year % 4 == 0) 
    {
		if (mon == 2) 
		{
			flag = 1;
		}
    }
    return (flag);
}

void shell_extractIpAddress(unsigned char *sourceString,short *ipAddress)
{
    unsigned short len=0;
    unsigned char  oct[4]={0},cnt=0,cnt1=0,i,buf[5];
 
    len=strlen(sourceString);
    for(i=0;i<len;i++)
    {
        if(sourceString[i]!='.'){
            buf[cnt++] =sourceString[i];
        }
        if(sourceString[i]=='.' || i==len-1){
            buf[cnt]='\0';
            cnt=0;
            oct[cnt1++]=atoi(buf);
        }
    }
    ipAddress[0]=oct[0];
    ipAddress[1]=oct[1];
    ipAddress[2]=oct[2];
    ipAddress[3]=oct[3];
}

void shell_dir(void) {
	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd() error");
	} else {
		printf("%s", cwd);
	}

}

void handle_sigint(int sig) { 
    stop = 1;
}
int shell_clock(char **args) {
	int hour, minute, second, exitClock;
	printf("Exit with CRTL + C\n");
	sleep(2);

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    hour = timeinfo->tm_hour;
	minute = timeinfo->tm_min;
	second = timeinfo->tm_sec;


	signal(SIGINT, handle_sigint);
    while(!stop)
    {
        //clear output screen
        system("clear");
        //print time in HH : MM : SS format
        printf("%02d : %02d : %02d ",hour,minute,second);
         
         //clear output buffer in gcc
        fflush(stdout);
         
         //increase second
        second++;
 
        //update hour, minute and second
        if(second==60){
            minute+=1;
            second=0;
        }
        if(minute==60){
            hour+=1;
            minute=0;
        }
        if(hour==24){
            hour=0;
            minute=0;
            second=0;
        }
        
        sleep(1);   //wait till 1 second 
    }
	stop = 0;
    return 1;
}
int shell_compound_interest(char **args) {
	float ci;
	if((strtof(args[1],NULL) == 0) || (strtof(args[2],NULL) == 0) || (strtof(args[3],NULL) == 0)) {
		printf("shell: Specify correct arguments for: Principal, Rate, Time!\n");
		return 1;
	}
    ci=strtof(args[1],NULL)*((pow((1+strtof(args[2],NULL)/100),strtof(args[3],NULL))-1));
    printf("Starting with: %s\n", args[1]);
    printf("Compound interest is: %f\n",ci);
	printf("Result in %s years: %f\n", args[3], strtof(args[1], NULL) + ci);
         
    return 1;
}

int shell_get_age(char **args) {

	int DaysInMon[] = {31, 28, 31, 30, 31, 30,
                       31, 31, 30, 31, 30, 31};
    int days, month, year, i;
    char dob[100];
    time_t ts;
    struct tm *ct;
	char day[5],mnth[5],yr[5];
    /* enter date of birth */
    // printf("Enter your date of birth (DD/MM/YYYY): ");
    // scanf("%d/%d/%d",&days,&month, &year);
	for (i = 0; i < 2; i++) {
		day[i] = args[1][i];
	}
	if (strlen(day) != 2) {
		printf("shell: Agrument formatting error!\n");
		return 1;
	}
	days = atoi(day);
	for (i = 0; i < 2; i++) {
		mnth[i] = args[1][i+3];
	}
	mnth[3] = '\0';
	if (strlen(mnth) -1 != 2) {
		printf("shell: Agrument formatting error!\n");
		return 1;
	}
	month = atoi(mnth);
	for (i = 0; i < 4; i++) {
		yr[i] = args[1][i+6];
	}
	if (strlen(yr) != 4) {
		printf("shell: Agrument formatting error!\n");
		return 1;
	}
	year = atoi(yr);
    /*get current date.*/
    ts = time(NULL);
    ct = localtime(&ts);
 
    printf("Current Date: %d/%d/%d\n",
            ct->tm_mday, ct->tm_mon + 1, ct->tm_year + 1900);
 
    days = DaysInMon[month - 1] - days + 1;
 
    /* leap year checking*/
    if (isLeapYear(year, month)) 
    {
            days = days + 1;
    }
 
    /* calculating age in no of days, years and months */
    days = days + ct->tm_mday;
    month = (12 - month) + (ct->tm_mon);
    year = (ct->tm_year + 1900) - year - 1;
 
    /* checking for leap year feb - 29 days */
    if (isLeapYear((ct->tm_year + 1900), (ct->tm_mon + 1))) 
    {
            if (days >= (DaysInMon[ct->tm_mon] + 1)) 
            {
                    days = days - (DaysInMon[ct->tm_mon] + 1);
                    month = month + 1;
            }
    } 
    else if (days >= DaysInMon[ct->tm_mon]) 
    {
            days = days - (DaysInMon[ct->tm_mon]);
            month = month + 1;
    }
 
    if (month >= 12) 
    {
            year = year + 1;
            month = month - 12;
    }
 
    /* print age */
    printf("\n## Hey you are  %d years %d months and %d days old. ##\n", year, month, days);
 
    return 1;
}

int shell_ip_class(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "shell: expected argument to \"ipclass\"\n");
	} else {
		unsigned char ip[20];
    	short ipAddress[4];
		shell_extractIpAddress(args[1],&ipAddress[0]);
		printf("\nIp Address: %03d. %03d. %03d. %03d\n",ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3]);
    
		if(ipAddress[0]>=0 && ipAddress[0]<=127)
			printf("Class A Ip Address.\n");
		if(ipAddress[0]>127 && ipAddress[0]<191)
			printf("Class B Ip Address.\n");
		if(ipAddress[0]>191 && ipAddress[0]<224)
			printf("Class C Ip Address.\n");
		if(ipAddress[0]>224 && ipAddress[0]<=239)
			printf("Class D Ip Address.\n");
		if(ipAddress[0]>239)
			printf("Class E Ip Address.\n"); 
	}
	return 1;
}

int shell_cd(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "shell: exprected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("shell");
		}
	}
	return 1;
}

int shell_help(char **args) {
	int i;
	printf("Simple shell written in C\n");
	printf("Built in commands are:\n");
	for (i = 0; i < shell_num_builtins(); i++) {
		printf("\033[1;31m");
		printf("  %s", builtin_str[i]);
		printf("\033[0m");
		if( builtin_str[i] == "ipclass") {
			printf(" - Provide a valid IP Address (xxx.xxx.xxx.xxx format).");
		}
		if ( builtin_str[i] == "age") {
			printf(" - Provide your date of birth (DD/MM/YYYY).");
		}
		if ( builtin_str[i] == "cmpinterest") {
			printf(" - Provide in order: principal, rate and time in years, will print compound interest on entered principal for given time period.");
		}
		if ( builtin_str[i] == "clock") {
			printf(" - Starts a regular clock, use it as a screen saver.");
		}
		printf("\n");
	}

	printf("Use the \"man\" command for info on other inherited shell commands.\n");
	return 1;
}

int shell_execute(char **args) {
	int i;
	if (args[0] == NULL) {
		return 1;
	}
	for(i = 0; i < shell_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return shell_launch(args);
}

int shell_exit(char **args) {
	return 0;
}

int shell_launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("shell");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("shell");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

char **shell_split_line(char *line) {
	int bufsize = SHELL_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;
	if(!tokens) {
		fprintf(stderr, "shell: allocation error\n");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, SHELL_TOK_DELIMITER);
	while (token != NULL) {
		tokens[position] = token;
		position++;
		if(position >= bufsize) {
			bufsize += SHELL_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if(!tokens) {
				fprintf(stderr, "shell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, SHELL_TOK_DELIMITER);
	}
	tokens[position] = NULL;
	return tokens;
}

char *shell_read_line(void) {
	int bufsize = SHELL_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;
	if(!buffer) {
		fprintf(stderr, "shell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		c = getchar();
		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;
		if(position >= bufsize) {
			bufsize += SHELL_RL_BUFSIZE;
			buffer = realloc(buffer,bufsize);
			if (!buffer) {
				fprintf(stderr, "shell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

void shell_loop(void) {
	char *line;
	char **args;
	int status;
	do {
		printf("\033[1;30m");
		shell_dir();
		printf("\033[1;31m");
		printf("$ ");
		printf("\033[0m");
		line = shell_read_line();
		args = shell_split_line(line);
		status = shell_execute(args);
		free(line);
		free(args);
	} while (status);
}



int main(int argc, char **argv) {
	shell_loop();

	return EXIT_SUCCESS;
}

