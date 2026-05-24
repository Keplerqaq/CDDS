#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXSIZE 96
#define YEARS 21

void fish_game(void) {}

void manufacturing_system(void) {}

typedef struct QNode{
    int card;
    struct QNode *next;
} QNode, *PQNode;

typedef struct {
    PQNode front;
    PQNode rear;
} LinkQueue, *PLinkQueue;

typedef struct SNode {
    int card;
    struct SNode *next;
} SNode, *PSNode;

typedef struct {
    PSNode top;
} LinkStack, *PLinkStack;

void queue_init(PLinkQueue q) {
    q->front = q->rear = NULL;
}

int queue_is_empty(PLinkQueue q) {
    return q->front == NULL;
}

void enqueue(PLinkQueue q, int card) {
    PQNode node = (PQNode)malloc(sizeof(QNode));
    node->card = card;
    node->next = NULL;  //不置NULL就是野指针
    if(queue_is_empty(q)) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
}

int dequeue(PLinkQueue q) {
    PQNode tmp = q->front;
    int card = tmp->card;
    q->front = q->front->next;
    if(q->front == NULL) {
        q->rear = NULL;
    }
    free(tmp);
    return card;
}

int queue_length(PLinkQueue q) {
    int len = 0;
    PQNode p = q->front;
    while(p) {
        len++;
        p = p->next;
    }
    return len;
}

void queue_print(PLinkQueue q) {
    PQNode p = q->front;
    if(!p) {
        printf("(空)");
        return;
    }
    while(p) {
        printf ("%d ", p->card);
        p = p->next;
        if(p) {
            printf(", ");
        }
    }
}

void queue_destroy(PLinkQueue q){
    while(!queue_is_empty(q)) {
        dequeue(q);
    }
}

void stack_init(PLinkStack s) {
    s->top = NULL;
}

int stack_is_empty(PLinkStack s) {
    return s->top == NULL;
}

void push(PLinkStack s, int card) {
    PSNode node = (PSNode)malloc(sizeof(SNode));
    node->card = card;
    node->next = s->top;
    s->top = node;
}

int pop(PLinkStack s) {
    PSNode tmp = s->top;
    int card = tmp->card;
    s->top = s->top->next;
    free(tmp);
    return card;
}

void stack_print(PLinkStack s) {
    if(s->top == NULL) {
        printf("(空)");
        return;
    }
    PSNode p = s->top;
    while(p) {
        printf("%d ", p->card);
        p = p->next;
        if(p){
            printf(", ");
        }
    }
}

void stack_destroy(PLinkStack s) {
    while(!stack_is_empty(s)){
        pop(s);    
    }
}

void shuffle(int deck[], int n) {
    for(int i = n -1; i > 0; i--) {
        int j = rand() % (i + 1);   // 0 ~ i
        int tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

void deal_cards(int deck[], int deck_size, PLinkQueue player_a, PLinkQueue player_b) {
    int half = deck_size / 2;
    for(int i = 0; i < half; i++) {
        enqueue(player_a, deck[i]);
    }
    for(int i = half; i < deck_size; i++) {
        enqueue(player_b, deck[i]);
    }
}

int play_turn(PLinkQueue player, PLinkStack table, int flag[], char who[]) {
    int card = dequeue(player);
    printf("%s 出[%d]，", who, card);

    if(flag[card] == 1) {
        printf("桌上已有 %d， 收牌", card);
        enqueue(player, card);
        while(1) {
            int top = pop(table);
            flag[top] = 0;
            enqueue(player, top);
            if(top == card) {
                break;
            }
        }
    } else {
        printf("无匹配，留在桌面");
        push(table, card);
        flag[card] = 1;
    }

    printf("\n");
    return queue_is_empty(player);  //每出一次牌都要判空
}

int main(void) {
    while(1) {
        printf("=====数据结构课程设计======\n");
        printf("1.小猫钓鱼游戏\n");
        printf("2.制造业增加值统计分析系统\n");
        printf("0.退出\n");

        int choice;
        printf("请选择：");
        scanf("%d", &choice);
        switch (choice) {
            case 0:
                printf("再见！\n");
                return 0;
            case 1:
                fish_game();
                break;
            case 2:
                manufacturing_system();
                break;
            default:
                printf("无效选择，请重新输入！\n");
        }
    }
}