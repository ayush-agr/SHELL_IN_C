

void func_piping(char* arg){
	int count=0;
	int act_out=dup(1);
	char** argi=split(arg,"|",&count);
	int g=0,cl=0,flg1=0,flg2=0;
	char **ar[1024],*tmpc;
	int xyz;
	for(int abc=0;abc<count;abc++)
	{
		xyz=0;
		if(abc==0)
        {
            for(int y=0;y<strlen(argi[abc]);y++)
            {
                if(argi[abc][y]=='<')
                {
                    flg1=1;
                    break;
                }
            }
            if(flg1==1)
            {
                ar[abc]=split(argi[abc]," <",&xyz);
                tmpc=ar[abc][xyz-1];
                ar[abc][xyz-1]=NULL;
                //printf("%s\n", *(ar[abc]+1));
            }
            else
            	ar[abc]=split(argi[abc]," ",&xyz);	
        }
		else if(abc==count-1)
        {
            for(int y=0;y<strlen(argi[abc]);y++)
            {
                if(argi[abc][y]=='>')
                    flg2=1;
            }
            if(flg2==1)
            {
                ar[abc]=split(argi[abc]," >",&xyz);
                tmpc=ar[abc][xyz-1];
                ar[abc][xyz-1]=NULL;
                //printf("%s\n", *(ar[abc]+1));
            }
            else
            	ar[abc]=split(argi[abc]," ",&xyz);
        }
        else
			ar[abc]=split(argi[abc]," ",&xyz);
	}
	int fd[2],fd1[2];
	int status;
	if(flg1==1)
    {
    	int act_out=dup(1);
    	pipe(fd);
         if(dup2(fd[1],1)!=1)
             perror("ERROR in DUP1 : ");
         //close(fd[1]);
        //printf("%s\n", ar[0]);
        func_foregrd_redirect(ar[0],tmpc,2,0);
    }
	for(cl=flg1;cl<count-flg2;cl++)
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
	/*if(flg2==1)
    {
    	
         if(dup2(fd[0],0)!=0)
             perror("ERROR in DUP1 : ");
         //close(fd[1]);
        //printf("%s\n", ar[0]);
         int asdf=0;
         //while(ar[0][asdf])
         //	printf("%s\n",*ar[0][asdf++]);
        func_foregrd_redirect(ar[count-1],tmpc,0,1);
    }*/
	//close(fd[0]),close(fd[1]),close(fd1[0]),close(fd1[1]);
}
