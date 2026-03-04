#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imageprocessing.h"
#include "bmp.h"


typedef struct image {
    int N, M;
    int ***idx;
} image;

typedef struct filter {
    int size;
    float **idx;
} filter;

void eliberaref(float **filtru, int size) {
    for (int i = 0; i < size; i++) {
        free(filtru[i]);
    }

    free(filtru);
    }

int main() {
// TODO(gabriela) :Task 7
        image *images = 0;
        filter *filters = 0;
        const int kCommandSize = 5;
        const int kMaxPathLength = 100;
        int indexi = 0, indexf = 0;
        char command[kCommandSize];
        scanf("%s", command);
    while (strcmp(command, "e") != 0) {
        if (strcmp(command, "l") == 0) {
            int N = 0, M = 0;
            char path[kMaxPathLength];
            scanf("%d %d %s", &N, &M, path);
            int ***matrice = alocare(N, M);
            read_from_bmp(matrice, N, M, path);
            indexi++;
            images = realloc(images, sizeof(image) * indexi);  // cream loc pentru urmatoarea imagine
            images[indexi - 1].N = N;
            images[indexi - 1].M = M;
            images[indexi - 1].idx = matrice;  // dam imaginii de la indexul curent valorile date
        }

        if (strcmp(command, "s") == 0) {
            int index = 0;
            char path[kMaxPathLength];
            scanf("%d", &index);
            scanf("%s", path);
            write_to_bmp(images[index].idx, images[index].N, images[index].M, path);
        }

        if (strcmp(command, "ah") == 0) {
            int index = 0;
            scanf("%d", &index);
            images[index].idx = flip_horizontal(images[index].idx, images[index].N, images[index].M);
        }

        if (strcmp(command, "ar") == 0) {
            int index = 0;
            scanf("%d", &index);
            image i = images[index];
            int ***aux = rotate_left(i.idx, i.N, i.M);
                images[index].idx = aux;
                images[index].N = i.M;
                images[index].M = i.N;
            }

        if (strcmp(command, "ac") == 0) {
            int x = 0, y = 0, h = 0, w = 0, index = 0;
            scanf("%d %d %d %d %d", &index, &x, &y, &w, &h);
            int ***aux = crop(images[index].idx, images[index].N, images[index].M, x, y, h, w);
            images[index].idx = aux;
            images[index].N = h;
            images[index].M = w;
        }

        if (strcmp(command, "ae") == 0) {
            int index = 0, rows = 0, cols = 0, R = 0, G = 0, B = 0;
            scanf(" %d %d %d %d %d %d", &index, &rows, &cols, &R, &G, &B);
            int ***aux = extend(images[index].idx, images[index].N, images[index].M, rows, cols, R, G, B);
            images[index].idx = aux;
            images[index].N = 2*rows + images[index].N;
            images[index].M = 2*cols + images[index].M;
        }

        if (strcmp(command, "ap") == 0) {
            int index_dst = 0, index_src = 0, x = 0, y = 0;
            scanf("%d %d %d %d", &index_dst, &index_src, &x, &y);
            image i = images[index_dst];
            int ***aux = paste(i.idx, i.N, i.M, images[index_src].idx, images[index_src].N, images[index_src].M, x, y);
            i.idx = aux;
        }

        if (strcmp(command, "cf") == 0) {
            int size = 0;
            scanf("%d", &size);
            filter f;
            f.size = size;
            f.idx = malloc( size * sizeof(float *));
            for (int i = 0; i < size; i++) {
                    f.idx[i] = malloc( size * sizeof(float));
                    for (int j = 0; j < size; j++)
                        scanf("%f", &f.idx[i][j]);
            }
            indexf++;
            filters = realloc(filters, sizeof(filter) * indexf);  // cream loc pentru urmatorul filtru
            filters[indexf-1] = f;  // dam filtrului de la indexul curent valorile date
        }

        if (strcmp(command, "af") == 0) {
            int index_i = 0, index_f = 0;
            scanf("%d %d", &index_i, &index_f);
            image i = images[index_i];
            int ***aux = apply_filter(i.idx, i.N, i.M, filters[index_f].idx, filters[index_f].size);
            images[index_i].idx = aux;
            }

        if (strcmp(command, "df") == 0) {
            int index_filter = 0;
            scanf("%d", &index_filter);
            eliberaref(filters[index_filter].idx, filters[index_filter].size);
            indexf--;
            for (int i = index_filter; i < indexf; i++)
                filters[i] = filters[i+1];  // indexful fiecarului filtru dupa cel sters se decaleaza
            }

        if (strcmp(command, "di") == 0) {
            int index_img = 0;
            scanf("%d", &index_img);
            eliberare(images[index_img].idx, images[index_img].N, images[index_img].M);
            indexi--;
            for (int i = index_img; i < indexi; i++)
            images[i] = images[i+1];  // indexful fiecarei imagini dupa cea stearsa se decaleaza
        }
        scanf("%s", command);
    }
    for (int i = 0; i < indexi; i++)
        eliberare(images[i].idx, images[i].N, images[i].M);
    for (int i = 0; i < indexf; i++)
        eliberaref(filters[i].idx, filters[i].size);
    free(images);
    free(filters);
    return 0;
}

