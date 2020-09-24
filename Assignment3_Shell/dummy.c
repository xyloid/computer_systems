#include <stdio.h>
#include<stdlib.h>
int main(int argc, char **argv) {
    char buf[256];
    scanf("%s",buf);
    printf("Get:%s\n",buf);
    
    /* 
    size_t buf_size=256;
    char buf[buf_size];
    char** p = (char**)&buf;
    getline(p,&buf_size,stdin);
    printf("%s\n",buf);
    */
    
    //int c;
    // Ctrl-d = EOF in cmd.
    /* 
    while ((c=getchar())!=EOF)
    {
        putchar(c);
    }
    */
	return 0;
}
