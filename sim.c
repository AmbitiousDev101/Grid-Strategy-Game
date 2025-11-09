#include <stdio.h>
#include <stdlib.h>
#include "sim_header.h"



int *p1_x, *p1_y, *p1_w, *p1_h, ***p1_occ; 
int *p2_x, *p2_y, *p2_w, *p2_h, ***p2_occ;
int p1_count = 0, p2_count = 0;
int grid_x, grid_y, grid_width, grid_height, **grid;
int p1_tracking = 0, p2_tracking = 0, p1_tx, p1_ty, p2_tx, p2_ty;

void read_cities(int player) {
    char line[1000];
    int cap = 10, count = 0;
    int *cx = malloc(cap * sizeof(int));
    int *cy = malloc(cap * sizeof(int));
    int *cw = malloc(cap * sizeof(int));
    int *ch = malloc(cap * sizeof(int));
    int ***occ = malloc(cap * sizeof(int **));

    while (1) {
        fgets(line, 1000, stdin);
        if (line[0] == 'd') break;

        int x, y, w, h;
        sscanf(line, "(%d, %d), %d, %d", &x, &y, &w, &h);

        if (count >= cap) {
            cap *= 2;
            cx = realloc(cx, cap * sizeof(int));
            cy = realloc(cy, cap * sizeof(int));
            cw = realloc(cw, cap * sizeof(int));
            ch = realloc(ch, cap * sizeof(int));
            occ = realloc(occ, cap * sizeof(int **));
        }

        cx[count] = x;
        cy[count] = y;
        cw[count] = w;
        ch[count] = h;
        occ[count] = malloc(h * sizeof(int *));
        for (int i = 0; i < h; i++) {
            occ[count][i] = malloc(w * sizeof(int));
            for (int j = 0; j < w; j++) {
                occ[count][i][j] = 0;
            }
        }
        count++;
    }

    if (player == 1) {
        p1_x = cx; p1_y = cy; p1_w = cw; p1_h = ch; p1_occ = occ; p1_count = count;
    } else {
        p2_x = cx; p2_y = cy; p2_w = cw; p2_h = ch; p2_occ = occ; p2_count = count;
    }
}

void calc_grid() {
    int min_x = p1_x[0], min_y = p1_y[0];
    int max_x = p1_x[0] + p1_w[0] - 1, max_y = p1_y[0] + p1_h[0] - 1;

    for (int i = 0; i < p1_count; i++) {
        if (p1_x[i] < min_x) min_x = p1_x[i];
        if (p1_y[i] < min_y) min_y = p1_y[i];
        if (p1_x[i] + p1_w[i] - 1 > max_x) max_x = p1_x[i] + p1_w[i] - 1;
        if (p1_y[i] + p1_h[i] - 1 > max_y) max_y = p1_y[i] + p1_h[i] - 1;
    }
    for (int i = 0; i < p2_count; i++) {
        if (p2_x[i] < min_x) min_x = p2_x[i];
        if (p2_y[i] < min_y) min_y = p2_y[i];
        if (p2_x[i] + p2_w[i] - 1 > max_x) max_x = p2_x[i] + p2_w[i] - 1;
        if (p2_y[i] + p2_h[i] - 1 > max_y) max_y = p2_y[i] + p2_h[i] - 1;
    }

    grid_x = min_x; grid_y = min_y;
    grid_width = max_x - min_x + 1;
    grid_height = max_y - min_y + 1;

    grid = malloc(grid_height * sizeof(int *));
    for (int i = 0; i < grid_height; i++) {
        grid[i] = malloc(grid_width * sizeof(int));
        for (int j = 0; j < grid_width; j++) grid[i][j] = 0;
    }
}

int in_own(int p, int x, int y) {
    int *cx = (p == 1) ? p1_x : p2_x;
    int *cy = (p == 1) ? p1_y : p2_y;
    int *cw = (p == 1) ? p1_w : p2_w;
    int *ch = (p == 1) ? p1_h : p2_h;
    int cnt = (p == 1) ? p1_count : p2_count;

    for (int i = 0; i < cnt; i++) {
        if (x >= cx[i] && x < cx[i] + cw[i] && y >= cy[i] && y < cy[i] + ch[i]) return 1;
    }
    return 0;
}

int in_opp(int p, int x, int y) {
    int opp = (p == 1) ? 2 : 1;
    int *cx = (opp == 1) ? p1_x : p2_x;
    int *cy = (opp == 1) ? p1_y : p2_y;
    int *cw = (opp == 1) ? p1_w : p2_w;
    int *ch = (opp == 1) ? p1_h : p2_h;
    int cnt = (opp == 1) ? p1_count : p2_count;

    for (int i = 0; i < cnt; i++) {
        if (x >= cx[i] && x < cx[i] + cw[i] && y >= cy[i] && y < cy[i] + ch[i]) return i;
    }
    return -1;
}

int is_mono(int p, int idx) {
    int opp = (p == 1) ? 2 : 1;
    int h = (opp == 1) ? p1_h[idx] : p2_h[idx];
    int w = (opp == 1) ? p1_w[idx] : p2_w[idx];
    int **occ = (opp == 1) ? p1_occ[idx] : p2_occ[idx];

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if (occ[i][j] == 0) return 0;
        }
    }
    return 1;
}

int all_mono(int p) {
    int opp = (p == 1) ? 2 : 1;
    int cnt = (opp == 1) ? p1_count : p2_count;
    for (int i = 0; i < cnt; i++) {
        if (!is_mono(p, i)) return 0;
    }
    return 1;
}

