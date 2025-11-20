#include "sim_header.h"


void init_game(GameState *game) {
    memset(game, 0, sizeof(GameState));
    game->p1.count = 0;
    game->p2.count = 0;
    game->p1_tracking = 0;
    game->p2_tracking = 0;
}

void read_cities(GameState *game, int player) {
    char line[1000];
    int cap = 10, count = 0;
    int *cx = malloc(cap * sizeof(int));
    int *cy = malloc(cap * sizeof(int));
    int *cw = malloc(cap * sizeof(int));
    int *ch = malloc(cap * sizeof(int));
    int ***occ = malloc(cap * sizeof(int **));

    while (1) {
        if (!fgets(line, 1000, stdin)) break;
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
            occ[count][i] = calloc(w, sizeof(int));
        }
        count++;
    }

    if (player == 1) {
        game->p1.x = cx; game->p1.y = cy; game->p1.w = cw; game->p1.h = ch;
        game->p1.occ = occ; game->p1.count = count;
    } else {
        game->p2.x = cx; game->p2.y = cy; game->p2.w = cw; game->p2.h = ch;
        game->p2.occ = occ; game->p2.count = count;
    }
}

void calc_grid(GameState *game) {
    int min_x = game->p1.x[0], min_y = game->p1.y[0];
    int max_x = game->p1.x[0] + game->p1.w[0] - 1;
    int max_y = game->p1.y[0] + game->p1.h[0] - 1;

    for (int i = 0; i < game->p1.count; i++) {
        if (game->p1.x[i] < min_x) min_x = game->p1.x[i];
        if (game->p1.y[i] < min_y) min_y = game->p1.y[i];
        if (game->p1.x[i] + game->p1.w[i] - 1 > max_x) max_x = game->p1.x[i] + game->p1.w[i] - 1;
        if (game->p1.y[i] + game->p1.h[i] - 1 > max_y) max_y = game->p1.y[i] + game->p1.h[i] - 1;
    }
    for (int i = 0; i < game->p2.count; i++) {
        if (game->p2.x[i] < min_x) min_x = game->p2.x[i];
        if (game->p2.y[i] < min_y) min_y = game->p2.y[i];
        if (game->p2.x[i] + game->p2.w[i] - 1 > max_x) max_x = game->p2.x[i] + game->p2.w[i] - 1;
        if (game->p2.y[i] + game->p2.h[i] - 1 > max_y) max_y = game->p2.y[i] + game->p2.h[i] - 1;
    }

    game->grid_x = min_x;
    game->grid_y = min_y;
    game->grid_width = max_x - min_x + 1;
    game->grid_height = max_y - min_y + 1;

    game->grid = malloc(game->grid_height * sizeof(int *));
    for (int i = 0; i < game->grid_height; i++) {
        game->grid[i] = calloc(game->grid_width, sizeof(int));
    }
}

int in_own(GameState *game, int p, int x, int y) {
    PlayerData *pd = (p == 1) ? &game->p1 : &game->p2;
    for (int i = 0; i < pd->count; i++) {
        if (x >= pd->x[i] && x < pd->x[i] + pd->w[i] &&
            y >= pd->y[i] && y < pd->y[i] + pd->h[i]) return 1;
    }
    return 0;
}

int in_opp(GameState *game, int p, int x, int y) {
    int opp = (p == 1) ? 2 : 1;
    PlayerData *pd = (opp == 1) ? &game->p1 : &game->p2;
    for (int i = 0; i < pd->count; i++) {
        if (x >= pd->x[i] && x < pd->x[i] + pd->w[i] &&
            y >= pd->y[i] && y < pd->y[i] + pd->h[i]) return i;
    }
    return -1;
}

int is_mono(GameState *game, int p, int idx) {
    int opp = (p == 1) ? 2 : 1;
    PlayerData *pd = (opp == 1) ? &game->p1 : &game->p2;
    for (int i = 0; i < pd->h[idx]; i++) {
        for (int j = 0; j < pd->w[idx]; j++) {
            if (pd->occ[idx][i][j] == 0) return 0;
        }
    }
    return 1;
}

int all_mono(GameState *game, int p) {
    int opp = (p == 1) ? 2 : 1;
    PlayerData *pd = (opp == 1) ? &game->p1 : &game->p2;
    for (int i = 0; i < pd->count; i++) {
        if (!is_mono(game, p, i)) return 0;
    }
    return 1;
}

