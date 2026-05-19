#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct QNode {
    int card;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front;
    QNode *rear;
} LinkQueue;

void queue_init(LinkQueue *q) {
    q->front = q->rear = NULL;
}

int queue_is_empty(LinkQueue *q) {
    return q->front == NULL;
}

void enqueue(LinkQueue *q, int card) {
    QNode *node = (QNode *)malloc(sizeof(QNode));
    node->card = card;
    node->next = NULL;
    if (queue_is_empty(q)) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
}

int dequeue(LinkQueue *q) {
    QNode *tmp = q->front;
    int card = tmp->card;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(tmp);
    return card;
}

int queue_length(LinkQueue *q) {
    int len = 0;
    QNode *p = q->front;
    while (p) { len++; p = p->next; }
    return len;
}

void queue_print(LinkQueue *q) {
    QNode *p = q->front;
    if (!p) { printf("（空）"); return; }
    while (p) {
        printf("%d", p->card);
        p = p->next;
        if (p) printf(", ");
    }
}

void queue_destroy(LinkQueue *q) {
    while (!queue_is_empty(q)) dequeue(q);
}

typedef struct SNode {
    int card;
    struct SNode *next;
} SNode;

void push(SNode **stack, int card) {
    SNode *node = (SNode *)malloc(sizeof(SNode));
    node->card = card;
    node->next = *stack;
    *stack = node;
}

int pop(SNode **stack) {
    SNode *tmp = *stack;
    int card = tmp->card;
    *stack = (*stack)->next;
    free(tmp);
    return card;
}

int stack_is_empty(SNode *stack) {
    return stack == NULL;
}

void stack_print(SNode *stack) {
    if (!stack) { printf("（空）"); return; }
    SNode *p = stack;
    while (p) {
        printf("%d", p->card);
        p = p->next;
        if (p) printf(", ");
    }
}

void stack_destroy(SNode **stack) {
    while (!stack_is_empty(*stack)) pop(stack);
}

