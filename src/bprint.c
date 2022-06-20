#include <stdlib.h>
#include <stdio.h>
#include "bprint.h"
#include "btypes.h"

#define BDEF  "\x1B[0m"
#define BBLU  "\x1B[34m"
#define BGRE  "\x1B[32m"
#define BRED  "\x1B[31m"
#define BMAG  "\x1b[35m"

void bshow_error(beorn_state* curr) {
  const char * et = berrors_to_str(curr->et);
  printf("%s%s: %s%s", BRED, et, curr->cval, BDEF);
}

void bprint_pack(beorn_state* curr) {
  putchar('{');
  for (int i = 0; i < curr->length; i++) {
    bprint(curr->child[i]);
    if (i + 1 < curr->length) putchar(' ');
  }
  putchar('}');
}

void bprint_list(beorn_state* curr) {
  putchar('[');
  for (int i = 0; i < curr->length; i++) {
    bprint(curr->child[i]);
    if (i + 1 < curr->length) putchar(' ');
  }
  putchar(']');
}

void bprint_expression(beorn_state* curr) {
    putchar('(');
    for (int i = 0; i < curr->length; i++) {
      bprint(curr->child[i]);
      if (i + 1 < curr->length) putchar(' ');
    }
    putchar(')');
}

void bprint(beorn_state* curr) {
  switch (curr->type) {
    case BT_INTEGER:      printf("%ld", curr->ival);    break;
    case BT_FLOAT:        printf("%Lf", curr->fval);    break;
    case BT_STRING:       printf("\"%s\"", curr->cval); break;
    case BT_SYMBOL:       printf("%s",  curr->cval);    break;
    case BT_FUNCTION:     printf("#function<>");        break;
    case BT_LAMBDA:       printf("#lambda<>");          break;
    case BT_PACK:         bprint_pack(curr);            break;
    case BT_LIST:         bprint_list(curr);            break;
    case BT_EXPRESSION:   bprint_expression(curr);      break;
    case BT_ERROR:        bshow_error(curr);            break;
    
    default:
      bshow_error(new_error(BRUNTIME_ERROR, "unspected token."));
      break;
  }

  free(curr);
}
