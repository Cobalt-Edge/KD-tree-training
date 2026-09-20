#include <cmath>
#include <cstdio>
#include <cstdlib>
const int MAXN = 1e5 + 5;
const int DIMENSION = 3;
const long long MMM = 1e18 + 5;
const float alpha = 0.7;
template <typename ANYTYPE>
inline void SWAP(ANYTYPE *a, ANYTYPE *b) {
    ANYTYPE temp = *b;
    *b = *a;
    *a = temp;
}
typedef struct Point {
    long long *position;
    long long value;
} point;
inline long long DISTANCE(point point1, point point2, int dimension) {
    long long distance = 0;
    for (int i = 0; i < dimension; i ++) {
        distance += (point1.position[i] - point2.position[i]) * (point1.position[i] - point2.position[i]);
    }
    return sqrt(distance);
}
inline long long ABS(long long x) {
    return x >= 0 ? x : -x;
}
inline long long MIN(long long a, long long b) {
    return a > b ? b : a;
}
inline long long MAX(long long a, long long b) {
    return a > b ? a : b;
}
inline bool IS_IN_INTERVAL(long long *interval_point1, long long *interval_point2, long long *check_point1, long long *check_point2, int dimension) {
    bool whether_in_interval = true;
    for (int i = 0; i < dimension; i ++) {
        if (!(MIN(interval_point1[i], interval_point2[i]) <= MIN(check_point1[i], check_point2[i])
            && MAX(interval_point1[i], interval_point2[i]) >= MAX(check_point1[i], check_point2[i]))) {
            whether_in_interval = false;
        }
    }
    return whether_in_interval;
}
typedef struct Node {
    point point_information;
    int this_dimension;
    long long lazy_tag;
    int size;
    long long total_value;
    struct Node *left_son;
    struct Node *right_son;
    struct Node *father;
} node;
inline int get_size_of_a_tree(node *root) {
    if (root == NULL) {
        return 0;
    }
    return root->size;
}
inline long long get_total_value_of_a_tree(node *root) {
    if (root == NULL) {
        return 0;
    }
    return root->total_value;
}
node *initialization(point new_point, int dimension, node *father) {
    node *new_node = (node *)calloc(1, sizeof(node));
    if (new_node == NULL) {
        printf("No memory");
        exit(1);
    }
    new_node->point_information = new_point;
    new_node->this_dimension = dimension;
    new_node->lazy_tag = 0;
    new_node->total_value = new_point.value;
    new_node->size = 1;
    new_node->left_son = NULL;
    new_node->right_son = NULL;
    new_node->father = father;
    return new_node;
}
int partition(point *points, int l, int r, int division, int dimension) {
    SWAP(points + r, points + division);
    long long pivot = points[r].position[dimension];
    int i = l;
    for (int j = l; j < r; j ++) {
        if (points[j].position[dimension] <= pivot) {
            SWAP(points + j, points + i);
            i ++;
        }
    }
    SWAP(points + i, points + r);
    return i;
}
// Find the k-th point in specific dimension
void quickSelect(point *points, int l, int r, int k, int dimension) {
    if (l == r) {
        return;
    }
    int part = partition(points, l, r, (l + r) / 2, dimension);
    if (part == k) {
        return;
    } else if (part > k) {
        return quickSelect(points, l, part - 1, k, dimension);
    } else {
        return quickSelect(points, part + 1, r, k, dimension);
    }
}
node *build_KD_tree(point *points, int l, int r, int dimension, int present_dimension, node *father) {
    if (l > r) {
        return NULL;
    }
    if (l == r) {
        return initialization(points[l], present_dimension, father);
    }
    quickSelect(points, l, r, (l + r) / 2 , present_dimension);
    node* new_node = initialization(points[(l + r) / 2], present_dimension, father);
    new_node->left_son = build_KD_tree(points, l, ((l + r) / 2) - 1, dimension, (present_dimension + 1) % dimension, new_node);
    new_node->right_son = build_KD_tree(points, ((l + r) / 2) + 1 , r, dimension, (present_dimension + 1) % dimension, new_node);
    new_node->size = get_size_of_a_tree(new_node->left_son) + get_size_of_a_tree(new_node->right_son) + 1;
    new_node->total_value = get_total_value_of_a_tree(new_node->left_son) + get_total_value_of_a_tree(new_node->right_son) + new_node->point_information.value;
    return new_node;
}
void deleteTree(node **present) {
    if (present == NULL || *present == NULL) {
        return;
    }
    if ((*present)->left_son != NULL) {
        deleteTree(&(*present)->left_son);
        (*present)->left_son = NULL;
    }
    if ((*present)->right_son != NULL) {
        deleteTree(&(*present)->right_son);
        (*present)->right_son = NULL;
    }
    free(*present);
}
void dfs(node *present, int depth, int dimension) {
    if (present == NULL) {
        return;
    }
    printf("Position: ");
    for (int i = 0; i < dimension; i ++) {
        printf("%lld ", present->point_information.position[i]);
    }
    printf("value: %lld", present->point_information.value);
    printf(" depth: %d this_dimension: %d\n", depth, present->this_dimension);
    if (present->left_son != NULL) {
        dfs(present->left_son, depth + 1, dimension);
    }
    if (present->right_son != NULL) {
        dfs(present->right_son, depth + 1, dimension);
    }
}
int find_nearest_point(node *present, point point_to_be_found, int dimension) {
    long long present_distance = DISTANCE(present->point_information, point_to_be_found, dimension);
    if (present->left_son == NULL && present->right_son == NULL) {
        return present_distance;
    }
    if (point_to_be_found.position[present->this_dimension] <= present->point_information.position[present->this_dimension]) {
        if (present->left_son == NULL) {
            int shortest_distance_candidate2 = find_nearest_point(present->right_son, point_to_be_found, dimension);
            return MIN(shortest_distance_candidate2, present_distance);
        }
        int present_shortest_distance = find_nearest_point(present->left_son, point_to_be_found, dimension);
        int shortest_distance_candidate1 = MIN(present_shortest_distance, present_distance);
        if (shortest_distance_candidate1 > ABS(point_to_be_found.position[present->this_dimension] - present->point_information.position[present->this_dimension]) && present->right_son != NULL) {
            int shortest_distance_candidate2 = find_nearest_point(present->right_son, point_to_be_found, dimension);
            return MIN(shortest_distance_candidate1, shortest_distance_candidate2);
        } else {
            return shortest_distance_candidate1;
        }
    } else {
        if (present->right_son == NULL) {
            int shortest_distance_candidate2 = find_nearest_point(present->left_son, point_to_be_found, dimension);
            return MIN(shortest_distance_candidate2, present_distance);
        }
        int present_shortest_distance = find_nearest_point(present->right_son, point_to_be_found, dimension);
        int shortest_distance_candidate1 = MIN(present_shortest_distance, present_distance);
        
        if (shortest_distance_candidate1 > ABS(point_to_be_found.position[present->this_dimension] - present->point_information.position[present->this_dimension]) && present->left_son != NULL) {
            int shortest_distance_candidate2 = find_nearest_point(present->left_son, point_to_be_found, dimension);
            return MIN(shortest_distance_candidate1, shortest_distance_candidate2);
        } else {
            return shortest_distance_candidate1;
        }
    }
}
void push_down(node *present) {
    present->point_information.value += present->lazy_tag;
    if (present->left_son != NULL) {
        present->left_son->lazy_tag += present->lazy_tag;
        present->left_son->total_value += present->lazy_tag * present->left_son->size;
    }
    if (present->right_son != NULL) {
        present->right_son->lazy_tag += present->lazy_tag;
        present->right_son->total_value += present->lazy_tag * present->right_son->size;
    }
    present->lazy_tag = 0;
}
bool is_not_balance(node *present) {
    return ((float)(get_size_of_a_tree(present->left_son)) > (float)(present->size) * alpha || (float)(get_size_of_a_tree(present->right_son)) > (float)(present->size) * alpha);
}
int get_all_points(point* points_needed, node *present, int clk) {
    if (present->lazy_tag != 0) {
        push_down(present);
    }
    if (present->left_son == NULL && present->right_son == NULL) {
        points_needed[clk] = present->point_information;
        return clk + 1;
    }
    int new_clk = clk;
    if (present->left_son != NULL) {
        new_clk = get_all_points(points_needed, present->left_son, clk);        
    }
    points_needed[new_clk] = present->point_information;
    new_clk ++;
    if (present->right_son != NULL) {
        new_clk = get_all_points(points_needed, present->right_son, new_clk);
    }
    return new_clk;
}
node *rebuild_tree(node *root, int dimension) {
    point *points_needed = new point[root->size];
    int final_size = get_all_points(points_needed, root, 0);
    node *new_root = build_KD_tree(points_needed, 0, final_size - 1, dimension, root->this_dimension, root->father);
    deleteTree(&root);
    delete[] points_needed;
    return new_root;
}
bool add_new_point(point new_point, node *present, int dimension, int present_dimension, node **root, node *father) {
    if (present == NULL || root == NULL || *root == NULL) {
        *root = initialization(new_point, 0, father);
        return true;
    }
    bool no_rebuild_beforehand = true;
    if (present->lazy_tag != 0) {
        push_down(present);
    }
    if (new_point.position[present->this_dimension] <= present->point_information.position[present->this_dimension]) {
        if (present->left_son == NULL) {
            node *new_node = initialization(new_point, (present->this_dimension + 1) % dimension, present);
            present->left_son = new_node;
        } else {
            no_rebuild_beforehand = (no_rebuild_beforehand && add_new_point(new_point, present->left_son, dimension, (present_dimension + 1) % dimension, root, present));
        }
    } else {
        if (present->right_son == NULL) {
            node *new_node = initialization(new_point, (present->this_dimension + 1) % dimension, present);
            present->right_son = new_node;
        } else {
            no_rebuild_beforehand = (no_rebuild_beforehand && add_new_point(new_point, present->right_son, dimension, (present_dimension + 1) % dimension, root, present));
        }
    }
    present->size = get_size_of_a_tree(present->left_son) + get_size_of_a_tree(present->right_son) + 1;
    present->total_value = get_total_value_of_a_tree(present->left_son) + get_total_value_of_a_tree(present->right_son) + present->point_information.value;
    if (is_not_balance(present) && no_rebuild_beforehand) {
        no_rebuild_beforehand = false;
        if (present->father == NULL) {
            *root = rebuild_tree(present, dimension);
        } else {
            if (present->father->left_son == present) {
                present->father->left_son = rebuild_tree(present, dimension);
            } else {
                present->father->right_son = rebuild_tree(present, dimension);
            }
        }
    }
    return no_rebuild_beforehand;
}