void salesperson_report(int player, int x, int y, int *n, int *e, int *s, int *w) {
    *n = *e = *s = *w = 0;
    int tx = -1, ty = -1;

    if ((player == 1 && p1_tracking) || (player == 2 && p2_tracking)) {
        tx = (player == 1) ? p1_tx : p2_tx;
        ty = (player == 1) ? p1_ty : p2_ty;
    } else {
        int opp = (player == 1) ? 2 : 1;
        int *cx = (opp == 1) ? p1_x : p2_x;
        int *cy = (opp == 1) ? p1_y : p2_y;
        int *cw = (opp == 1) ? p1_w : p2_w;
        int *ch = (opp == 1) ? p1_h : p2_h;
        int ***occ = (opp == 1) ? p1_occ : p2_occ;
        int cnt = (opp == 1) ? p1_count : p2_count;

        int min_d = -1;
        int bx = -1, by = -1;
        int bprio = -1;
        int bhoff = -1;

        for (int i = 0; i < cnt; i++) {
            for (int dy = 0; dy < ch[i]; dy++) {
                for (int dx = 0; dx < cw[i]; dx++) {
                    if (occ[i][dy][dx] == 0) {
                        int dx_pos = cx[i] + dx;
                        int dy_pos = cy[i] + dy;
                        int d = abs(dx_pos - x) + abs(dy_pos - y);
                        int pr = 0;
                        if (dy_pos > y && dx_pos == x) pr = 8; // N
                        else if (dx_pos > x && dy_pos == y) pr = 7; // E
                        else if (dy_pos < y && dx_pos == x) pr = 6; // S
                        else if (dx_pos < x && dy_pos == y) pr = 5; // W
                        else if (dy_pos < y && dx_pos < x) pr = 4; // SW
                        else if (dy_pos < y && dx_pos > x) pr = 3; // SE
                        else if (dy_pos > y && dx_pos < x) pr = 2; // NW
                        else if (dy_pos > y && dx_pos > x) pr = 1; // NE
                        int hoff = abs(dx_pos - x);
                        if (min_d == -1 || d < min_d
                            || (d == min_d && pr > bprio)
                            || (d == min_d && pr == bprio && hoff < bhoff)) {
                            min_d = d;
                            bx = dx_pos; by = dy_pos;
                            bprio = pr;
                            bhoff = hoff;
                        }
                    }
                }
            }
        }
        if (min_d != -1) {
            tx = bx; ty = by;
            if (player == 1) { p1_tracking = 1; p1_tx = bx; p1_ty = by; }
            else { p2_tracking = 1; p2_tx = bx; p2_ty = by; }
        } else {
            tx = ty = -1;
        }
    }

    if (ty != -1 && ty > y) *n = 1;
    if (tx != -1 && tx > x) *e = 1;
    if (ty != -1 && ty < y) *s = 1;
    if (tx != -1 && tx < x) *w = 1;
}


int main() {
    printf("P1:\n");
    read_cities(1);
    printf("P2:\n");
    read_cities(2);
    calc_grid();
    printf("(%d, %d), %d, %d\n", grid_x, grid_y, grid_width, grid_height);

    int cp = 1;
    while (1) {
        printf("P%d:\n", cp);
        char line[100];
        fgets(line, 100, stdin);

        if (line[0] == 'f') {
            printf("G\nP%d wins\n", (cp == 1) ? 2 : 1);
            break;
        }

        int x, y;
        sscanf(line, "(%d, %d)", &x, &y);

        if (in_own(cp, x, y) || x < grid_x || x >= grid_x + grid_width || y < grid_y || y >= grid_y + grid_height || grid[y - grid_y][x - grid_x] != 0) {
            printf("F\n");
            cp = (cp == 1) ? 2 : 1;
            continue;
        }

        int idx = in_opp(cp, x, y);
        if (idx >= 0) {
            int opp = (cp == 1) ? 2 : 1;
            int *cx = (opp == 1) ? p1_x : p2_x;
            int *cy = (opp == 1) ? p1_y : p2_y;
            int ***occ = (opp == 1) ? p1_occ : p2_occ;

            occ[idx][y - cy[idx]][x - cx[idx]] = 1;
            grid[y - grid_y][x - grid_x] = cp;
            if (cp == 1) p1_tracking = 0; else p2_tracking = 0;

            if (is_mono(cp, idx)) {
                if (all_mono(cp)) {
                    printf("G\nP%d wins\n", cp);
                    break;
                }
                printf("M\n");
            } else printf("H\n");
        } else {
            int n, e, s, w;
            salesperson_report(cp, x, y, &n, &e, &s, &w);
            printf("R (%d, %d, %d, %d)\n", n, e, s, w);
            grid[y - grid_y][x - grid_x] = cp;
            cp = (cp == 1) ? 2 : 1;
        }
    }

    for (int i = 0; i < p1_count; i++) {
        for (int j = 0; j < p1_h[i]; j++) free(p1_occ[i][j]);
        free(p1_occ[i]);
    }
    free(p1_x); free(p1_y); free(p1_w); free(p1_h); free(p1_occ);

    for (int i = 0; i < p2_count; i++) {
        for (int j = 0; j < p2_h[i]; j++) free(p2_occ[i][j]);
        free(p2_occ[i]);
    }
    free(p2_x); free(p2_y); free(p2_w); free(p2_h); free(p2_occ);

    for (int i = 0; i < grid_height; i++) free(grid[i]);
    free(grid);

    return 0;
}
