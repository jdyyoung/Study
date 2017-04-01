void itoa(char* buf, int m)
   {
      //char buf[11]={};
      buf[0]='0';
       buf[1]='x';
       int i=9;
       int c;
      while(m)
      {   c=m%16;
          if(c>9)
          {
              buf[i]=c-10+'A';
          }
          else
          {
              buf[i]=c+'0';
          }
          i--;
          m=m/16;
      }
      //Моід0
      while(i>=2)
      {
          buf[i--]='0';
      }
      buf[10]='\0';
  }
