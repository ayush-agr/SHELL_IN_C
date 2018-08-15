char host[1024];
char user[1024];
char cwd[1024];
char *line;
char** args;
char** comm;
char dir[1021];
int dirlen;
char *cdir="~\0";
char last_cd[1021];
int processarr[100];
int cnt=0;

typedef struct process{
    char pname[1024];
    char stats[1024];
    struct process *link;
    int pid;
}prc;
prc *BG_process=NULL;

void func_ls(char*);
void func_ls_l(char*,char*);
void func_cd();
void func_echo();
void func_pinfo();
void new_func_backgrd();
void new_func_foregrd();
void func_foregrd_redirect(char **,char *,int,int);
void new_func_foregrd_redirect(char **,char *,char *,int,int);
char *getting_permissions(struct stat st,char *);
void func_setenv(char *,char *);
prc* BGadd(prc *,char *,int ,char *);
void displayBG(prc *, int );

int count;
int count_comm;

void printPrompt()
{
    struct passwd *passwd = getpwuid(getuid());
    strcpy(user,passwd->pw_name);

    if(!strcmp(dir,cdir))
    {
    	cdir="~\0";
    }
    else if(strlen(cdir)>strlen(dir) && cdir[dirlen]=='/')
    {
        cdir[dirlen]='\0';
        if(!strcmp(cdir,dir))
        {
            cdir[dirlen]='/';
            cdir[dirlen-1]='~';
            cdir+=dirlen-1;
        }
    }
    printf("<%s@%s:%s> ",user,host,cdir);
}