void shuffle(int deck[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

void deal_cards(int deck[], int deck_size,
                LinkQueue *player_a, LinkQueue *player_b) {
    int half = deck_size / 2;
    for (int i = 0; i < half; i++)
        enqueue(player_a, deck[i]);
    for (int i = half; i < deck_size; i++)
        enqueue(player_b, deck[i]);
}

int play_turn(LinkQueue *player, SNode **table,
              int *flag, const char *who) {
    int card = dequeue(player);
    printf("  %s出 [%d] → ", who, card);

    if (flag[card] == 1) {
        printf("桌上已有 %d，收牌！", card);
        enqueue(player, card);
        int collected = 1;
        while (1) {
            int top = pop(table);
            flag[top] = 0;
            enqueue(player, top);
            collected++;
            if (top == card) break;
        }
        printf(" 共收 %d 张", collected);
    } else {
        printf("无匹配，留在桌面");
        push(table, card);
        flag[card] = 1;
    }

    printf("  |  手牌[%d]: ", queue_length(player));
    queue_print(player);
    printf("\n");
    return queue_is_empty(player);
}

void fish_game(void) {
    srand((unsigned int)time(NULL));

    printf("========================================\n");
    printf("         小猫钓鱼游戏\n");
    printf("========================================\n\n");

    int deck[36], deck_size = 36, idx = 0;
    for (int v = 1; v <= 9; v++)
        for (int i = 0; i < 4; i++)
            deck[idx++] = v;

    shuffle(deck, deck_size);

    LinkQueue player_a, player_b;
    queue_init(&player_a);
    queue_init(&player_b);
    deal_cards(deck, deck_size, &player_a, &player_b);

    SNode *table = NULL;
    int table_flag[10] = {0};

    printf("初始手牌：\n");
    printf("  甲 (%d张): ", queue_length(&player_a));
    queue_print(&player_a);
    printf("\n  乙 (%d张): ", queue_length(&player_b));
    queue_print(&player_b);
    printf("\n\n");

    int round = 0;
    while (1) {
        round++;
        printf("第 %d 轮：", round);
        if (play_turn(&player_a, &table, table_flag, "甲")) {
            printf("\n甲手牌出完，乙获胜！\n"); break;
        }
        printf("       ");
        if (play_turn(&player_b, &table, table_flag, "乙")) {
            printf("\n乙手牌出完，甲获胜！\n"); break;
        }
        printf("\n");
    }

    printf("\n========================================\n");
    printf("              游戏结束\n");
    printf("========================================\n");
    printf("桌面剩余牌（栈顶→栈底）: ");
    stack_print(table);
    printf("\n");
    if (!queue_is_empty(&player_a)) {
        printf("甲方手牌: "); queue_print(&player_a); printf("\n");
    }
    if (!queue_is_empty(&player_b)) {
        printf("乙方手牌: "); queue_print(&player_b); printf("\n");
    }

    queue_destroy(&player_a);
    queue_destroy(&player_b);
    stack_destroy(&table);
}

#define MAXSIZE 96
#define YEARS   21          

typedef struct {
    char  country[30];               
    int   country_type;              
    float value_added[YEARS];        
    float growth_rate[YEARS];        
    int   year[YEARS];               
    int   index_va[YEARS];           
    int   index_gr[YEARS];           
} RecType;

typedef struct {
    RecType r[MAXSIZE];
    int     index_l[MAXSIZE];    
    int     index_ml[MAXSIZE];   
    int     index_mh[MAXSIZE];   
    int     index_h[MAXSIZE];    
    int     length;
    int     count_l, count_ml, count_mh, count_h;  
    int     growth_done;
} SqList;

int partition(SqList *L, int a[], int low, int high, int year) {
    int pivot = a[low];
    while (low < high) {
        while (low < high && L->r[a[high]].value_added[year] <= L->r[pivot].value_added[year])
            high--;
        a[low] = a[high];
        while (low < high && L->r[a[low]].value_added[year] >= L->r[pivot].value_added[year])
            low++;
        a[high] = a[low];
    }
    a[low] = pivot;
    return low;
}

void quick_sort_idx(SqList *L, int a[], int low, int high, int year) {
    if (low < high) {
        int p = partition(L, a, low, high, year);
        quick_sort_idx(L, a, low, p - 1, year);
        quick_sort_idx(L, a, p + 1, high, year);
    }
}

void MVA_SqList_Read(SqList *L, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { printf("打开文件失败！请确认文件路径正确。\n"); return; }

    int n = 0;
    char temp_type[30];

    while (n < MAXSIZE) {
        if (fscanf(fp, "%s %s", L->r[n].country, temp_type) != 2) break;
        for (int i = 0; i < YEARS; i++)
            fscanf(fp, "%f", &L->r[n].value_added[i]);

        if (strcmp(temp_type, "低收入国家") == 0)
            L->r[n].country_type = 0;
        else if (strcmp(temp_type, "中低等收入国家") == 0)
            L->r[n].country_type = 1;
        else if (strcmp(temp_type, "中高等收入国家") == 0)
            L->r[n].country_type = 2;
        else if (strcmp(temp_type, "高收入国家") == 0)
            L->r[n].country_type = 3;

        for (int i = 0; i < YEARS; i++)
            L->r[n].year[i] = 1999 + i;
        n++;
    }

    L->length = n;
    fclose(fp);
    printf("导入成功，共 %d 个国家。\n", L->length);
}

void MVA_SqList_Search(SqList *L) {
    char name[30];
    int  year;

    printf("请输入国家名：");
    scanf("%s", name);
    printf("请输入年份 (1999-2019)：");
    scanf("%d", &year);

    if (year < 1999 || year > 2019) {
        printf("年份超出范围，应为 1999-2019。\n"); return;
    }

    int i;
    for (i = 0; i < L->length; i++)
        if (strcmp(name, L->r[i].country) == 0) break;

    if (i >= L->length) {
        printf("未找到该国。\n"); return;
    }

    int idx = year - 1999;
    printf("%s %d年: 制造业增加值 = %.2f 亿美元",
           name, year, L->r[i].value_added[idx]);
    if (idx > 0)
        printf(", 增速 = %.2f%%", L->r[i].growth_rate[idx] * 100);
    printf("\n");
}

void MVA_SqList_Calculate(SqList *L) {
    for (int i = 0; i < L->length; i++) {
        L->r[i].growth_rate[0] = 0;  
        for (int k = 1; k < YEARS; k++) {
            float prev = L->r[i].value_added[k - 1];
            if (prev == 0)
                L->r[i].growth_rate[k] = 0;
            else
                L->r[i].growth_rate[k] =
                    (L->r[i].value_added[k] - prev) / prev;
        }
    }
    L->growth_done = 1;
    printf("增速计算完成。\n");
}

void MVA_SqList_Sort_Va(SqList *L) {
    int idx_arr[MAXSIZE];   

    for (int year = 0; year < YEARS; year++) {

        for (int i = 0; i < L->length; i++)
            idx_arr[i] = i;

        quick_sort_idx(L, idx_arr, 0, L->length - 1, year);

        for (int rank = 0; rank < L->length; rank++)
            L->r[idx_arr[rank]].index_va[year] = rank + 1;

        printf("\n%d 年增加值排名:\n", 1999 + year);
        printf("%-4s %-20s %12s\n", "名次", "国家", "增加值（亿美元）");
        for (int j = 0; j < L->length; j++) {
            int id = idx_arr[j];
            printf("%-4d %-20s %12.2f\n",
                   j + 1, L->r[id].country, L->r[id].value_added[year]);
        }
    }
}

void group_sort_select(SqList *L, int *group, int group_size,
                       int *result_idx, int year) {
    for (int i = 0; i < group_size; i++)
        result_idx[i] = group[i];

    for (int i = 0; i < group_size - 1; i++) {
        int max = i;
        for (int j = i + 1; j < group_size; j++) {
            float gr_j = L->r[result_idx[j]].growth_rate[year];
            float gr_max = L->r[result_idx[max]].growth_rate[year];
            if (gr_j > gr_max) max = j;
        }
        if (max != i) {
            int tmp = result_idx[i];
            result_idx[i] = result_idx[max];
            result_idx[max] = tmp;
        }
    }
}

void MVA_SqList_Sort_Gr(SqList *L) {
    if (!L->growth_done) { printf("请先执行增速计算！\n"); return; }
    int temp_rank[MAXSIZE];

    L->count_l = L->count_ml = L->count_mh = L->count_h = 0;
    for (int i = 0; i < L->length; i++) {
        switch (L->r[i].country_type) {
            case 0: L->index_l[L->count_l++] = i; break;
            case 1: L->index_ml[L->count_ml++] = i; break;
            case 2: L->index_mh[L->count_mh++] = i; break;
            case 3: L->index_h[L->count_h++] = i; break;
        }
    }

    for (int i = 0; i < L->length; i++)
        for (int year = 0; year < YEARS; year++)
            L->r[i].index_gr[year] = i;

    char *type_name[] = {"低收入", "中低等收入", "中高等收入", "高收入"};
    int  *groups[]     = {L->index_l, L->index_ml, L->index_mh, L->index_h};
    int   sizes[]      = {L->count_l, L->count_ml, L->count_mh, L->count_h};

    for (int year = 0; year < YEARS; year++) {
        for (int g = 0; g < 4; g++) {
            if (sizes[g] == 0) continue;
            group_sort_select(L, groups[g], sizes[g], temp_rank, year);

            for (int rank = 0; rank < sizes[g]; rank++)
                L->r[temp_rank[rank]].index_gr[year] = rank + 1;

            printf("\n%d年 %s国家 增速排名:\n",
                   1999 + year, type_name[g]);
            for (int j = 0; j < sizes[g]; j++) {
                int id = temp_rank[j];
                printf("  %d. %s 增速=%.2f%%\n", j + 1,
                       L->r[id].country,
                       L->r[id].growth_rate[year] * 100);
            }
        }
    }
}

void MVA_SqList_Analyze(SqList *L) {
    if (!L->growth_done) { printf("请先执行增速计算！\n"); return; }
    char name[30];
    printf("请输入要分析的国家名：");
    scanf("%s", name);

    int i;
    for (i = 0; i < L->length; i++)
        if (strcmp(name, L->r[i].country) == 0) break;

    if (i >= L->length) { printf("未找到该国。\n"); return; }

    RecType *rec = &L->r[i];

    float min_va = rec->value_added[0], max_va = rec->value_added[0];
    float sum_va = 0, sum_sq_va = 0;
    float min_gr = rec->growth_rate[0], max_gr = rec->growth_rate[0];
    float sum_gr = 0, sum_sq_gr = 0;

    for (int k = 0; k < YEARS; k++) {
        float v = rec->value_added[k];
        if (v < min_va) min_va = v;
        if (v > max_va) max_va = v;
        sum_va += v;

        float g = rec->growth_rate[k];
        if (g < min_gr) min_gr = g;
        if (g > max_gr) max_gr = g;
        sum_gr += g;
    }

    float avg_va = sum_va / YEARS;
    float avg_gr = sum_gr / YEARS;

    for (int k = 0; k < YEARS; k++) {
        sum_sq_va += (rec->value_added[k] - avg_va) * (rec->value_added[k] - avg_va);
        sum_sq_gr += (rec->growth_rate[k] - avg_gr) * (rec->growth_rate[k] - avg_gr);
    }
    float var_va = sum_sq_va / (YEARS - 1);  
    float var_gr = sum_sq_gr / (YEARS - 1);

    printf("\n%s 1999-2019 年制造业统计分析：\n", name);
    printf("  增加值 — 最小值: %.2f, 最大值: %.2f, 均值: %.2f, 方差: %.2f\n",
           min_va, max_va, avg_va, var_va);
    printf("  增速   — 最小值: %.2f%%, 最大值: %.2f%%, 均值: %.2f%%, 方差: %.6f\n",
           min_gr * 100, max_gr * 100, avg_gr * 100, var_gr);

    if (var_va > 1e6)
        printf("  → 该国制造业增加值波动很大，发展不稳定。\n");
    else if (var_va < 1e3)
        printf("  → 该国制造业增加值波动较小，发展较平稳。\n");
}

void MVA_SqList_Save(SqList *L, const char *src_name) {
    char file_va[300], file_gr[300];
    FILE *fp;

    if (L->r[0].index_va[0] == 0) {
        printf("（自动执行增加值排名...）\n");
        MVA_SqList_Sort_Va(L);
    }
    if (L->r[0].index_gr[0] == 0) {
        printf("（自动执行增速排名...）\n");
        MVA_SqList_Sort_Gr(L);
    }

    int len = (int)strlen(src_name);
    strncpy(file_va, src_name, len - 4); file_va[len - 4] = '\0';
    strncpy(file_gr, src_name, len - 4); file_gr[len - 4] = '\0';
    strcat(file_va, "_Sorted.txt");
    strcat(file_gr, "_Grouped_Sorted.txt");

    fp = fopen(file_va, "w");
    if (!fp) { printf("无法创建文件 %s\n", file_va); return; }
    int rank_idx[MAXSIZE];
    for (int year = 0; year < YEARS; year++) {

        for (int i = 0; i < L->length; i++)
            rank_idx[L->r[i].index_va[year] - 1] = i;

        fprintf(fp, "\n%d 年世界各国制造业增加值排名\n", 1999 + year);
        fprintf(fp, "%-4s %-20s %12s\n", "名次", "国家", "增加值（亿美元）");
        for (int rk = 1; rk <= L->length; rk++) {
            int id = rank_idx[rk - 1];
            fprintf(fp, "%-4d %-20s %12.2f\n",
                    rk, L->r[id].country, L->r[id].value_added[year]);
        }
    }
    fclose(fp);

    fp = fopen(file_gr, "w");
    if (!fp) { printf("无法创建文件 %s\n", file_gr); return; }

    char *type_name[] = {"低收入", "中低等收入", "中高等收入", "高收入"};
    int  *groups[]     = {L->index_l, L->index_ml, L->index_mh, L->index_h};
    int   sizes[]      = {L->count_l, L->count_ml, L->count_mh, L->count_h};

    for (int year = 0; year < YEARS; year++) {
        for (int g = 0; g < 4; g++) {
            if (sizes[g] == 0) continue;

            for (int j = 0; j < sizes[g]; j++) {
                int id = groups[g][j];
                rank_idx[L->r[id].index_gr[year] - 1] = id;
            }

            fprintf(fp, "\n%d 年 %s国家制造业增加值增速排名\n",
                    1999 + year, type_name[g]);
            fprintf(fp, "%-4s %-20s %10s\n", "名次", "国家", "增速");
            for (int rk = 1; rk <= sizes[g]; rk++) {
                int id = rank_idx[rk - 1];
                fprintf(fp, "%-4d %-20s %10.2f%%\n",
                        rk, L->r[id].country,
                        L->r[id].growth_rate[year] * 100);
            }
        }
    }
    fclose(fp);

    printf("排名结果已保存至：\n");
    printf("  增加值排名 → %s\n", file_va);
    printf("  增速排名   → %s\n", file_gr);
}

int MVA_Menu_Show(void) {
    printf("\n========== 世界各国制造业增加值统计分析系统 ==========\n");
    printf("*  1: 原始数据导入        2: 原始数据查询             *\n");
    printf("*  3: 增速计算            4: 增加值排名               *\n");
    printf("*  5: 增速排名            6: 增加值分析               *\n");
    printf("*  7: 统计结果保存        0: 退出                     *\n");
    printf("========================================================\n");
    printf("请输入选择（0-7）：");

    int key;
    while (1) {
        scanf("%d", &key);
        if (key >= 0 && key <= 7) return key;
        printf("输入无效，请重新输入（0-7）：");
    }
}

void manufacturing_system(void) {
    SqList L;
    L.length = 0;
    L.growth_done = 0;
    char filename[300] = "制造业分析_inputdate.txt";
    int  loaded = 0;

    while (1) {
        int key = MVA_Menu_Show();
        switch (key) {
            case 0:
                printf("退出制造业统计分析系统。\n");
                return;
            case 1:
                printf("请输入数据文件名（默认: 制造业分析_inputdate.txt）：");
                getchar();  
                {
                    char tmp[300];
                    fgets(tmp, sizeof(tmp), stdin);
                    tmp[strcspn(tmp, "\n")] = 0;
                    if (strlen(tmp) > 0) strcpy(filename, tmp);
                }
                MVA_SqList_Read(&L, filename);
                loaded = 1;
                break;
            case 2:
                if (!loaded) { printf("请先导入数据！\n"); break; }
                MVA_SqList_Search(&L);
                break;
            case 3:
                if (!loaded) { printf("请先导入数据！\n"); break; }
                MVA_SqList_Calculate(&L);
                break;
            case 4:
                if (!loaded) { printf("请先导入数据！\n"); break; }
                MVA_SqList_Sort_Va(&L);
                break;
            case 5:
                if (!loaded) { printf("请先导入数据！\n"); break; }
                MVA_SqList_Sort_Gr(&L);
                break;
            case 6:
                if (!loaded) { printf("请先导入数据！\n"); break; }
                MVA_SqList_Analyze(&L);
                break;
            case 7:
                if (!loaded) { printf("请先导入数据！\n"); break; }
                MVA_SqList_Save(&L, filename);
                break;
        }
    }
}

int main(void) {
    while (1) {
        printf("\n===== 数据结构课程设计 =====\n");
        printf("1. 小猫钓鱼游戏\n");
        printf("2. 制造业增加值统计分析系统\n");
        printf("0. 退出\n");
        printf("============================\n");
        printf("请选择：");

        int choice;
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
                printf("无效选择，请重新输入。\n");
        }
    }
}
