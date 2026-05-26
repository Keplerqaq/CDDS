#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXSIZE 96
#define YEARS 21

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
    printf("%s出%d，", who, card);

    if(flag[card - 1] == 1) {
        printf("桌上已有 %d，收牌", card);
        enqueue(player, card);
        while(1) {
            int top = pop(table);
            flag[top - 1] = 0;
            enqueue(player, top);
            if(top == card) {
                break;
            }
        }
    } else {
        printf("无匹配，留在桌面");
        push(table, card);
        flag[card - 1] = 1;
    }

    printf("\n");
    return queue_is_empty(player);  //每出一次牌都要判空
}

void fish_game(void) {
    printf("========================================\n");
    printf("            小猫钓鱼游戏\n");
    printf("========================================\n\n");
    srand((unsigned int)time(NULL));
    int deck[36], deck_size = 36, idx = 0;
    for (int v = 1; v <=9; v++) {
        for (int i = 0; i < 4; i++) {
            deck[idx] = v;
            idx++;
        }
    }

    shuffle(deck, deck_size);

    LinkQueue player_a, player_b;
    queue_init(&player_a);
    queue_init(&player_b);

    LinkStack table;
    stack_init(&table);
    int table_flag[9] = {0};

    deal_cards(deck, deck_size, &player_a, &player_b);

    int turn = 0;
    while(1) {
        turn++;
        printf("第%d轮：", turn);
        if(play_turn(&player_a, &table, table_flag, "甲")) {
            printf("乙获胜！\n\n");
            break;
        }
        if(play_turn(&player_b, &table, table_flag, "乙")) {
            printf("甲获胜！\n\n");
            break;
        }
        printf("\n");
    }

    printf("桌面剩余牌：");
    stack_print(&table);
    printf("\n");
    if(!queue_is_empty(&player_a)) {
        printf("甲方手牌：");
        queue_print(&player_a);
        printf("\n\n");
    }
    if(!queue_is_empty(&player_b)) {
        printf("乙方手牌：");
        queue_print(&player_b);
        printf("\n\n");
    }
    queue_destroy(&player_a);
    queue_destroy(&player_b);
    stack_destroy(&table);
}

typedef struct {
    char country[30];
    int country_type;
    float value_added[YEARS];
    float growth_rate[YEARS];
    int index_va[YEARS];
    int index_gr[YEARS];
} RecType;

typedef struct {
    RecType r[MAXSIZE];
    int length;
    int count_l, count_ml, count_mh, count_h;
    int index_l[MAXSIZE], index_ml[MAXSIZE], index_mh[MAXSIZE], index_h[MAXSIZE];
    int growth_done;
} SqList, *PSqList;

void MVA_SqList_Read(PSqList L, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("打开文件失败，请检查文件路径是否正确！\n");
        return;
    }

    int n = 0;
    char temp_type[30];

    while(n < MAXSIZE) {
        if (fscanf(fp, "%s %s", L->r[n].country, temp_type) != 2) {
            break;
        }
        if (strcmp(temp_type, "低收入国家") == 0) {
            L->r[n].country_type = 0;
        } else if (strcmp(temp_type, "中低等收入国家") == 0) {
            L->r[n].country_type = 1;
        } else if (strcmp(temp_type, "中高等收入国家") == 0) {
            L->r[n].country_type = 2;
        } else if (strcmp(temp_type, "高收入国家") == 0) {
            L->r[n].country_type = 3;
        }
        for(int i = 0; i < YEARS; i++) {
            fscanf(fp, "%f", &L->r[n].value_added[i]);
        }
        n++;
    }

    L->length = n;
    fclose(fp);
    printf("导入成功，共%d个国家。\n", L->length);
}

void MVA_SqList_Search(PSqList L) {
    char name[30];
    int year;

    printf("请输入国家名：");
    scanf("%s", name);
    printf("请输入年份:");
    scanf("%d", &year);

    if (year < 1999 || year > 2019) {
        printf("请输入1999 ～ 2019之间的年份！\n");
        return;
    }

    int i;
    for (i = 0; i < L->length; i++) {
        if (strcmp(name, L->r[i].country) == 0) {
            break;
        }
    }

    if (i >= L->length) {
        printf("未找到该国。\n");
        return;
    }

    int idx = year - 1999;  //年份索引
    printf("%s%d年：制造业增加值 = %.2f亿美元\n", name, year, L->r[i].value_added[idx]);
    if (idx > 0) {
        printf("%s%d年：增速 = %.2f%%\n", name, year, L->r[i].growth_rate[idx] * 100);
    }
}

void MVA_SqList_Calculate(PSqList L) {
    for (int i = 0; i < L->length; i++) {
        L->r[i].growth_rate[0] = 0;
        for (int k = 1; k < YEARS; k++) {
            float prev = L->r[i].value_added[k - 1];
            if (prev == 0) {
                L->r[i].growth_rate[k] = 0;
            } else {
                L->r[i].growth_rate[k] = (L->r[i].value_added[k] - prev) / prev;
            }
        }
    }
    L->growth_done = 1;
    printf("增速计算完成！\n");
}

int partition(PSqList L, int a[], int low, int high, int year) {
    int pivot = a[low];
    while (low < high) {
        while (low < high && L->r[a[high]].value_added[year] <= L->r[pivot].value_added[year]) {
            high--;
        }
        a[low] = a[high];
        while (low < high && L->r[a[low]].value_added[year] >= L->r[pivot].value_added[year]) {
            low++;
        }
        a[high] = a[low];
    }
    a[low] = pivot;
    return low;
}

void quick_sort(PSqList L, int a[], int low, int high, int year) {
    if (low < high) {
        int p = partition(L, a, low, high, year);
        quick_sort(L, a, low, p - 1, year);
        quick_sort(L, a, p + 1, high, year);
    }
}

void MVA_SqList_Sort_Va(PSqList L) {
    int idx_arr[MAXSIZE];
    for (int year = 0; year < YEARS; year++) {
        for (int i = 0; i < L->length; i++) {
            idx_arr[i] = i;
        }
        quick_sort(L, idx_arr, 0, L->length - 1, year);
        for (int rank = 0; rank < L->length; rank++) {
            L->r[idx_arr[rank]].index_va[year] = rank + 1;
        }
        printf("%d年增加值排名：\n", 1999 + year);
        printf("%-4s %-30s %-10s\n", "名次", "国家", "增加值");
        for (int j = 0; j < L->length; j++) {
            int id = idx_arr[j];
            printf("%-4d %-30s %-10.2f\n", j + 1, L->r[id].country, L->r[id].value_added[year]);
        }
    }
}

void group_sort_select(PSqList L, int group[], int group_size, int result_idx[], int year) {
    for (int i = 0; i < group_size; i++) {
        result_idx[i] = group[i];
    }

    for (int i = 0; i < group_size - 1; i++) {
        int max = i;
        for (int j = i + 1; j < group_size; j++) {
            if (L->r[result_idx[j]].growth_rate[year] > L->r[result_idx[max]].growth_rate[year]) {
                max = j;
            }
        }

        if (max != i) {
            int temp = result_idx[i];
            result_idx[i] = result_idx[max];
            result_idx[max] = temp;
        }
    }
}

void manufacturing_system(void) {
    SqList L;
    L.length = 0;
    L.growth_done = 0;
    MVA_SqList_Read(&L, "制造业分析_inputdate.txt");
    MVA_SqList_Calculate(&L);
    MVA_SqList_Sort_Va(&L);
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