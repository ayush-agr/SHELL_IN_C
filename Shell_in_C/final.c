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
#include "piping.c"

prc* BGadd(prc *p,char pname[],int pid,char stats[])
{
    if (p==NULL)
    {
        prc *tmp = (prc*)malloc(sizeof(prc));
        strcpy(tmp->pname,pname);
        strcpy(tmp->stats,stats);
        tmp->pid=pid;
        tmp->link=NULL;
        return tmp;
    }
    else
        p->link=BGadd(p->link,pname,pid,stats);
    return p;
}

int main()
{
    if(gethostname(host,200) == -1)
        perror("Cannot get the host name");
    getcwd(dir,sizeof(dir));
    getcwd(last_cd,sizeof(last_cd));
    dirlen=strlen(dir);
    int exit_term=0;
    //signal(SIGCHLD, sig_child_handler);
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

            if(pi>=1 /*&& cn==0*/)
            {
                //printf("HI\n");
                func_piping(arg);
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

            char *unsetenv="unsetenv\0";
            if(!strcmp(unsetenv,args[0]))
            {
                j=1;
                func_unsetenv(args[1]);
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

            char *jobs="jobs\0";
            if(!strcmp(jobs,args[0]))
            {
                j=1;
                displayBG(BG_process,0);
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