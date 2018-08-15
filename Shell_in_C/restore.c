#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <error.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include "allfunctions.h"

int main()
{
    if(gethostname(host,200) == -1)
        perror("Cannot get the host name");
    getcwd(dir,sizeof(dir));
    getcwd(last_cd,sizeof(last_cd));
    dirlen=strlen(dir);
    int exit_term=0;
    signal(SIGCHLD, sig_child_handler);
    while(1)
    {
        printPrompt();
        line=input_line();
        count_comm=0;
        char *delim=";";
        comm=split(line,delim,&count_comm);
        int i;
        for(i=0;i<count_comm;i++)
        {
          //  printf("%d--%s\n",i,comm[i]);
            int h=0,j=0;
            cnt-=j;
            count =0;
            char *delim2=" \t";
            //printf("%s\n",comm[i]);
            char arg[1024];
            strcpy(arg,comm[i]);
            //printf("%s\n",arg);
            int len=strlen(arg),cn=0,pi=0;
            while(h<len)
            {  
                if (arg[h]=='>' || arg[h]=='<')
                    cn++;
                if (arg[h]=='|')
                    pi++;
                if (cn==2)
                    break;
                h++;
            }
            //printf("%d\n",cn);

            if(pi>=1 && cn==0)
            {
                //printf("HI\n");
                count=0;
                char** argi=split(arg,"|",&count);
                int g=0,cl=0;
                char **ar[1024];
                int xyz;
                for(int abc=0;abc<count;abc++)
                {
                    xyz=0;
                    ar[abc]=split(argi[abc]," ",&xyz);
                }
                // for(g=0;g<count;g++)
                // {
                //     if(strcmp(argi[g],"|"))
                //     {
                //         ar[t]=argi[g];
                //         printf("%s\n",ar[t]);
                //         t++;
                //     }
                // }
                //printf("%d\n",t);
                int fd[2],fd1[2];
                int status;
                for(cl=0;cl<count;cl++)
                {
                    if(cl%2==0)
                        pipe(fd);
                    else
                        pipe(fd1);
                    pid_t pid=fork();
                    if(pid<0)
                        perror("ERROR forking : ");
                    if(pid==0)
                    {
                        if(cl==0)
                        {
                            dup2(fd[1],1);
                            execvp(*ar[cl], ar[cl]);
                        }
                        else if(cl==count-1)
                        {
                            if(cl%2==1 && dup2(fd[0],0)!=0)
                                perror("ERROR in Dup25 : ");
                            else if(cl%2==0 && dup2(fd1[0],0)!=0)
                                perror("ERROR in Dup23 : ");
                            execvp(*ar[cl],ar[cl]);
                        }
                        else
                        {
                            if(cl%2==0)
                            {
                                //printf("HI\n");
                                if(dup2(fd[1],1)!=1)
                                    perror("ERROR in dup21 : ");
                                if(dup2(fd1[0],0)!=0)
                                    perror("ERROR in dup24 : ");
                                execvp(*ar[cl], ar[cl]);
                            }
                            else
                            {
                                if(dup2(fd[0],0)!=0)
                                    perror("ERROR in dup23 : ");
                                if(dup2(fd1[1],1)!=1)
                                    perror("ERROR in dup22 : ");
                                execvp(*ar[cl],ar[cl]);
                            }
                        }
                    }
                    else
                    {
                        if(cl==0)
                            close(fd[1]);
                        if(cl==count-1)
                        {
                            if(cl%2!=0)
                                close(fd[0]);
                            else
                                close(fd1[0]);
                        }
                        else if(cl!=0 && cl%2==0)
                        { 
                           close(fd[1]);
                           close(fd1[0]);
                        }
                        else if(cl%2!=0)
                        {
                            close(fd[0]);
                            close(fd1[1]);
                        }
                        waitpid(pid,&status,0);       
                    }
                }
                close(fd[0]),close(fd[1]),close(fd1[0]),close(fd1[1]);
                continue;
            }

            if(cn>=1)
            {
                count=0;
                char** argi=split(arg,delim2,&count);
                char *ar[1024],*outar,*inar;
                int g=0,in=0,out=0,app=0,s=0;
                //printf("%d\n", count);
                for(g=0;g<count;g++)
                {
                    //printf("%s\n",argi[g]);
                    if(!strcmp(argi[g],">>"))
                    {
                        outar=argi[g+1];
                        s=-1;
                        app++;
                    }
                    else if(argi[g][0]=='>')
                    {
                        outar=argi[g+1];
                        s=-1;
                        out++;
                    }
                    else if(argi[g][0]=='<')
                    {
                        inar=argi[g+1];
                        s=-1;
                        in++;
                    }
                    else if(s!=-1)
                        ar[g]=argi[g];
                    //printf("%s\n",ar[g]);
                }
                if(in==1 && out==1)
                {
                    new_func_foregrd_redirect(ar,inar,outar,0,0);
                    continue;
                }
                if(in==1 && app==1)
                {
                    new_func_foregrd_redirect(ar,inar,outar,1,0);
                    continue;
                }
                if(in==1)
                {
                    func_foregrd_redirect(ar,argi[count-1],2,0);
                    continue;
                }
                if(out==1)
                {
                    func_foregrd_redirect(ar,argi[count-1],0,1);
                    continue;
                }
                if(app==1)
                {
                    func_foregrd_redirect(ar,argi[count-1],1,1);
                    continue;
                }
            }
            count=0;
            args=split(comm[i],delim2,&count);
            if(!args[0])
                continue;

                char *backg="&\0";
                if(count>1 && !strcmp(backg,args[count-1]))
                {
                    j=1;
                    if(i+1==count_comm)
                        new_func_backgrd();
                    else
                    {
                        perror("ERROR : ");
                        break;
                    }
                    continue;
                }

            char *cd="cd\0";
            if(!strcmp(cd,args[0]))
            {
                j=1;
                func_cd();
            }

            char *setenv="setenv\0";
            if(!strcmp(setenv,args[0]))
            {
                j=1;
                func_setenv(args[1],args[2]);
            }

            char* pinfo="pinfo\0";
            if(!strcmp(pinfo,args[0]))
            {
                j=1;
                func_pinfo();
            }
            
            char *pwd="pwd\0";
            if(!strcmp(pwd,args[0]))
            {
                j=1;
                char cwd2[1001];
                getcwd(cwd2,sizeof(cwd2));
                printf("%s\n",cwd2);
            }

            char *ls="ls\0";
            if(!strcmp(ls,args[0]))
            {
                j=1;
                if(args[1]==NULL)
                    func_ls(args[1]);
                else if(!strcmp("-a\0",args[1]) && args[2]==NULL)
                    func_ls(args[1]);
                else
                    func_ls_l(args[1],args[2]);
            }

            char *echo="echo\0";
            if(!strcmp(echo,args[0]))
            {
                j=1;
                func_echo();
            }

            char *exit="exit\0";
            if(!strcmp(exit,args[0]))
            {
                exit_term=1;
                j=1;
                break;
            }
            if(j!=1)
                new_func_foregrd();
        }
        if(exit_term)
            break;

    }
    return 0;
}