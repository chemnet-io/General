#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define for_each_item(item, list) \
    for(T * item = list->head; item != NULL; item = item->next)

struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

int main(void)
{
  FILE *fp;
  FILE *fpread;
  CURL *curl;
  CURLcode res;
  char urlfront[75] = "http://classyfire.wishartlab.com/entities/";
  char urlback[25] = ".json";
  char inchi[50];
  char uri[200];

  fpread = fopen("inchikeys.txt", "r");

  if (NULL == fpread) {
    printf("file can't be opened \n");
  }

  fgets(inchi, 50, fpread);
  printf("%s\n", inchi);

  fclose(fpread);

  strcpy(uri, urlfront);
  strcat(uri, inchi);
  strcat(uri, urlback);

  curl = curl_easy_init();
  if(curl) {
    struct string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, uri);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);

    if((fp=freopen("OUT.json", "w" ,stdout))==NULL) {
        printf("Cannot open file.\n");
        exit(1);
    }
    printf("%s\n", s.ptr);

    fclose(fp);

    free(s.ptr);

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}
