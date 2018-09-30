// Copyright 2017 Stefan Adrian-Daniel (stefan.adrian1997@yahoo.com)

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

unsigned char* INITIALIZE(int N) {
    unsigned char *arena;
    if ((arena = (unsigned char*)calloc(N, sizeof(unsigned char))) == NULL) {
        printf("Error...!\n\n");
        exit(-1);
    }
    return arena;
}

void FINALIZE(unsigned char* arena) {
    free(arena);
    exit(0);
}

void DUMP(unsigned char *arena, int N) {
    int i = 0, a = 16;

    while (i < N) {
        printf("%08X", i);
        printf("\t");
        for ( ; i < a && i < N; i++) {
            if (a - i - 1 == 8) {
                printf("%02X  ", *(arena + i));
            } else {
                printf("%02X ", *(arena + i));
            }
        }
        a += 16;
        printf("\n");
    }
    printf("%08X\n", N);
}

void ALLOC(int N, unsigned char *arena, int SIZE) {
    unsigned char *blocu1, *blocu2;
    int *bloci1, *bloci2;
    int index, a, b;

    blocu1 = arena;
    bloci1 = (int*)blocu1;

    if (*bloci1 == 0) {
        if ((SIZE + 3 * sizeof(int)) <= (N - sizeof(int))) {
            *bloci1 = 4;
            blocu1 += 4;
            bloci1 = (int*)blocu1;
            *bloci1 = 0;
            *(bloci1 + 1) = 0;
            *(bloci1 + 2) = 3 * sizeof(int) + SIZE;
            printf("%zu\n", 4 * sizeof(int));
            return;
        }
        printf("0\n");
        return;
    }

    index = 0;
    a = *bloci1;
    b = sizeof(int);

    while (a != 0) {
        blocu2 = blocu1 + (a - index);
        bloci2 = (int*)blocu2;

        if ((3 * sizeof(int) + SIZE) <= (a - index - b)) {
            *bloci1 = index + b;
            blocu1 += b;
            bloci1 = (int*)blocu1;
            *bloci1 = a;
            *(bloci1 + 1) = index;
            *(bloci1 + 2) = 3 * sizeof(int) + SIZE;
            *(bloci2 + 1) = index + b;
            printf("%zu\n", index + b + 3 * sizeof(int));
            return;
        } else {
            index = a;
            blocu1 = blocu2;
            bloci1 = (int*)blocu1;
            a = *bloci1;
            b = *(bloci1 + 2);
        }
    }

    if ((3 * sizeof(int) + SIZE) <= (N - index - b)) {
        *bloci1 = index + b;
        blocu1 += b;
        bloci1 = (int*)blocu1;
        *bloci1 = 0;
        *(bloci1 + 1) = index;
        *(bloci1 + 2) = SIZE + 3 * sizeof(int);
        printf("%zu\n", index + b + 3 * sizeof(int));
        return;
    }
    printf("0\n");
}

void FREE(unsigned char *arena, int index) {
    int *help = (int*)(arena + index - 3 * sizeof(int));
    int x, y;

    x = *help;
    y = *(help + 1);

    help = (int*)(arena + y);
    *help = x;
    if (x != 0) {
        help = (int*)(arena + x);
        *(help + 1) = y;
    }
}

void FILL(unsigned char *arena, int index, int SIZE, int value) {
    int i;
    for (i = index; i < index + SIZE; i++) {
        *(arena + i) = value;
    }
}

void SHOW_FREE(unsigned char *arena, int N) {
    int nr_octeti_liberi = 0, nr_regiuni = 0, index = 0, l, a;
    l = sizeof(int);

    a = *((int*)arena);

    if (a == 0) {
        printf("1 blocks (%zu bytes) free\n", N - sizeof(int));
        return;
    }

    while (a != 0) {
        if (a - l - index > 0) {
            nr_octeti_liberi += a - l - index;
            nr_regiuni++;
        }
        l = *(((int*)(arena + a) + 2));
        index = a;
        a = *((int*)(arena + a));
    }

    if (N - index - l > 0) {
        nr_octeti_liberi += N - index - l;
        nr_regiuni++;
    }
    printf("%d blocks (%d bytes) free\n", nr_regiuni, nr_octeti_liberi);
}