void interval_operation(long long *interval_point1, long long *interval_point2, int dimension, node *present, long long *present_interval1, long long *present_interval2, long long value_to_be_added) {
    if (present == NULL) {
        return;
    }
    if (IS_IN_INTERVAL(interval_point1, interval_point2, present_interval1, present_interval2, dimension)) {
        present->lazy_tag += value_to_be_added;
        present->total_value += present->size * value_to_be_added;
        return;
    }
    if (present->lazy_tag != 0) {
        push_down(present);
    }
    if (IS_IN_INTERVAL(interval_point1, interval_point2, present->point_information.position, present->point_information.position, dimension)) {
        present->point_information.value += value_to_be_added;
    }
    long long left_interval = MIN(interval_point1[present->this_dimension], interval_point2[present->this_dimension]);
    long long right_interval = MAX(interval_point1[present->this_dimension], interval_point2[present->this_dimension]);
    long long present_left_end = present_interval1[present->this_dimension];
    long long present_right_end = present_interval2[present->this_dimension];
    long long new_division = present->point_information.position[present->this_dimension];
    if (left_interval <= new_division && present->left_son != NULL) {
        present_interval1[present->this_dimension] = present_left_end;
        present_interval2[present->this_dimension] = new_division;
        interval_operation(interval_point1, interval_point2, dimension, present->left_son, present_interval1, present_interval2, value_to_be_added);
    }
    if (right_interval >= new_division && present->right_son != NULL) {
        present_interval1[present->this_dimension] = new_division;
        present_interval2[present->this_dimension] = present_right_end;
        interval_operation(interval_point1, interval_point2, dimension, present->right_son, present_interval1, present_interval2, value_to_be_added);
    }
    present->total_value = get_total_value_of_a_tree(present->left_son) + get_total_value_of_a_tree(present->right_son) + present->point_information.value;
    present_interval1[present->this_dimension] = present_left_end;
    present_interval2[present->this_dimension] = present_right_end;
}

