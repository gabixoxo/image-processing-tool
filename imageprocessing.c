#include <stdio.h>
#include <stdlib.h>
#include "imageprocessing.h"

int modif(int nr) {
    const int valoare = 255;
    if (nr < 0) {
        return 0;
    } else if (nr > valoare) {
        return valoare;
    } else {
        return nr;
    }
    }
int ***alocare(int N, int M) {
    int ***image = (int ***)malloc(N * sizeof(int **));
    for (int i = 0; i < N; i++) {
        image[i] = (int **)malloc(M * sizeof(int *));
        for (int j = 0; j < M; j++) {
            image[i][j] = (int *)malloc(3 * sizeof(int));
        }
    }
    return image;
}

void eliberare(int ***image, int N, int M) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            free(image[i][j]);
        }
        free(image[i]);
    }
    free(image);
}

// TODO(gabriela) :Task 1
int ***flip_horizontal(int ***image, int N, int M) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M / 2; j++) {
            for (int k = 0; k < 3; k++) {
                int aux = image[i][j][k];
                image[i][j][k] = image[i][M - j - 1][k];
                image[i][M - j - 1][k] = aux;
            }
        }
    }
    return image;
}

// TODO(gabriela) :Task 2
int ***rotate_left(int ***image, int N, int M) {
    int ***matrice2 = alocare(M, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            matrice2[j][i][0] = image[i][M-j-1][0];
            matrice2[j][i][1] = image[i][M-j-1][1];
            matrice2[j][i][2] = image[i][M-j-1][2];
            }
    }
    eliberare(image, N, M);
    return matrice2;
}

// TODO(gabriela) :Task 3
int ***crop(int ***image, int N, int M, int x, int y, int h, int w) {
    int ***matrice3 = alocare(h, w);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int k = 0; k < 3; k++) {
                matrice3[i][j][k] = image[y + i][x + j][k];
            }
        }
    }
    eliberare(image, N, M);
    return matrice3;
}


// TODO(gabriela) :Task 4
int ***extend(int ***image, int N, int M, int rows, int cols, int new_R, int new_G, int new_B) {
    int l = N + 2 * rows;
    int c = M + 2 * cols;
    int ***matrice4 = alocare(l, c);
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < c; j++) {
            matrice4[i][j][0] = new_R;
            matrice4[i][j][1] = new_G;
            matrice4[i][j][2] = new_B;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < 3; k++) {
                matrice4[i + rows][j + cols][k] = image[i][j][k];
            }
        }
    }
    eliberare(image, N, M);
    return matrice4;
}

// TODO(gabriela) :Task 5
int ***paste(int ***image_dst, int N_dst, int M_dst, int ***image_src, int N_src, int M_src, int x, int y) {
    if (y > N_dst || x > M_dst) {
    return image_dst;
    }
    for (int i = 0; i < N_src && i + y < N_dst; ++i) {
        for (int j = 0; j < M_src && j + x < M_dst; ++j) {
            for (int k = 0; k < 3; k++) {
            image_dst[i + y][j + x][k] = image_src[i][j][k];
        }
    }
}
return image_dst;
}

// TODO(gabriela) :Task 6
int ***apply_filter(int ***image, int N, int M, float **filter, int filter_size) {
    int ***matrice6 = alocare(N, M);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            float sum_r = 0, sum_g = 0, sum_b = 0;

            for (int a = 0; a < filter_size; a++) {
                for (int b = 0; b < filter_size; b++) {
                    int c = i - filter_size / 2 + a;
                    int d = j - filter_size / 2 + b;
        if (c >= 0 && c < N && d >= 0 && d < M) {
        sum_r += (float)image[c][d][0]*filter[a][b];
        sum_g += (float)image[c][d][1]*filter[a][b];
        sum_b += (float)image[c][d][2]*filter[a][b];
    } else {
        sum_r += 0;
        sum_g += 0;
        sum_b += 0;
            }
        }
    }
        matrice6[i][j][0] = modif((int)sum_r);
        matrice6[i][j][1] = modif((int)sum_g);
        matrice6[i][j][2] = modif((int)sum_b);
        }
    }
    eliberare(image, N, M);
    return matrice6;
    }