void SHOW_USAGE(unsigned char *arena, int N) {
    int a, resd, nr_octeti_folositi, nr_octeti_rezervati;
    int nr_regiuni, index, l, nr_blocuri;
    float resf;
    char c = '%';

    nr_octeti_folositi = nr_regiuni = nr_blocuri = index = 0;
    nr_octeti_rezervati = sizeof(int);
    l = sizeof(int);

    a = *((int*)arena);
    if (a == 0) {
        printf("0 blocks (0 bytes) used\n");
        printf("0%c efficiency\n", c);
        printf("0%c fragmentation\n", c);
        return;
    }

    while (a != 0) {
        if (a - l - index > 0) {
            nr_regiuni++;
        }

        index = a;
        l = *(((int*)(arena + index) + 2));
        a = *((int*)(arena + index));
        nr_blocuri++;
        nr_octeti_rezervati += l;
        nr_octeti_folositi += l - 3 * sizeof(int);
    }
    if (N - index - l > 0) {
        nr_regiuni++;
    }

    printf("%d blocks (%d bytes) used\n", nr_blocuri, nr_octeti_folositi);
    resf = (nr_octeti_folositi * 1.0)/nr_octeti_rezervati;
    resd = (int)(resf * 100);
    printf("%d%c efficiency\n", resd, c);

    resd = nr_regiuni - 1;
    resf = (float)(resd * 1.0);
    resf = (float)resf/nr_blocuri;
    resf = resf * 100;
    resd = (int)resf;
    printf("%d%c fragmentation\n", resd, c);
}

void SHOW_ALLOCATIONS(unsigned char *arena, int N) {
    int index = 0, a, l;

    a = *((int*)arena);
    l = sizeof(int);

    while (a != 0) {
        printf("OCCUPIED %d bytes\n", l);
        if (a - l - index > 0) {
            printf("FREE %d bytes\n", a - l - index);
        }
        index = a;
        l = *((int*)(arena + a) + 2);
        a = *((int*)(arena + a));
    }
    printf("OCCUPIED %d bytes\n", l);
    if (N - index - l > 0) {
        printf("FREE %d bytes\n", N - index - l);
    }
}

int main() {
    int N, a, b, c;
    unsigned char *arena;
    char comanda[100], *p;

    while (fgets(comanda, 100, stdin) != NULL) {
        if (strchr(comanda, '\n')) {
            comanda[strlen(comanda) - 1] = '\0';
        }

        p = strtok(comanda, " ");

        if (strcmp(p, "INITIALIZE") == 0) {
            p = strtok(NULL, " ");
            N = atoi(p);
            arena = INITIALIZE(N);
        } else if (strcmp(p, "FINALIZE") == 0) {
            FINALIZE(arena);
        } else if (strcmp(p, "DUMP") == 0) {
            DUMP(arena, N);
        } else if (strcmp(p, "ALLOC") == 0) {
            p = strtok(NULL, " ");
            ALLOC(N, arena, atoi(p));
        } else if (strcmp(p, "FREE") == 0) {
            p = strtok(NULL, " ");
            FREE(arena, atoi(p));
        } else if (strcmp(p, "FILL") == 0) {
            p = strtok(NULL, " ");
            a = atoi(p);
            p = strtok(NULL, " ");
            b = atoi(p);
            p = strtok(NULL, " ");
            c = atoi(p);
            FILL(arena, a, b, c);
        } else if (strcmp(p, "SHOW") == 0) {
            p = strtok(NULL, " ");

            if (strcmp(p, "FREE") == 0) {
                SHOW_FREE(arena, N);
            } else if (strcmp(p, "USAGE") == 0) {
                SHOW_USAGE(arena, N);
            } else if (strcmp(p, "ALLOCATIONS") == 0) {
                SHOW_ALLOCATIONS(arena, N);
            }
        }
    }
    return 0;
}
