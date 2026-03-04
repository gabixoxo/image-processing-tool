# Image Processing CLI (BMP) – editor simplu in C

## 1. Descriere generala

Acest proiect implementeaza un **editor de imagini BMP** (24-bit RGB) in C, controlat prin comenzi citite de la `stdin`.
Programul poate incarca mai multe imagini in memorie, poate defini mai multe filtre si poate aplica operatii de prelucrare:

- flip orizontal
- rotire la stinga (90°)
- crop (decupare)
- extend (bordare / extindere cu o culoare data)
- paste (lipire imagine-sursa peste destinatie)
- aplicare filtru (convolutie)
- salvare BMP
- stergere imagini / filtre din memorie

Implementarea este modularizata astfel:
- `imageprocessing.c/.h` – operatii pe matricea de pixeli (RGB)
- `bmp.c/.h` – citire/scriere BMP (functii `read_from_bmp`, `write_to_bmp`)
- `interactive.c` – interfata CLI (interpreteaza comenzile si gestioneaza listele de imagini/filtre)

> Observatie: Imaginea este reprezentata ca **matrice 3D** `int***` cu dimensiuni `[N][M][3]`, unde `N` = inaltime, `M` = latime, iar ultimul indice reprezinta canalele `R, G, B`.

---


## 2. Reprezentari si structuri de date

### 2.1. Reprezentarea unei imagini (pixel matrix)

In `imageprocessing.c` o imagine este:
- `int*** image`
- `image[i][j][0]` = R
- `image[i][j][1]` = G
- `image[i][j][2]` = B

Alocarea este pe 3 nivele:
1. vector de linii `image[N]`
2. pentru fiecare linie, vector de coloane `image[i][M]`
3. pentru fiecare pixel, vector de 3 int-uri (RGB)

### 2.2. Structuri folosite in `interactive.c`

In `interactive.c` apar doua structuri locale:

```c
typedef struct image {
    int N, M;
    int ***idx;
} image;

typedef struct filter {
    int size;
    float **idx;
} filter;
```

- `image` retine dimensiunile si pointerul catre matricea RGB.
- `filter` retine dimensiunea (ex: 3, 5) si o matrice 2D de coeficienti `float`.

---

## 3. Functii implementate in `imageprocessing.c`

### 3.1. `int modif(int nr)`
Clampeaza o valoare in intervalul [0, 255].
- daca `nr < 0` -> 0
- daca `nr > 255` -> 255
- altfel -> `nr`

Este folosita dupa aplicarea filtrului pentru a evita depasiri la canalele RGB.

---

### 3.2. `int*** alocare(int N, int M)`
Aloca dinamic o imagine de dimensiune `N x M`:
- aloca `N` linii
- pentru fiecare linie aloca `M` pixeli
- pentru fiecare pixel aloca 3 canale (RGB)

Returneaza `int***` catre matricea alocata.

---

### 3.3. `void eliberare(int ***image, int N, int M)`
Elibereaza corect memoria unei imagini `N x M` alocate cu `alocare`:
- elibereaza canalele fiecarui pixel
- elibereaza fiecare linie
- elibereaza vectorul principal

---

### 3.4. Task 1 – `int*** flip_horizontal(int ***image, int N, int M)`
Face flip pe orizontala (oglindire stinga-dreapta) **in-place**.
Algoritm:
- pentru fiecare linie `i`, schimba pixelii `j` cu `M - j - 1` pina la j < M/2
- schimba toate cele 3 canale

Returneaza acelasi pointer `image` (modificat).

Complexitate:
- timp: O(N * M)
- memorie suplimentara: O(1)

---

### 3.5. Task 2 – `int*** rotate_left(int ***image, int N, int M)`
Roteste imaginea la stinga cu 90°.
- aloca o noua matrice `M x N`
- copieaza pixelii cu mapare:
  - `matrice2[j][i] = image[i][M - j - 1]` (pe fiecare canal)
- elibereaza imaginea veche
- returneaza matricea noua

Dupa apel, dimensiunile se schimba:
- noul `N` devine vechiul `M`
- noul `M` devine vechiul `N`

Complexitate:
- timp: O(N * M)
- memorie: O(N * M) pentru matricea noua

---

### 3.6. Task 3 – `int*** crop(int ***image, int N, int M, int x, int y, int h, int w)`
Decupeaza un dreptunghi de dimensiune `h x w` din pozitia (x, y) (coltul stinga-sus).
- aloca o noua matrice `h x w`
- copiaza:
  - `matrice3[i][j] = image[y + i][x + j]`
- elibereaza imaginea originala
- returneaza imaginea decupata

Dupa apel, dimensiunile devin:
- `N = h`
- `M = w`

---

### 3.7. Task 4 – `int*** extend(int ***image, int N, int M, int rows, int cols, int new_R, int new_G, int new_B)`
Extinde imaginea adaugind o bordura:
- `rows` linii sus si jos
- `cols` coloane stinga si dreapta
- bordura este umpluta cu culoarea `(new_R, new_G, new_B)`

