#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int main(const int argc, const char* const* const argv) {
  int n = 0;
  printf("Input unsigned digit: ");
  scanf("%d", &n);
  char* string = calloc(n, sizeof(char));
 if (!string) {
    fprintf(stderr, "ERROR\n");
    return 1;
  }
  printf("%c\n", string[n-1]);
  free(string);
  return 0;
}