void salesperson_report(GameState *game, int player, int x, int y, int *n, int *e, int *s, int *w) {
    *n = *e = *s = *w = 0;
    int tx = -1, ty = -1;

    if ((player == 1 && game->p1_tracking) || (player == 2 && game->p2_tracking)) {
        tx = (player == 1) ? game->p1_tx : game->p2_tx;
        ty = (player == 1) ? game->p1_ty : game->p2_ty;
    } else {
        int opp = (player == 1) ? 2 : 1;
        PlayerData *pd = (opp == 1) ? &game->p1 : &game->p2;
        int min_d = -1, bx = -1, by = -1, bprio = -1, bhoff = -1;

        for (int i = 0; i < pd->count; i++) {
            for (int dy = 0; dy < pd->h[i]; dy++) {
                for (int dx = 0; dx < pd->w[i]; dx++) {
                    if (pd->occ[i][dy][dx] == 0) {
                        int dx_pos = pd->x[i] + dx;
                        int dy_pos = pd->y[i] + dy;
                        int d = abs(dx_pos - x) + abs(dy_pos - y);
                        int pr = 0;
                        if (dy_pos > y && dx_pos == x) pr = 8;
                        else if (dx_pos > x && dy_pos == y) pr = 7;
                        else if (dy_pos < y && dx_pos == x) pr = 6;
                        else if (dx_pos < x && dy_pos == y) pr = 5;
                        else if (dy_pos < y && dx_pos < x) pr = 4;
                        else if (dy_pos < y && dx_pos > x) pr = 3;
                        else if (dy_pos > y && dx_pos < x) pr = 2;
                        else if (dy_pos > y && dx_pos > x) pr = 1;
                        int hoff = abs(dx_pos - x);
                        if (min_d == -1 || d < min_d || (d == min_d && pr > bprio) ||
                            (d == min_d && pr == bprio && hoff < bhoff)) {
                            min_d = d; bx = dx_pos; by = dy_pos; bprio = pr; bhoff = hoff;
                        }
                    }
                }
            }
        }
        if (min_d != -1) {
            tx = bx; ty = by;
            if (player == 1) { game->p1_tracking = 1; game->p1_tx = bx; game->p1_ty = by; }
            else { game->p2_tracking = 1; game->p2_tx = bx; game->p2_ty = by; }
        }
    }

    if (ty != -1 && ty > y) *n = 1;
    if (tx != -1 && tx > x) *e = 1;
    if (ty != -1 && ty < y) *s = 1;
    if (tx != -1 && tx < x) *w = 1;
}

void cleanup_game(GameState *game) {
    for (int i = 0; i < game->p1.count; i++) {
        for (int j = 0; j < game->p1.h[i]; j++) free(game->p1.occ[i][j]);
        free(game->p1.occ[i]);
    }
    free(game->p1.x); free(game->p1.y); free(game->p1.w); free(game->p1.h); free(game->p1.occ);

    for (int i = 0; i < game->p2.count; i++) {
        for (int j = 0; j < game->p2.h[i]; j++) free(game->p2.occ[i][j]);
        free(game->p2.occ[i]);
    }
    free(game->p2.x); free(game->p2.y); free(game->p2.w); free(game->p2.h); free(game->p2.occ);

    for (int i = 0; i < game->grid_height; i++) free(game->grid[i]);
    free(game->grid);
}

int parent(int i) { return (i - 1) / 2; }
int left(int i) { return 2 * i + 1; }
int right(int i) { return 2 * i + 2; }

Candidate *create_candidate(int x, int y, int cc) {
    Candidate *cand = malloc(sizeof(Candidate));
    cand->x = x;
    cand->y = y;
    cand->cc = cc;
    cand->pqi = -1;
    return cand;
}

void destroy_candidate(Candidate *cand) {
    free(cand);
}

int is_prioritized(Candidate *cand1, Candidate *cand2) {
    if (cand1->cc != cand2->cc) return cand1->cc > cand2->cc;
    if (cand1->y != cand2->y) return cand1->y < cand2->y;
    return cand1->x < cand2->x;
}

SearchPQ *create_searchPQ(void) {
    SearchPQ *pq = malloc(sizeof(SearchPQ));
    pq->size = 0;
    pq->capacity = 10;
    pq->storage = malloc(pq->capacity * sizeof(Candidate *));
    return pq;
}

void destroy_searchPQ(SearchPQ *pq) {
    for (int i = 0; i < pq->size; i++) {
        destroy_candidate(pq->storage[i]);
    }
    free(pq->storage);
    free(pq);
}

void pq_heapify(SearchPQ *pq, int i) {
    if (i < 0 || i >= pq->size) return;
    
    while (i > 0) {
        int p = parent(i);
        if (is_prioritized(pq->storage[i], pq->storage[p])) {
            Candidate *temp = pq->storage[i];
            pq->storage[i] = pq->storage[p];
            pq->storage[p] = temp;
            pq->storage[i]->pqi = i;
            pq->storage[p]->pqi = p;
            i = p;
        } else break;
    }
    
    while (1) {
        int l = left(i);
        int r = right(i);
        int largest = i;
        
        if (l < pq->size && is_prioritized(pq->storage[l], pq->storage[largest])) {
            largest = l;
        }
        if (r < pq->size && is_prioritized(pq->storage[r], pq->storage[largest])) {
            largest = r;
        }
        
        if (largest != i) {
            Candidate *temp = pq->storage[i];
            pq->storage[i] = pq->storage[largest];
            pq->storage[largest] = temp;
            pq->storage[i]->pqi = i;
            pq->storage[largest]->pqi = largest;
            i = largest;
        } else break;
    }
}

