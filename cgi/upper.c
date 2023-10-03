#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv){
	printf("HTTP/1.1 200 OK\n");
	printf("Content-type: text/html\n");
	printf("\n");
	printf("<html>\n");
	printf("<body>\n");
	printf("<h1>Upper</h1>\n");
	if (argc >= 2){
		char *s = argv[1];
		while (*s){
			putchar(toupper(*s));
			s++;
		}
	}
	printf("nothing input");
	printf("</body>\n");
	printf("</html>\n");
}