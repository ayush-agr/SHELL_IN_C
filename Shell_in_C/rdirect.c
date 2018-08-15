void func_redirect(char * arg)
        {
             char *delim2=" \t";
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
                return;
            }
            if(in==1 && app==1)
            {
                new_func_foregrd_redirect(ar,inar,outar,1,0);
                return;
            }
            if(in==1)
            {
                func_foregrd_redirect(ar,argi[count-1],2,0);
                return;
            }
            if(out==1)
            {
                func_foregrd_redirect(ar,argi[count-1],0,1);
                return;
            }
            if(app==1)
            {
                func_foregrd_redirect(ar,argi[count-1],1,1);
                return;
            }
        }
