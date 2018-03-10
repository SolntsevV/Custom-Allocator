#include <mem.h>

int getPositiveInt (void);

int main(const int argc, char* argv[]) {
  int n = 0;
  printf("Input unsigned digit: ");
  scanf("%d", &n);
  char* string = _calloc(n, sizeof(char)); // 1 - количество элементов, 2 - размер
  if (!string) {
    fprintf(stderr, "Ошибка!\n");
    return 1;
  }
  printf("%c\n", string[n-1]);
  _free(string);
  return 0;
}

