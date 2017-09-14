#include <stdio.h>
#include <stdlib.h>


int main()
{
   puts("jashh");
   char str1[10], str2[10], str3[10];
   char name[20], value[20];
   char line[80];
   int year;
   FILE * fp;

   fp = fopen ("smi-server.conf", "w+");
   fputs("global=1", fp);
   fputs("# comment", fp);
   fputs("", fp);
   fputs("[Person]", fp);
   fputs(" name=Max", fp);
   fputs(" surname=Mustermann", fp);
   fputs("[car]", fp);
   fputs(" name=Ferrari", fp);
   fputs(" model=F40", fp);

   rewind(fp);

//   fscanf(fp, "%s = %s", str1, str2);
   printf("Read String1 |%s|\n", str1 );
   printf("Read String2 |%s|\n\n", str2 );

//   fscanf(fp, "%s = %s", str1, str2);
   printf("Read String1 |%s|\n", str1 );
   printf("Read String2 |%s|\n\n", str2 );

//   fscanf(fp, "%s = %s", str1, str2);
   printf("Read String1 |%s|\n", str1 );
   printf("Read String2 |%s|\n\n", str2 );

//   rewind(fp);

    sscanf(line, "%s %s", name, value);
    printf("Read: |%s|%s|\n\n", name, value );

	sscanf(line, "%s %s", name, value);
	printf("Read: |%s|%s|\n\n", name, value );

	sscanf(line, "%s %s", name, value);
	printf("Read: |%s|%s|\n\n", name, value );
	fclose(fp);

   return(0);
}
