#include <stdio.h>
#include <stdlib.h>

int main(int args, char *argv[]) {
    if (args != 2) return -1;

    char *contents;
    {
        const char *filename = argv[1];
        
        FILE *f = fopen(filename, "r");
        if (!f) {
            printf("Failed to open file");
            return -1;
        }

        fseek(f, 0, SEEK_END);
        const unsigned int size = ftell(f);
        fseek(f, 0, SEEK_SET);

        contents = (char *)malloc(size+1);
        fread(contents, sizeof(char), size, f);
        fclose(f);
        contents[size] = 0;
    }

    printf("%s", contents);

    free(contents);
}