long long interval_query(long long *interval_point1, long long *interval_point2, int dimension, node *present, long long *present_interval1, long long *present_interval2) {
    if (present == NULL) {
        return 0;
    }
    if (IS_IN_INTERVAL(interval_point1, interval_point2, present_interval1, present_interval2, dimension)) {
        return present->total_value;
    }
    if (present->lazy_tag != 0) {
        push_down(present);
    }
    long long answer = 0;
    if (IS_IN_INTERVAL(interval_point1, interval_point2, present->point_information.position, present->point_information.position, dimension)) {
        answer += present->point_information.value;
    }
    long long left_interval = MIN(interval_point1[present->this_dimension], interval_point2[present->this_dimension]);
    long long right_interval = MAX(interval_point1[present->this_dimension], interval_point2[present->this_dimension]);
    long long present_left_end = present_interval1[present->this_dimension];
    long long present_right_end = present_interval2[present->this_dimension];
    long long new_division = present->point_information.position[present->this_dimension];
    if (left_interval <= new_division && present->left_son != NULL) {
        present_interval1[present->this_dimension] = present_left_end;
        present_interval2[present->this_dimension] = new_division;
        answer += interval_query(interval_point1, interval_point2, dimension, present->left_son, present_interval1, present_interval2);
    }
    if (right_interval >= new_division && present->right_son != NULL) {
        present_interval1[present->this_dimension] = new_division;
        present_interval2[present->this_dimension] = present_right_end;
        answer += interval_query(interval_point1, interval_point2, dimension, present->right_son, present_interval1, present_interval2);
    }
    present_interval1[present->this_dimension] = present_left_end;
    present_interval2[present->this_dimension] = present_right_end;
    return answer;
}

