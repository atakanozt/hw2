#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <stdio.h>
#include "hw2_output.h"

using namespace std;

void take_inputs(int& row, int& column, int**& matrix)
{
    cin >> row >> column;
    matrix = new int*[row];

    for (size_t i = 0; i < row; i++)
    {
        matrix[i] = new int[column];
        for (size_t j = 0; j < column; j++)
        {
            cin >> matrix[i][j];
        }
    }
}

void free_inputs(int row, int column, int**& matrix)
{
    for (int i = 0; i < row; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int row1, column1;
int **matrix1;

int row2, column2;
int **matrix2;

int row3, column3;
int **matrix3;

int row4, column4;
int **matrix4;

int **result_matrix1;
int **result_matrix2;
int **result;

int *column_count;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t *mutex_array;
sem_t *semaphores;
int *is_semaphore_sent;

void print_matrix(int **matrix, int row, int column)
{
    for (size_t i = 0; i < row; i++)
    {
        for (size_t j = 0; j < column; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    
}


void *first_addition(void *index_p);
void *second_addition(void *index_p);
void *apply_mult(void *arg);

int main(){

    // DON'T FORGET TO CALL hw2_init_output(void);
    hw2_init_output();

    // GET INPUTS.
    take_inputs(row1, column1, matrix1);
    take_inputs(row2, column2, matrix2);
    take_inputs(row3, column3, matrix3);
    take_inputs(row4, column4, matrix4);

    // allocating memory for the first result matrix. A + B = result_matrix1
    result_matrix1 = new int *[row1];
    for (int i = 0; i < row1; i++)
    {
        result_matrix1[i] = new int[column1];
    }

    // allocating memory for the second result matrix. C + D = result_matrix2
    result_matrix2 = new int *[row3];
    for (int i = 0; i < row3; i++)
    {
        result_matrix2[i] = new int[column3];
    }

    // allocating memory for the result matrix.  result_matrix1 x result_matrix2 = result
    result = new int *[row1];
    for (int i = 0; i < row1; i++)
    {
        result[i] = new int[column3];
    }

    // allocate memory for columncount with 0s.
    column_count = new int[column4]();

    
    /* creating semaphores  and initializing mutexex*/
    int number_of_semaphores = row1 * column4;

    mutex_array = new pthread_mutex_t[number_of_semaphores];
    semaphores = new sem_t[number_of_semaphores];

    for (size_t i = 0; i < number_of_semaphores; i++)
    {
        sem_init(&semaphores[i], 0, 0); // initialize with semaphores
        mutex_array[i] = PTHREAD_MUTEX_INITIALIZER; // initialize mutexes.
    }

    /* creating flag of semaphores. */
    is_semaphore_sent = new int[number_of_semaphores]();

    

    // creating threads for first addition.
    int *iret = new int[row1];
    pthread_t *threads_first_addition = new pthread_t[row1];

    // creating threads for second addition
    int *iret2 = new int[row3];
    pthread_t *threads_second_addition = new pthread_t[row3];

    // creating threads for result.
    int *iret3 = new int[row1];
    pthread_t *threads_result = new pthread_t[row1];
    

    // CREATING THREADS
    for(int i = 0; i < row1; i++)
    {
        int *arg = new int;
        *arg = i;
        iret[i] = pthread_create(threads_first_addition + i, NULL, first_addition, (void *) (arg));
    }

    for(int i = 0; i < row3; i++)
    {
        int *arg = new int;
        *arg = i;
        iret2[i] = pthread_create(threads_second_addition + i, NULL, second_addition, (void *) (arg));
    }

    for(int i = 0; i < row1; i++)
    {
            int *arg = new int;
            *arg = i;
            iret3[i] = pthread_create(threads_result + i, NULL, apply_mult, (void *) (arg));
    }
    

    // joining threads.
    int max_of_rows = max(row1, row4);
    for (size_t i = 0; i < max_of_rows; i++)
    {
        if(i < row1)
        {
            pthread_join( threads_first_addition[i], NULL);
        }

        if(i < row3)
        {
            pthread_join( threads_second_addition[i], NULL);
        }
        
        if(i < row1)
        {
            pthread_join( threads_result[i], NULL);
        }
    }

    /*
    cout << "Resulted sum matrix1: " << endl;
    print_matrix(result_matrix1, row1, column1);
    cout << "***************************************" << endl;
    cout << "Resulted sum matrix2: " << endl;
    print_matrix(result_matrix2, row3, column3);
    cout << "***************************************" << endl;
    cout << "Result matrix: " << endl;
    print_matrix(result, row1, column4);
    */
    print_matrix(result, row1, column4);


    free_inputs(row1, column1, matrix1);
    free_inputs(row2, column2, matrix2);
    free_inputs(row3, column3, matrix3);
    free_inputs(row4, column4, matrix4);
    
    free_inputs(row1, column1, result_matrix1);
    free_inputs(row3, column3, result_matrix2);
    free_inputs(row1, column3, result);
    delete[] iret;
    delete[] iret2;
    delete[] iret3;
    delete[] threads_first_addition;
    delete[] threads_second_addition;
    delete[] threads_result;

    delete[] column_count;

    for (size_t i = 0; i < number_of_semaphores; i++)
    {   
        sem_destroy( semaphores + i);
        pthread_mutex_destroy( mutex_array + i);
    }
    delete[] semaphores;
    delete[] mutex_array;
    delete[] is_semaphore_sent;

    return 0;
}

void send_signal(int row) //sending signal function of row elements.
{
    int sem_index;

    for (size_t i = 0; i < column4; i++)
    {
        

        sem_index = (row * column4) + i;

        pthread_mutex_lock( &mutex_array[sem_index] );
        if(++is_semaphore_sent[sem_index] == 2)
        {
            sem_post( semaphores + sem_index);
        }
        pthread_mutex_unlock( &mutex_array[sem_index] );
        
        //cout << "send_signal sem_index is: " << sem_index << " for row: " << row << endl; 
        
    }
}

void send_signal2(int row, int column)
{
    int sem_index;

    for (size_t i = 0; i < row1; i++)
    {
        sem_index = (i * column4) + column;

        pthread_mutex_lock( &mutex_array[sem_index] );
        if(++is_semaphore_sent[sem_index] == 2)
        {
            sem_post( semaphores + sem_index);
        }
        pthread_mutex_unlock( &mutex_array[sem_index] );

        //cout << "send_signal2 sem_index is: " << sem_index << " for column: " << column <<  endl; 
    }

}

void *first_addition(void *index_p)
{
    int index = *((int *) index_p);

    for (size_t i = 0; i < column1; i++)
    {
        
        result_matrix1[index][i] = matrix1[index][i] + matrix2[index][i];
        
        hw2_write_output(0, index + 1, i + 1, result_matrix1[index][i]);

        if(i == column1 - 1) // now the row calculation is ended. should send signal to the multp.
        {
            send_signal(index);
        }

    }
    delete (int *)index_p;
    pthread_exit( (void *) NULL);
    return NULL;
}

void *second_addition(void *index_p)
{
    int index = *((int *) index_p);
    bool flag = false;
    for (size_t i = 0; i < column3; i++)
    {
        result_matrix2[index][i] = matrix3[index][i] + matrix4[index][i];
        hw2_write_output(1, index + 1, i + 1, result_matrix2[index][i]);

        pthread_mutex_lock( &mutex ); // lock mutex.

        column_count[i]++;
        if(column_count[i] == row4)
        {
            flag = true;
        }
        pthread_mutex_unlock( &mutex );

        if(flag)
        {
            send_signal2(index, i); // send signal to all multp elements.
            flag = false;
        }
    }

    delete (int *)index_p;
    pthread_exit( (void *) NULL);
    return NULL;
}


void *apply_mult(void *arg)
{
    int row = *((int *)arg);
    int sem_index;
    int acc;

    for (size_t i = 0; i < column4; i++)
    {
        sem_index = (row * column4) + i;
        acc = 0;

        sem_wait( semaphores + sem_index);
        /* calculate matrix multp for one element.*/
        for (size_t k = 0; k < column1; k++)
        {
            acc += result_matrix1[row][k] * result_matrix2[k][i];
        }


        // update acc
        result[row][i] = acc;
        pthread_mutex_lock( &mutex);
        //cout << "I'm calculating: " << acc << " for (" << row << "," << i << ") and I waited sem_index: " << sem_index << endl;
        pthread_mutex_unlock( &mutex); 
        hw2_write_output(2, row +1, i + 1, result[row][i]);
    }

    delete (int *)arg;
    pthread_exit((void *)NULL);
    return NULL;
}