Algoritm:
1. calculeaza dimensiunile noi:
   - `l = N + 2*rows`
   - `c = M + 2*cols`
2. aloca `l x c` si initializeaza tot cu culoarea noua
3. copiaza imaginea veche in centru, offset `(rows, cols)`
4. elibereaza imaginea veche
5. returneaza matricea noua

---

### 3.8. Task 5 – `int*** paste(...)`
Semnatura:
```c
int ***paste(int ***image_dst, int N_dst, int M_dst,
             int ***image_src, int N_src, int M_src,
             int x, int y)
```

Lipeste `image_src` peste `image_dst` cu coltul stinga-sus la (x, y) in destinatie.

- daca `x`/`y` sint in afara destinatiei, intoarce `image_dst` nemodificat
- altfel, parcurge sursa si copiaza numai zona care incape in destinatie:
  - pentru `i` < N_src si `i+y` < N_dst
  - pentru `j` < M_src si `j+x` < M_dst
  - copiaza canalele RGB

Operatia este **in-place** pe destinatie (nu aloca memorie noua).

---

### 3.9. Task 6 – `int*** apply_filter(int ***image, int N, int M, float **filter, int filter_size)`
Aplica un filtru de tip **convolutie** (kernel `filter_size x filter_size`) pe fiecare canal RGB.

Algoritm:
- aloca o matrice noua `N x M` (rezultat)
- pentru fiecare pixel (i, j):
  - calculeaza suma ponderata pe vecinatate:
    - `sum_r += image[c][d][0] * filter[a][b]`
    - similar pentru G, B
  - daca vecinul (c, d) este in afara imaginii, contribuie cu 0 (zero-padding)
  - la final:
    - `rez[i][j][k] = modif((int)sum_k)` pentru fiecare canal

Elibereaza imaginea veche si returneaza rezultatul.

---



## 4. Interfata CLI – comenzi suportate (`interactive.c`)

Programul citeste comenzi din `stdin` pina la comanda `e` (exit).
Se lucreaza cu:
- o lista dinamica de imagini (`image *images`, marime `indexi`)
- o lista dinamica de filtre (`filter *filters`, marime `indexf`)

### 4.1. `l` – load image
Format:
```
l N M path
```
- aloca o imagine `N x M`
- citeste din BMP folosind `read_from_bmp(...)`
- adauga imaginea la final in vectorul `images` (prin `realloc`)

### 4.2. `s` – save image
Format:
```
s index path
```
- salveaza imaginea `images[index]` in fisier BMP cu `write_to_bmp(...)`

### 4.3. `ah` – apply horizontal flip
Format:
```
ah index
```
- aplica `flip_horizontal` pe imaginea data

### 4.4. `ar` – apply rotate left
Format:
```
ar index
```
- aplica `rotate_left`
- actualizeaza dimensiunile imaginii:
  - `N <- vechiul M`
  - `M <- vechiul N`

### 4.5. `ac` – apply crop
Format (conform codului citit):
```
ac index x y w h
```
Observatie: in cod se citesc `index, x, y, w, h`, dar functia `crop` primeste `(x, y, h, w)`.
Dupa crop:
- `N = h`
- `M = w`

### 4.6. `ae` – apply extend
Format:
```
ae index rows cols R G B
```
- extinde cu bordura `rows/cols` si culoarea (R,G,B)
- actualizeaza dimensiunile:
  - `N <- N + 2*rows`
  - `M <- M + 2*cols`

### 4.7. `ap` – apply paste
Format:
```
ap index_dst index_src x y
```
- lipeste imaginea `index_src` peste `index_dst` la (x, y)

### 4.8. `cf` – create filter
Format:
```
cf size
<matrice size x size de float-uri>
```

Exemplu:
```
cf 3
0 -1 0
-1 5 -1
0 -1 0
```

Filtrul este stocat in lista `filters`.

### 4.9. `af` – apply filter
Format:
```
af index_img index_filter
```
- aplica filtrul selectat pe imagine

### 4.10. `df` – delete filter
Format:
```
df index_filter
```
- elibereaza matricea filtrului cu `eliberaref`
- elimina filtrul din vector prin shift la stinga

### 4.11. `di` – delete image
Format:
```
di index_img
```
- elibereaza imaginea cu `eliberare`
- elimina imaginea din vector prin shift la stinga

### 4.12. `e` – exit
Opreste executia. Inainte de iesire, programul:
- elibereaza toate imaginile ramase
- elibereaza toate filtrele ramase
- elibereaza vectorii `images` si `filters`

---


## 5. Managementul memoriei

- Imaginile sint alocate cu `alocare` si eliberate cu `eliberare`.
- Operatiile `rotate_left`, `crop`, `extend`, `apply_filter`:
  - **aloca o imagine noua**
  - **elibereaza imaginea veche**
  - returneaza noul pointer
- `flip_horizontal` si `paste` modifica imaginea destinatie **in-place**.
- Filtrele sint alocate ca matrice 2D `float**` si eliberate cu `eliberaref`.
