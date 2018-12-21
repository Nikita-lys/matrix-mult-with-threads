//  I43_1.c

/*
 Лысенко Никита, 3.8
 
 Задание I43
 
 1. Заданы две матрицы вещественных чисел размеров NxN. Выполнить операцию умножения двух матриц
 в k потоках. Каждый поток должен обрабатывать заданный набор строк первой матрицы и часть второй.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//специальная структура для данных потока
typedef struct arguments
{
    int kol;    // количесто столбцов для подсчёта одного потока
    int n;      // размер
    int to;     /// номер обрабатываемого столбца
    int **arr1;
    int **arr2;
    int **res;
} arguments;

// Создание рандомной матрицы
int** create_random_matrix(int n)
{
    int** m = (int**)malloc(n*sizeof(int*));
    for (int i = 0; i < n; ++i)
    {
        m[i] = (int*)malloc(n*sizeof(int));
        for (int j = 0; j < n; ++j)
        {
            m[i][j] = rand() % 5;
        }
    }
    return m;
}

// Печать матрицы
void print_matrix(int** m, int n)
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
            printf("%d ", m[i][j]);
        printf("\n");
    }
    printf("\n");
}

// Функция перемножения матриц
int** mult_matrix(int** m1, int** m2, int n)
{
    int** res = (int**)malloc(n * sizeof(int*));
    
    for(int i = 0; i < n; ++i)
    {
        res[i] = (int*)malloc(n * sizeof(int));
        for(int j = 0; j < n; ++j)
        {
            int q = 0;
            for (int k = 0; k < n; ++k)
            {
                q = q + m1[i][k] * m2[k][j];
            }
            res[i][j] = q;
        }
    }
    return res;
}

// Функция перемножения матриц с помощью потоков
void* mult_matrix_by_using_threads(void* thread_data)
{
    arguments *data = (arguments*) thread_data;
    printf("%d", data->kol);
    printf(" количество");
    printf("\n");
    
    for (int j = 0; j < data->kol; ++j)
    {
        printf("%d", data->to + 1);
        printf(" столбец\n");
        for(int i = 0; i < data->n; i++)
        {
            for(int j = data->to; j < data->to + 1; j++)
            {
                int q = 0;
                for(int k = 0; k < data->n; k++)
                {
                    q = q + data->arr1[i][k] * data->arr2[k][j];
                }
                data->res[i][j] = q;
            }
        }
        data->to = data->to + 1;
    }
    return NULL;
}

int main(int argc, const char * argv[])
{
    int n;
    printf("Введите N: ");
    scanf("%d", &n);
    int k;
    printf("Введите количество потоков: ");
    scanf("%d", &k);
    int** m1 = create_random_matrix(n);
    int** m2 = create_random_matrix(n);
    print_matrix(m1, n);
    print_matrix(m2, n);
    
    // Выделение памяти под res матрицу
    int** res = (int**) malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i)
    {
        res[i] = (int*) malloc(n * sizeof(int));
    }
    
    //выделяем память под массив идентификаторов потоков
    pthread_t* threads = (pthread_t*) malloc(n * sizeof(pthread_t));
    //сколько потоков - столько и структур с потоковых данных
    arguments* threadData = (arguments*) malloc(n * sizeof(arguments));
    
    int d = 0;
    int f = 0;
    if (k > n)
    {
        k = n;
    }
    if (k < n)
    {
        d = n / k; // целое
        f = n % k; // остаток
    }
    if (k == n)
        d = 1;
    
    int l = 0;
    while (l < n)
    {
        for(int i = 0; i < k; i++)
        {
            if (f > 0)
            {
                threadData[i].kol = d + 1;
                --f;
            }
            else
            {
                threadData[i].kol = d;
            }
            threadData[i].n = n;
            threadData[i].arr1 = m1;
            threadData[i].arr2 = m2;
            threadData[i].res = res;
            threadData[i].to = l;
            
            printf("%d", i + 1);
            printf(" поток \n");
            // pthread_create(&(threads[i]), NULL, mult_matrix_by_using_threads, &threadData[i]);
            l = l + threadData[i].kol;
        }
    }
    
    
    
    // Засечение времени умножения квадратных матриц с помощью потоков
    struct timespec mt1, mt2;
    clock_gettime (CLOCK_REALTIME, &mt1);
    time_t t = clock();
    
    for (int i = 0; i < k; ++i)
        pthread_create(&(threads[i]), NULL, mult_matrix_by_using_threads, &threadData[i]);
    //ожидаем выполнение всех потоков
    for(int i = 0; i < k; i++)
        pthread_join(threads[i], NULL);

    printf("\n");
    printf("%f\n", (double)(clock() - t)/CLOCKS_PER_SEC);
    clock_gettime (CLOCK_REALTIME, &mt2);
    //printf("%f\n", (mt2.tv_sec - mt1.tv_sec)+0.000000001*(mt2.tv_nsec - mt1.tv_nsec));
    printf("\n");
    print_matrix(res, n);
    
    
    // Засечение времени умножения квадратных матриц
    clock_gettime (CLOCK_REALTIME, &mt1);
    t = clock();
    
    res = mult_matrix(m1, m2, n);
    
    printf("\n");
    printf("%f\n", (double)(clock() - t)/CLOCKS_PER_SEC);
    clock_gettime (CLOCK_REALTIME, &mt2);
    //printf("%f\n", (mt2.tv_sec - mt1.tv_sec)+0.000000001*(mt2.tv_nsec - mt1.tv_nsec));
    printf("\n");
    print_matrix(res, n);
    
    
    // Очищение памяти
    free(threads);
    free(threadData);
    for (int i = 0; i < n; i++)
    {
        free(m1[i]);
        free(m2[i]);
        free(res[i]);
    }
    free(m1);
    free(m2);
    free(res);

    return 0;
}
