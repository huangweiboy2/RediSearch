#include "../stemmer.h"
#include "../tokenize.h"
#include "test_util.h"
#include "../rmutil/alloc.h"
#include <string.h>

int testStemmer() {

  Stemmer *s = NewStemmer(SnowballStemmer, "en");
  ASSERT(s != NULL)

  size_t sl;
  const char *stem = s->Stem(s->ctx, "arbitrary", (size_t)strlen("arbitrary"), &sl);
  ASSERT(stem != NULL)
  ASSERT(!strcasecmp(stem, "arbitrari"));
  ASSERT(sl == strlen(stem));
  printf("stem: %s\n", stem);

  // free((void*)stem);
  s->Free(s);
  return 0;
}

typedef struct {
  int num;
  char **expected;

} tokenContext;

int tokenFunc(void *ctx, const Token *t) {
  // printf("%s %d\n", t.s, t.type);

  tokenContext *tx = ctx;
  int ret = strcmp(t->s, tx->expected[tx->num++]);
  assert(ret == 0);
  assert(t->len == strlen(t->s));
  assert(t->fieldId == 1);
  assert(t->pos > 0);
  assert(t->score == 1);
  if (t->type == DT_STEM) {
    // printf("%s -> %s\n",t.s, tx->expected[tx->num-2]);
    assert(strcmp(t->s, tx->expected[tx->num - 2]) != 0);
  }
  return 0;
}

int testTokenize() {

  char *txt = strdup("Hello? world... worlds going ? -WAZZ@UP? שלום");
  tokenContext ctx = {0};
  const char *expected[] = {"hello", "world", "worlds", "world", "going",
                            "go",    "wazz",  "up",     "שלום"};
  ctx.expected = (char **)expected;

  Stemmer *s = NewStemmer(SnowballStemmer, "en");
  ASSERT(s != NULL)

  tokenize(txt, 1, 1, &ctx, tokenFunc, s, 0, DefaultStopWordList());
  ASSERT(ctx.num == 9);

  free(txt);

  return 0;
}

TEST_MAIN({
  RMUTil_InitAlloc();
  TESTFUNC(testStemmer);
  TESTFUNC(testTokenize);
});