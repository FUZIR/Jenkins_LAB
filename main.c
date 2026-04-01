#include <stdio.h>
#include <ctype.h>
#include <string.h>

void process_string(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n' || str[i] == '\r') {
            str[i] = '\0';
            break;
        }

        if (i % 2 == 0) {
            str[i] = toupper((unsigned char)str[i]);
        } else {
            str[i] = tolower((unsigned char)str[i]);
        }
    }
}

int main() {
    char input[256];
    int variant = 1;

    printf("%d\n", variant);

    if (fgets(input, sizeof(input), stdin) != NULL) {
        process_string(input);

        printf("%s\n", input);
    }

    return 0;
}
