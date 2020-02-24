#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define MAXOP   100    /* max size of operand/operator */
#define NUMBER '0'     /* signal that a number was found */
#define MAXVAL  100

size_t sp = 0;   // aka unsigned long -- printf using %zu
double val[MAXVAL];   // stack of values

char buf[BUFSIZ];
size_t bufp = 0;

int getch_(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }
void ungetch_(int c) {
  if (bufp >= BUFSIZ) { fprintf(stderr, "ungetch: too many characters\n");  return; }
  buf[bufp++] = c;
}

int getop(char* s) {
  int i, c, next;
  while ((s[0] = c = getch_()) == ' ' || c == '\t') { }  // skip whitespace
  s[1] = '\0';

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
      case 's':     push(sin(pop()));           break;
      case 'c':     push(cos(pop()));           break;
      case 't':     push(tan(pop()));           break;
      case '^':
        if ((op2 = pop()) == 0.0) {push(1);     break;}
        if ((op3 = pop()) == 0.0) {push(0.0);   break;}
        else
                    push(pow(op3, op2));        break;
      case 'e':     push(exp(pop()));           break;
      default:      fprintf(stderr, "unknown command %s\n", s);  break;
    }
  }
}

int main(int argc, const char * argv[]) {
  rpn();

  return 0;
}
