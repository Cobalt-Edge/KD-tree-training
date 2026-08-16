#include <cmath>
#include <cstdio>
#include <cstdlib>
const int MAXN = 1e5 + 5;
const int DIMENSION = 3;
template <typename ANYTYPE>
inline void SWAP(ANYTYPE *a, ANYTYPE *b) {
    ANYTYPE temp = *b;
    *b = *a;
    *a = temp;
}
inline double DISTANCE(double *position1, double *position2, int dimension) {
    double distance = 0;
    for (int i = 0; i < dimension; i ++) {
        distance += (position1[i] - position2[i]) * (position1[i] - position2[i]);
    }
    return sqrt(distance);
}
inline double ABS(double x) {
    return x >= 0 ? x : -x;
}
inline double MIN(double a, double b) {
    return a > b ? b : a;
}
typedef struct Node {
    double *position;
    int this_dimension;
    struct Node *left_son;
    struct Node *right_son;
} node;
node *initialization(double *new_position, int dimension) {
    node *new_node = (node *)calloc(1, sizeof(node));
    if (new_node == NULL) {
        printf("No memory");
        exit(1);
    }
    new_node->position = new_position;
    new_node->this_dimension = dimension;
    new_node->left_son = NULL;
    new_node->right_son = NULL;
    return new_node;
}
int partition(double **array, int l, int r, double division, int dimension) {
    int inil = l;
    int inir = r;
    while (l < r - 1) {
        while (l < r - 1 && array[r][dimension] > division) {
            r --;
        }
        while (l < r - 1 && array[l][dimension] <= division) {
            l ++;
        }
        if (array[l][dimension] > division && array[r][dimension] <= division) {
            SWAP(array + l, array + r);
        }
    }
    if (l < r && array[r][dimension] > division) {
        r --;
    }
    if (l < r && array[l][dimension] <= division) {
        l ++;
    }
    int target = 1;
    for (int i = inil; i <= inir; i ++) {
        if (array[i][dimension] == division) {
            target = i;
            break;
        }
    }
    SWAP(array + target, array + (l + r) / 2);
    return (l + r) / 2;
}
// Find the k-th point in specific dimension
void quickSelect(double **array, int l, int r, int k, int dimension) {
    if (l == r) {
        return;
    }
    int part = partition(array, l, r, array[(l + r) / 2][dimension], dimension);
    if (part == k) {
        return;
    } else if (part > k) {
        return quickSelect(array, l, part - 1, k, dimension);
    } else {
        return quickSelect(array, part + 1, r, k, dimension);
    }
}
node *build_KD_tree(double **array, int l, int r, int dimension, int present_dimension) {
    if (l > r) {
        return NULL;
    }
    if (l == r) {
        return initialization(*(array + l), present_dimension);
    }
    quickSelect(array, l, r, (l + r) / 2 , present_dimension);
    node* new_node = initialization(array[(l + r) / 2], present_dimension);
    new_node->left_son = build_KD_tree(array, l, ((l + r) / 2) - 1, dimension, (present_dimension + 1) % dimension);
    new_node->right_son = build_KD_tree(array, ((l + r) / 2) + 1 , r, dimension, (present_dimension + 1) % dimension);
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
        printf("%.2f ", present->position[i]);
    }
    printf(" depth: %d this_dimension: %d\n", depth, present->this_dimension);
    if (present->left_son != NULL) {
        dfs(present->left_son, depth + 1, dimension);
    }
    if (present->right_son != NULL) {
        dfs(present->right_son, depth + 1, dimension);
    }
}
double find_nearest_point(node *present, double *point, int dimension) {
    double present_distance = DISTANCE(present->position, point, dimension);
    if (present->left_son == NULL && present->right_son == NULL) {
        return present_distance;
    }
    if (point[present->this_dimension] <= present->position[present->this_dimension]) {
        if (present->left_son == NULL) {
            double shortest_distance_candidate2 = find_nearest_point(present->right_son, point, dimension);
            return MIN(shortest_distance_candidate2, present_distance);
        }
        double present_shortest_distance = find_nearest_point(present->left_son, point, dimension);
        double shortest_distance_candidate1 = MIN(present_shortest_distance, present_distance);
        if (shortest_distance_candidate1 > ABS(point[present->this_dimension] - present->position[present->this_dimension]) && present->right_son != NULL) {
            double shortest_distance_candidate2 = find_nearest_point(present->right_son, point, dimension);
            return MIN(shortest_distance_candidate1, shortest_distance_candidate2);
        } else {
            return shortest_distance_candidate1;
        }
    } else {
        if (present->right_son == NULL) {
            double shortest_distance_candidate2 = find_nearest_point(present->left_son, point, dimension);
            return MIN(shortest_distance_candidate2, present_distance);
        }
        double present_shortest_distance = find_nearest_point(present->right_son, point, dimension);
        double shortest_distance_candidate1 = MIN(present_shortest_distance, present_distance);
        
        if (shortest_distance_candidate1 > ABS(point[present->this_dimension] - present->position[present->this_dimension]) && present->left_son != NULL) {
            double shortest_distance_candidate2 = find_nearest_point(present->left_son, point, dimension);
            return MIN(shortest_distance_candidate1, shortest_distance_candidate2);
        } else {
            return shortest_distance_candidate1;
        }
    }
}
void add_new_point(double *a_new_point, node *present, int dimension, int present_dimension, node **root) {
    if (present == NULL || root == NULL || *root == NULL) {
        *root = initialization(a_new_point, 0);
        return;
    }
    if (a_new_point[present_dimension] <= present->position[present_dimension]) {
        if (present->left_son == NULL) {
            node *new_node = initialization(a_new_point, (present_dimension + 1) % dimension);
            present->left_son = new_node;
        } else {
            add_new_point(a_new_point, present->left_son, dimension, (present_dimension + 1) % dimension, root);
        }
    } else {
        if (present->right_son == NULL) {
            node *new_node = initialization(a_new_point, (present_dimension + 1) % dimension);
            present->right_son = new_node;
        } else {
            add_new_point(a_new_point, present->right_son, dimension, (present_dimension + 1) % dimension, root);
        }
    }
}

int main(void) {
    int n = 0;
    int d = 2;
    scanf("%d", &n);
    double **array = new double*[n];
    double *input = new double[n * d];
    for (int i = 0; i < n * d ; i ++) {
        scanf("%lf", &input[i]);
    }
    for (int i = 0; i < n; i ++) {
        *(array + i) = (input + i * d);
    }
    node *root = NULL;
    double answer = INFINITY;
    for (int i = 0; i < n - 1 ; i ++) {
        add_new_point(*(array + i), root, d, 0, &root);
        double present_answer = find_nearest_point(root, *(array + i + 1), d);
        answer = MIN(answer, present_answer);
    }
    printf("%.4f", answer);
    deleteTree(&root);
    delete[] array;
    delete[] input;
    return 0;
}