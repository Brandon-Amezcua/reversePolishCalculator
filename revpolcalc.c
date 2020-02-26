#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "revpolcalc.h"

#define MAXOP   100    /* max size of operand/operator */
#define NUMBER '0'     /* signal that a number was found */
#define MAXVAL  100
#define MATH '1'
#define STACK '2'
#define VARIABLE '3'

size_t sp = 0;   // aka unsigned long -- printf using %zu
double val[MAXVAL];   // stack of values

char buf[BUFSIZ];
size_t bufp = 0;

double variables[MAXOP];

int getch_(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }

void ungetch_(int c) {
  if (bufp >= BUFSIZ) { fprintf(stderr, "ungetch: too many characters\n");  return; }
  buf[bufp++] = c;
}

void math(char* s) {
  //printf("this is the input string %s\n", s);
  double op1, op2, result = 0;
  //printf("\nentered math function\n");
  if (strcmp(s, "sin") == 0) {
    //printf("%f \n", sin(pop()));
    result = sin(pop());
    //printf("sin detected");
  } else if (strcmp(s, "cos") == 0) {
    result = cos(pop());
  } else if (strcmp(s, "tan") == 0) {
    result = tan(pop());
  } else if (strcmp(s, "asin") == 0) {
    result = asin(pop());
  } else if (strcmp(s, "acos") == 0) {
    result = acos(pop());
  } else if (strcmp(s, "sinh") == 0) {
    result = sinh(pop());
  } else if (strcmp(s, "cosh") == 0) {
    result = cosh(pop());
  } else if (strcmp(s, "exp") == 0) {
    result = exp(pop());
  } else {
    printf("Unknown math function\n");
  }
  push(result);
}

void build_string(char* s) {
  int c, i = 0;
  while (isalpha(s[++i] = c = getch_())) {}
  s[i] = '\0';
  //printf("this is s %s\n", s);
}

int getop(char* s) {
  int i, c, next;
  while ((s[0] = c = getch_()) == ' ' || c == '\t') { }  // skip whitespace
  s[1] = '\0';
  //printf("c = %c\n", c);
  //printf("s = %s\n", s);
  if (c == '@') { build_string(s); return STACK;}
  if (c == '=' || c == '?') {build_string(s); return VARIABLE;}
  if (isalpha(c)) { build_string(s); return MATH;}

  if (!isdigit(c) && c != '.' && c != '-') { return c; }  // if not a digit, return
  if(c == '-')
  {
      next = getch_();
      if(!isdigit(next) && next != '.')
      {
         return c;
      }
      c = next;
  }
  else
  {
      c = getch_();
  }
  while(isdigit(s[++i] = c)) /* before the decimal*/
      c = getch_();
  if(c == '.')                     /* after the decimal*/
      while(isdigit(s[++i] = c = getch_()))
                  ;

  s[i] = '\0';
  if (c != EOF) { ungetch_(c); }
  return NUMBER;      // return type is NUMBER, number stored in s
}

double pop(void) {
  if (sp == 0) { fprintf(stderr, "stack underflow\n");  return 0.0; }
  return val[--sp];
}

void push(double f) {
  if (sp == MAXVAL) { fprintf(stderr, "stack overflow -- can't push %g\n", f);  return; }
  val[sp++] = f;
}

void peek(void){
  if (sp > 0)
    printf("Top of stack contains: %8g\n", val[sp - 1]);
}

void dupe(void) {
  double temp = pop();

  push(temp);
  push(temp);
}

void swap(void) {
  double first = pop();
  double second = pop();

  push(first);
  push(second);
}

void clear(void) {
  sp = 0;
}

void variable (char* s) {
  if (*s == '=') {
    variables[*++s - 'A'] = pop();
  } else if (*s == '?') {
    push(variables[*++s - 'A']);
  }
}

void stack(char* s) {
  ++s;
  if (strcmp(s, "swap") == 0) {
    swap();
  } else if (strcmp(s, "clear") == 0) {
    clear();
  } else if (strcmp(s, "dupe") == 0) {
    dupe();
  } else if (strcmp(s, "peek") == 0) {
    peek();
  }
}

void rpn(void) {
  int type;
  double op2;
  double op3;
  char s[BUFSIZ];

  while ((type = getop(s)) != EOF) {
    switch(type) {
      case '\n':    printf("\t%.8g\n", pop());  break;
      case NUMBER:  push(atof(s));              break;
      case '+':     push(pop() + pop());        break;
      case '*':     push(pop() * pop());        break;
      case '-':     push(-(pop() - pop()));     break;
      case '/':
        if ((op2 = pop()) == 0.0) { fprintf(stderr, "divide by zero\n");  break; }
        push(pop() / op2);
        break;
      case '%':     push(fmod(pop(), pop()));   break;
      case '!':     peek();                     break;
      case '^':
        if ((op2 = pop()) == 0.0) {push(1);     break;}
        if ((op3 = pop()) == 0.0) {push(0.0);   break;}
        else
                    push(pow(op3, op2));        break;
      case MATH:    math(s);                    break;
      case STACK:   stack(s);                   break;
      case VARIABLE: variable(s);               break;
      default:      fprintf(stderr, "unknown command %s\n", s);  break;
    }
  }
}

int main(int argc, const char * argv[]) {
  rpn();

  return 0;
}