void pq_insert(SearchPQ *pq, Candidate *cand) {
    if (pq->size >= pq->capacity) {
        pq->capacity *= 2;
        pq->storage = realloc(pq->storage, pq->capacity * sizeof(Candidate *));
    }
    
    pq->storage[pq->size] = cand;
    cand->pqi = pq->size;
    pq->size++;
    pq_heapify(pq, cand->pqi);
}

void pq_update(SearchPQ *pq, int i, int new_cc) {
    if (i < 0 || i >= pq->size) return;
    pq->storage[i]->cc = new_cc;
    pq_heapify(pq, i);
}

void pq_remove(SearchPQ *pq, int i) {
    if (i < 0 || i >= pq->size) return;
    
    Candidate *to_remove = pq->storage[i];
    pq->size--;
    
    if (i < pq->size) {
        pq->storage[i] = pq->storage[pq->size];
        pq->storage[i]->pqi = i;
        pq_heapify(pq, i);
    }
    
    destroy_candidate(to_remove);
}

void pq_extract_max(SearchPQ *pq, int *x, int *y) {
    if (pq->size > 0) {
        *x = pq->storage[0]->x;
        *y = pq->storage[0]->y;
        pq_remove(pq, 0);
    }
}

int deq(MonopolizationQueue *monq, int *x, int *y, char *dir) {
    if (monq->head == NULL) return 0;
    
    MonopolizationQueueNode *node = monq->head;
    *x = node->x;
    *y = node->y;
    *dir = node->dir;
    
    monq->head = node->next;
    if (monq->head == NULL) monq->tail = NULL;
    
    free(node);
    return 1;
}

void enq_neighbours(MonopolizationQueue *monq, int x, int y) {
    char dirs[4] = {'N', 'E', 'S', 'W'};
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    
    for (int i = 0; i < 4; i++) {
        MonopolizationQueueNode *node = malloc(sizeof(MonopolizationQueueNode));
        node->x = x + dx[i];
        node->y = y + dy[i];
        node->dir = dirs[i];
        node->next = NULL;
        
        if (monq->head == NULL) {
            monq->head = node;
            monq->tail = node;
        } else {
            monq->tail->next = node;
            monq->tail = node;
        }
    }
}

int main(void) {
    GameState game;
    init_game(&game);

    printf("P1:\n");
    read_cities(&game, 1);
    printf("P2:\n");
    read_cities(&game, 2);
    calc_grid(&game);
    printf("(%d, %d), %d, %d\n", game.grid_x, game.grid_y, game.grid_width, game.grid_height);

    int cp = 1;
    while (1) {
        printf("P%d:\n", cp);
        char line[100];
        if (!fgets(line, 100, stdin)) break;

        if (line[0] == 'f') {
            printf("G\nP%d wins\n", (cp == 1) ? 2 : 1);
            break;
        }

        int x, y;
        sscanf(line, "(%d, %d)", &x, &y);

        if (in_own(&game, cp, x, y) || x < game.grid_x || x >= game.grid_x + game.grid_width ||
            y < game.grid_y || y >= game.grid_y + game.grid_height ||
            game.grid[y - game.grid_y][x - game.grid_x] != 0) {
            printf("F\n");
            cp = (cp == 1) ? 2 : 1;
            continue;
        }

        int idx = in_opp(&game, cp, x, y);
        if (idx >= 0) {
            int opp = (cp == 1) ? 2 : 1;
            PlayerData *pd = (opp == 1) ? &game.p1 : &game.p2;
            pd->occ[idx][y - pd->y[idx]][x - pd->x[idx]] = 1;
            game.grid[y - game.grid_y][x - game.grid_x] = cp;
            if (cp == 1) game.p1_tracking = 0; else game.p2_tracking = 0;

            if (is_mono(&game, cp, idx)) {
                if (all_mono(&game, cp)) {
                    printf("G\nP%d wins\n", cp);
                    break;
                }
                printf("M\n");
            } else printf("H\n");
        } else {
            int n, e, s, w;
            salesperson_report(&game, cp, x, y, &n, &e, &s, &w);
            printf("R (%d, %d, %d, %d)\n", n, e, s, w);
            game.grid[y - game.grid_y][x - game.grid_x] = cp;
            cp = (cp == 1) ? 2 : 1;
        }
    }

    cleanup_game(&game);
    return 0;
}