char** split(char * line1,char* delim,int* counter)
{
    int Limit=512;
    char** args1=(char**)malloc(sizeof(char*)*Limit);
    char* elem=strtok(line1,delim);
    while(1)
    {
        if(!elem)
        {
            args1[*counter]=NULL;
            return args1;
        }
        args1[(*counter)++]=elem;
        elem=strtok(NULL,delim);
        if(*counter==Limit)
        {
            Limit=Limit<<1;
            args1=(char**)realloc(args1,sizeof(char*)*Limit);
            if(!args1)
            {
                perror("ERROR : ");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char* input_line()
{
    int Limit=512;
    char *line1=(char*)malloc(sizeof(char)*Limit);
    int ind=0;
    char c;
    while(1)
    {
        c=getchar();
        if(c=='\n' || c==EOF)
        {
            line1[ind]='\0';
            return line1;
        }

        line1[ind]=c;
        ind++;
        if(Limit==ind)
        {
            Limit=Limit<<1;
            line1=realloc(line1,sizeof(char)*Limit);
            if(!line1)
            {
                perror("ERROR : ");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void func_echo()
{
            int j=1;
            //char *arr="";
            //arr=(char*)malloc(1024*sizeof(char));
            while(args[j]!=NULL)
            {
                if(j!=1)
                    printf(" ");
                printf("%s",args[j]);
                j++;
            }
            printf("\n");
}

void func_ls(char* a)
{
    char *b;
    if(!a)
        b="x";
    else
        b=a;
    char cwd1[1024];
    getcwd(cwd1,sizeof(cwd1));
    struct dirent **fileList;
    int noOfFilesAndDirs=scandir(cwd1,&fileList,NULL,alphasort);
    for(int i=0;i<noOfFilesAndDirs;i++)
    {
        if(!strcmp(b,"-a\0") || fileList[i]->d_name[0]!='.')
            printf("%s  ",fileList[i]->d_name);
    }
    printf("\n");
}

void func_ls_l(char *a,char *d)
{
    char *b;
    if(!a)
        b="x";
    else
        b=a;
    if(!((d!=NULL && !strcmp(d,"-a\0")) || !strcmp(b,"-l\0") || !strcmp(b,"-a\0") || !strcmp(b,"-al\0") || !strcmp(b,"-la\0"))) 
    {   
        printf("ERROR : Invalid command\n");
        return;
    }
    char cwd1[1024];
        getcwd(cwd1,sizeof(cwd1));
    struct dirent **fileList;
    int noOfFilesAndDirs=scandir(cwd1,&fileList,NULL,alphasort);
    for(int i=0;i<noOfFilesAndDirs;i++)
    {
        char ans[11];
        if((d!=NULL&&!strcmp(d,"-a\0")) || !strcmp(b,"-a\0") || !strcmp(b,"-al\0") || !strcmp(b,"-la\0") || fileList[i]->d_name[0]!='.')
        {
            struct stat st;
            stat(fileList[i]->d_name, &st);
            struct passwd *p = getpwuid(st.st_uid);
            struct group  *grp = getgrgid(st.st_gid);
            int siz = st.st_size;
            struct tm *pr = localtime(&st.st_ctime);
            printf("%s %d %s %s %d",getting_permissions(st,ans),(int)st.st_nlink,p->pw_name,grp->gr_name,siz);
            char fulltime[50];
            strftime(fulltime,50," %d/%m %H:%M",localtime(&st.st_mtime));
            printf("%s %s\n",fulltime,fileList[i]->d_name);
        }
    }
    printf("\n");   
}

char* getting_permissions(struct stat outst, char *ans)
{
    if(S_ISDIR(outst.st_mode))
        ans[0]='d';
    else
        ans[0]='-';
    if((outst.st_mode & S_IRUSR) == S_IRUSR)
        ans[1]='r';
    else
        ans[1]='-';
    if((outst.st_mode & S_IWUSR) == S_IWUSR)
        ans[2]='w';
    else
        ans[2]='-';
    if((outst.st_mode & S_IXUSR) == S_IXUSR)
        ans[3]='x';
    else
        ans[3]='-';


    //file group permissions    
    if(outst.st_mode & S_IRGRP)
        ans[4]='r';
    else
        ans[4]='-';
    if(outst.st_mode & S_IWGRP)
        ans[5]='w';
    else
        ans[5]='-';
    if(outst.st_mode & S_IXGRP)
        ans[6]='x';
    else
        ans[6]='-';


    //file others permission
    if(outst.st_mode & S_IROTH)
        ans[7]='r';
    else
        ans[7]='-';
    if(outst.st_mode & S_IWOTH)
        ans[8]='w';
    else
        ans[8]='-';
    if(outst.st_mode & S_IXOTH)
        ans[9]='x';
    else
        ans[9]='-';

    ans[10]='\0';
    return ans;
}

void func_cd()
{
            char last_cwd[101];
            getcwd(last_cwd,sizeof(last_cwd));
            if(args[1]==NULL || !strcmp(args[1],"~\0"))
            {
                memset(last_cd,'\0',sizeof(last_cd));
                getcwd(last_cd,sizeof(last_cd));
                chdir(dir);
                cdir="~\0";
            }
            else if(!strcmp(args[1],"-\0"))
            {
                chdir(last_cd);
                strcpy(cwd,last_cd);
                cdir=&cwd[0];
                memset(last_cd,'\0',sizeof(last_cd));
                strcpy(last_cd,last_cwd);
            }
            else if(chdir(args[1])==-1)
                perror("ERROR : ");
            else
            {
                if(getcwd(cwd,sizeof(cwd)))
                {
                    memset(last_cd,'\0',sizeof(last_cd));
                    strcpy(last_cd,last_cwd);
                    /*if(strlen(cwd)<dirlen)
                        cdir=&cwd[0];
                    else if(!strcmp(cwd,dir))
                        cdir="~\0";
                    else if(cwd[dirlen]=='/')
                    {
                        cwd[dirlen]='\0';
                        if(!strcmp(dir,cwd))
                        {
                            cwd[dirlen-1]='~';
                            cwd[dirlen]='/';
                            cdir=&cwd[dirlen-1];
                        }
                    }
                    else*/
                    {
                        cdir=&cwd[0];
                    }
                }
                else
                    perror("Error: ");
            }
}

void sig_child_handler(int signum)
{
    int status;
    pid_t pid;
    while((pid=waitpid(-1,&status,WNOHANG))>0)
    {
        if (pid!=0 && pid!=-1)
            if(WIFEXITED(status))
                fprintf(stderr,"\n Process %d exited normally \n", pid);
            else if (WIFSIGNALED(status))
                fprintf(stderr,"\n Process %d exited normally \n", pid);
    }
    return;
}

void new_func_backgrd()
{
    int status,pid=fork();
    if(pid<0)
    {
        perror("ERROR : ");
        exit(1);
    }
    else if(pid==0)
    {
        args[count-1]=NULL;
        if(execvp(*args,args)<0)
        {
            perror("ERROR : ");
            exit(1);
        }
    }
    else
    {
        processarr[cnt]=pid;
        cnt++;
        //wait(NULL);
        if(!strcmp(args[count-1],"&\0")==0)
        {
            BG_process=BGadd(BG_process,args[0],getpid(),"RUNNING");
            waitpid(pid,&status,-1);
        }    
    }
        //wait(NULL);
        //while(wait(&status)!=pid);
}

void new_func_foregrd()
{
    int status,pid=fork();
    if(pid<0)
    {
        perror("ERROR: ");
        exit(1);
    }
    else if(pid==0)
    {
        if(execvp(*args,args)<0)
        {
            perror("ERROR: ");
            exit(1);
        }
    }
    else
        waitpid(pid,&status,0); 
}

void func_foregrd_redirect(char **arg,char *cr,int fl,int g)
{
    int status,pid=fork();
    if(pid<0)
    {
        perror("ERROR: ");
        exit(1);
    }
    else if(pid==0)
    {
        int i=0,fd;
        if(g==0)
        {
            fd = open(cr, O_RDONLY, 0400);
            close(0);
            if(dup2(fd, 0) != 0)
                perror("dup2 fail");
            close(fd);
        }
        if(fl==0)
        {
            fd = open(cr, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            close(1); // good practice
            if(dup2(fd, 1) != 1)
                perror("dup2 fail");
            close(fd);
        }
        else if(fl==1)
        {
            fd = open(cr, O_CREAT | O_RDWR | O_APPEND, 0644);
            close(1); // good practice
            if(dup2(fd, 1) != 1)
                perror("dup2 fail");
            close(fd);
        }
        if(fd == -1)
            perror("Failed to open file");
        if(execvp(*arg,arg)<0)
        {
            perror("ERROR: ");
            exit(1);
        }
    }
    else
        waitpid(pid,&status,0); 
}

void new_func_foregrd_redirect(char **arg,char *ar,char *cr,int fl,int g)
{
    int status,pid=fork();
    if(pid<0)
    {
        perror("ERROR: ");
        exit(1);
    }
    else if(pid==0)
    {
        int i=0,fd;
        if(g==0)
        {
            fd = open(ar, O_RDONLY, 0400);
            close(0);
            if(dup2(fd, 0) != 0)
                perror("dup2 fail");
            close(fd);
        }
        if(fl==0)
        {
            fd = open(cr, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            close(1); // good practice
            if(dup2(fd, 1) != 1)
                perror("dup2 fail");
            close(fd);
        }
        else if(fl==1)
        {
            fd = open(cr, O_CREAT | O_RDWR | O_APPEND, 0644);
            close(1); // good practice
            if(dup2(fd, 1) != 1)
                perror("dup2 fail");
            close(fd);
        }
        if(fd == -1)
            perror("Failed to open file");
        if(execvp(*arg,arg)<0)
        {
            perror("ERROR: ");
            exit(1);
        }
    }
    else
        waitpid(pid,&status,0); 
}

void func_pinfo()
{
    int pid;
    if(!args[1])
        pid=getpid();
    else
        pid=atoi(args[1]);

    char Name[600];
    char c[20]="/proc/\0";
    char d[20];
    snprintf(d,20,"%d",pid);
    char *f=strcat(c,d);
    f=strcat(f,"/status\0");

    char c1[20]="/proc/\0";
    char d1[20];
    snprintf(d1,20,"%d",pid);
    char* abc=strcat(c1,d1);
    abc=strcat(abc,"/exe\0");
    int ret=readlink(abc,Name,(size_t)sizeof(Name));

    FILE* file=fopen(f,"r");    
    if(!file)
    {
        perror("ERROR : ");
        return;
    }
    char line[500];
    char Status[50];
    char Size[300];
    int state_p=0;
    int size_p=0;
    while(fgets(line,sizeof(line),file))
    {
        if(!strncmp(line,"State\0",5))
        {
            strcpy(Status,line+7);
            state_p=1;
        }
        else if(!strncmp(line,"VmSize\0",6))
        {
            strcpy(Size,line+10);
            size_p=1;
            break;
        }
    }
    printf("\npid-> %d\n",pid);
    if(state_p)
        printf("Process Status-> %s",Status);
    if(size_p)
        printf("Memory-> %s",Size);
    if(ret!=-1)
    {
        Name[ret]='\0';
        if(dirlen<strlen(Name) && !strncmp(Name,dir,dirlen))
        {
            printf("Executable Path-> ~%s\n\n",&Name[dirlen]);
        }
        else
            printf("Executable Path-> %s\n\n",Name);
    }
    else
        printf("\n");
}

void func_setenv(char *a,char *b)
{
    if(setenv(a,b,1)<0)
        perror("ERROR:");
}
void func_unsetenv(char *a)
{
    unsetenv(a);
}

prc* BGremove(prc *p, int n)
{
    if(p==NULL)
        return NULL;
    else if(p->pid == n)
    {
        prc *ne = p->link;
        free(p);
        return ne;
    }

    p->link = BGremove(p->link,n);
    return p;
}

void displayBG(prc *t, int c)
{
    if(t==NULL)
        return;
    printf("[%d] %s - %d [ %s ]\n",c, t->pname, t->pid, t->stats);
    displayBG(t->link, c+1);
    return;
}