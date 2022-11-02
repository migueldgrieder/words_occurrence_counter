// Compilar: gcc -fopenmp -o run_openmp_contador openmp_contador.c
// Executar: ./run_openmp_contador

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

#define NUM_STRINGS 64
#define NUM_THREADS 8

struct timeval t1;
struct timeval t2;
char *read_file();
int *threading_contador();
void contador();

int main() {

  char * texto = read_file();
  if (texto == NULL) {
    printf("Error ao ler o arquivo txt.\n");
    return 1;
  }

  int lenTexto, contador;
  lenTexto = strlen(texto);
  printf("Tamanho do texto: %d\n", lenTexto);

  char *palavra_chave[NUM_STRINGS] = {"Sherlock", "England", "Pizza", "gentlemen", "Sherlock",
    "Holmes", "blanche", "address", "Brazil", "10", "What", "understand", "field", "CHAPTER",
    "Copernicus", "Napoleon", "produced", "administration", "president", "20", "hotel", "Little",
    "  ", "Matvevna", "staff officer", "screaming", "aaaaaaa", "twice", "Captain", "30", "Tushin",
    " never ", " ever ", "successfully", "seizing", "battery", "Only", "only", " only ", "40",
    "glee", "among", "Ekonomov", "battles", "shouting", "1917", "government", "Senate", "insubordination",
    "50", "violating", "war", "world-wide", "American", "Article X", "Drubetskaya", "Dmitri", "Razumovski",
    "poor", "60", "rich", " tea", " coffe", "last"};

  gettimeofday( & t1, NULL);
  clock_t start_time = clock();

  int* ocorrencias_arr = threading_contador(texto, palavra_chave, lenTexto);

  gettimeofday( & t2, NULL);
  clock_t end_time = clock();

  // printf("Tempo de CPU: %lf\n", (float)(end_time - start_time) / CLOCKS_PER_SEC);
  double t_total = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec) / 1000000.0);
  printf("Tempo absoluto: %f\n", t_total);

  // for (int i = 0; i < NUM_STRINGS; i++) {
  //   printf("ocorrencia(%s) =  %d\n", palavra_chave[i], ocorrencias_arr[i]);
  // }

  free(texto);
  return 0;
}


int* threading_contador(char *texto, char **palavra_chave, int lenTexto){
  static int ocorrencias_arr[NUM_STRINGS];
  int ratio = lenTexto / NUM_THREADS;
  #pragma omp parallel for schedule(guided) num_threads(NUM_THREADS)
  for (int i = 0; i < lenTexto; i++) {
    contador(texto, palavra_chave, lenTexto, i, ocorrencias_arr);
  }
  return ocorrencias_arr;
}


void contador(char *texto, char **palavra, int lenTexto, int i, int* ocorrencias_arr)
{ 
  int thread_ocorrencias_arr[NUM_STRINGS];

  for (int rep = 0; rep < NUM_STRINGS; ++rep) {
    int j, temp, chk, lenPalavra, contador = 0;
    lenPalavra = strlen(palavra[rep]);
    temp = i;
    for (j = 0; j < lenPalavra; j++) {
      if (texto[i] == palavra[rep][j])
        i++;
    }
    chk = i - temp;
    if (chk == lenPalavra)
      contador++;
    i = temp;
    thread_ocorrencias_arr[rep] = contador;
  }
    #pragma omp critical
    for (int i = 0; i < NUM_STRINGS; i++) {
      ocorrencias_arr[i] += thread_ocorrencias_arr[i];
    }
}


char *read_file()
{
  FILE *file;
  
  file = fopen("sherlock.txt", "r");
  
  if(file == NULL) return NULL;
  fseek(file, 0, SEEK_END); 

  int length = ftell(file);

  fseek(file, 0, SEEK_SET);
  
  char *string = malloc(sizeof(char) * (length+1));
  
  char c;

  int i = 0;

  while ( (c = fgetc(file)) != EOF)
  {
    string[i] = c;
    i++;
  }

  string[i] = '\0';
  fclose(file);
    return string;
}