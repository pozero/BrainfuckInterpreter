#include <stdio.h>
#include <stdlib.h>

#define MEMORY 1000
#define BUFMAX 50

typedef struct s
{
    unsigned buf[BUFMAX];
    unsigned ptr;
}stack;

void push(stack *s, unsigned data)
{
    s->buf[++s->ptr] = data;
}

unsigned pop(stack* s)
{
    return s->buf[s->ptr--];
}

typedef struct d
{
    unsigned name[BUFMAX];
    unsigned key[BUFMAX];
    unsigned size;
}dictionary;

void addNameKeyPair(dictionary* d, unsigned n, unsigned k)
{
    ++d->size;
    d->name[d->size] = n;
    d->key[d->size] = k;
}

unsigned* findKeyByName(dictionary* d, unsigned name)
{
    unsigned left = 1u;
    unsigned right = d->size;
    unsigned mid = (left + right) / 2;
    do
    {
        if(d->name[mid] > name)
        {
            right = mid;
        }
        else if (d->name[mid] < name)
        {
            left = mid;
        }
        else
        {
            return &(d->key[mid]);
        }
        mid = (left + right) / 2;
    } while (right - left > 1);

    if (d->name[left] == name) { return &(d->key[left]);}
    if (d->name[right] == name) { return &(d->key[right]);}
    return NULL;
}

unsigned* findNameByKey(dictionary* d, unsigned k)
{
    for (unsigned i = 1; i <= d->size; i ++)
    {
        if (d->key[i] == k) { return &(d->name[i]); }
    }
}

int main(int argc, char *argv[])
{
    unsigned char *mem = calloc(MEMORY, sizeof(unsigned char));
    unsigned char *ptr = mem;
    
    FILE* fp;
    if ((fp = fopen(argv[1], "rb")) == 0)
    {
       printf("Fail to open source file, please check your path.\n"); 
       free(mem);
       return -1; 
    }
    char instruction = 0;
    const long bytePerChar = sizeof(char);      // questionable implementation
    stack s;
    dictionary bracketTable;
    s.ptr = bracketTable.size = 0u;
    for (unsigned i = 0; (instruction = fgetc(fp)) != EOF; i ++)
    {
        unsigned* keyptr = NULL;
        unsigned left = 0u;
        switch (instruction)
        {
        case '[':
            push(&s, i);
            break;
        case ']':
            left = pop(&s);
            addNameKeyPair(&bracketTable, i, left);
            break;
        default:
            break;
        }
    }
    fseek(fp, 0, SEEK_SET);

    for (unsigned i = 0; (instruction = fgetc(fp)) != EOF; i ++)
    {
        unsigned* anotherBracket = NULL;
        switch (instruction)
        {
        case '>':
            ++ptr;
            break;
        case '<':
            --ptr;
            break;
        case '+':
            ++*ptr;
            break;
        case '-':
            --*ptr;
            break;
        case '.':
            putchar(*ptr);
            break;
        case ',':
            *ptr = getchar();
            break;
        case '[':
            if (*ptr == 0)
            {
                anotherBracket = findNameByKey(&bracketTable, i);
                fseek(fp, bytePerChar * (*anotherBracket - i), SEEK_CUR);
                i += *anotherBracket - i;
            }
            break;
        case ']':
            anotherBracket = findKeyByName(&bracketTable, i);
            fseek(fp, -bytePerChar * (long) (i - *anotherBracket + 1u), SEEK_CUR);
            i -= (i - *anotherBracket + 1u);
            break;
        default:
            break;
       }
    }

exit:
    fclose(fp);
    free(mem);
    return 0;
}