int main(void) {
    unsigned int d = 0;
    int q = 0;
    scanf("%u %d", &d, &q);
    point *all_points = new point[MAXN];
    long long *input = new long long[MAXN * d];
    int clk = 0;
    long long lst = 0;
    node *root = NULL;
    for (int i = 0; i < q; i ++) {
        int opt = 0;
        scanf("%d", &opt);
        switch (opt) {
            case 1: {
                for (int j = 0; j < d; j ++) {
                    scanf("%lld", &input[clk * d + j]);
                    input[clk * d + j] ^= lst;
                }
                all_points[clk].position = (input + clk * d);
                long long new_value = 0;
                scanf("%lld", &new_value);
                new_value ^= lst;
                all_points[clk].value = new_value;
                add_new_point(all_points[clk], root, d, 0, &root, NULL);
                clk ++;
                break;
            }
            case 2: {
                long long *interval1 = new long long[d];
                if (interval1 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                long long *interval2 = new long long[d];
                if (interval2 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                long long *present_interval1 = new long long[d];
                if (present_interval1 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                long long *present_interval2 = new long long[d];
                if (present_interval2 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                long long value_to_be_added = 0;
                for (int j = 0; j < d; j ++) {
                    scanf("%lld", &interval1[j]);
                    interval1[j] ^= lst;
                    present_interval1[j] = -MMM;
                }
                for (int j = 0; j < d; j ++) {
                    scanf("%lld", &interval2[j]);
                    interval2[j] ^= lst;
                    present_interval2[j] = MMM;
                }
                scanf("%lld", &value_to_be_added);
                value_to_be_added ^= lst;
                interval_operation(interval1, interval2, d, root, present_interval1, present_interval2, value_to_be_added);
                delete[] interval1;
                delete[] interval2;
                delete[] present_interval1;
                delete[] present_interval2;  
                break;
            }
            case 3: {
                long long *interval1 = new long long[d];
                if (interval1 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                long long *interval2 = new long long[d];
                if (interval2 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                long long *present_interval1 = new long long[d];
                if (present_interval1 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                long long *present_interval2 = new long long[d];
                if (present_interval2 == NULL) {
                    printf("No Memory!\n");
                    exit(1);
                }
                for (int j = 0; j < d; j ++) {
                    scanf("%lld", &interval1[j]);
                    interval1[j] ^= lst;
                    present_interval1[j] = -MMM;
                }
                for (int j = 0; j < d; j ++) {
                    scanf("%lld", &interval2[j]);
                    interval2[j] ^= lst;
                    present_interval2[j] = MMM;
                }
                long long answer = interval_query(interval1, interval2, d, root, present_interval1, present_interval2);
                lst = answer;
                printf("%lld\n", answer);
                delete[] interval1;
                delete[] interval2;
                delete[] present_interval1;
                delete[] present_interval2;
                break;
            }
            default: {
            }
        }
    }
    // dfs(root, 0, d);
    deleteTree(&root);
    delete[] all_points;
    delete[] input;
    return 0;
}