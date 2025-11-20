#ifndef SIM_HEADER_H
#define SIM_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int *x, *y, *w, *h;
    int ***occ;
    int count;
} PlayerData;

typedef struct {
    PlayerData p1, p2;
    int grid_x, grid_y, grid_width, grid_height;
    int **grid;
    int p1_tracking, p2_tracking;
    int p1_tx, p1_ty, p2_tx, p2_ty;
} GameState;

typedef struct Candidate Candidate;
struct Candidate {
    int x, y, cc, pqi;
};

typedef struct SearchPQ SearchPQ;
struct SearchPQ {
    Candidate **storage;
    int size, capacity;
};

typedef struct MonopolizationQueueNode MonopolizationQueueNode;
struct MonopolizationQueueNode {
    int x, y;
    char dir;
    MonopolizationQueueNode *next;
};

typedef struct MonopolizationQueue MonopolizationQueue;
struct MonopolizationQueue {
    MonopolizationQueueNode *head;
    MonopolizationQueueNode *tail;
};

void init_game(GameState *game);
void read_cities(GameState *game, int player);
void calc_grid(GameState *game);
int in_own(GameState *game, int p, int x, int y);
int in_opp(GameState *game, int p, int x, int y);
int is_mono(GameState *game, int p, int idx);
int all_mono(GameState *game, int p);
void salesperson_report(GameState *game, int player, int x, int y, int *n, int *e, int *s, int *w);
void cleanup_game(GameState *game);

int parent(int i);
int left(int i);
int right(int i);

Candidate *create_candidate(int x, int y, int cc);
void destroy_candidate(Candidate *cand);
int is_prioritized(Candidate *cand1, Candidate *cand2);

SearchPQ *create_searchPQ(void);
void destroy_searchPQ(SearchPQ *pq);
void pq_heapify(SearchPQ *pq, int i);
void pq_insert(SearchPQ *pq, Candidate *cand);
void pq_update(SearchPQ *pq, int i, int new_cc);
void pq_remove(SearchPQ *pq, int i);
void pq_extract_max(SearchPQ *pq, int *x, int *y);

int deq(MonopolizationQueue *monq, int *x, int *y, char *dir);
void enq_neighbours(MonopolizationQueue *monq, int x, int y);

#endif
