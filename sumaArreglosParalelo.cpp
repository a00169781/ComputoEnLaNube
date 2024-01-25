#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <unordered_set>

// Para compilar este archivo hay que instalar GCC y usar el siguiente comando:
// ./g++ sumaArreglosParalelo.cpp -o sumaArreglosParalelo -fopenmp

// Definimos algunas constantes que usaremos
// para controlar el tamaño de las algunas tareas
// que realizaremos en paralelo
#define N 10000
#define chunk 100
#define mostrar 10

// prototypo de la función que usaremos para imprimir los arreglos
// la función la implementamos despues de main
void imprimeArreglo(float *d);


// declaramos que vamos a usar funciones declaradas en esta librería
using namespace std;

int main() 
{
    cout << "Sumando Arreglos en Paralelo!\n";
    // declaramos las variable que usaremos en nuestro programa
    // estos arreglos se usarán para poblarlos de valores y luego sumarlos
    // usando ejecución en paralalo.
    float a[N], b[N], c[N];
    // estas variables son auxiliares
    // la variable i la reusamos como índice de nuestros bucles
    int i;
    // las variables tid las usaré para guardar los ids númericos de los hilos
    // de ejecusión, meramente con fines de ilustrar el funcionamiento del código
    int tidA, tidB, tids[N];


    // El primer paso es poblar los arreglos que se van a sumar entre si
    // Para esto esoy usando el generador de números pseudo aleatorios de de c++
    // aunque el poblar los arreglos tiene un costo computacional bajo, como prueba
    // de concepto estoy paralelizando su ejecución con la directiva de sections de omp
    // esta directiva permite explicitamente ejecutar bloque de código con diferentes threads
    // como prueba de que se ejecuta de manera paralela, estoy guadando los ids de los hilos,
    // para imprimirlos posteriormente
    #pragma omp parallel sections \
    private(i)
    {
        #pragma omp section
        {
            tidA = omp_get_thread_num();
            for (i = 0; i < N; i++) 
            {
                a[i] = rand() % 5;
            }
        }
        #pragma omp section
        {
            tidB = omp_get_thread_num();
            for (i = 0; i < N; i++) 
            {
                b[i] = rand() % 5;
            }
        }
    }

    // imprimimos los ids de los hilos que poblaron cada arreglo
    cout << "El arreglo 'a' esta siendo poblado por el thread " << tidA << endl;
    cout << "El arreglo 'b' esta siendo poblado por el thread " << tidB << endl;

    // Ahora vamos a paralelizar la suma de los arreglos
    // para esto usamos la directiva omp for
    // esto especifica que el bucle for será ejecutado por un grupo de hilos
    // la directiva también nos permite especificar el uso de las variables, si se van a usar
    // de manera compartida como es el caso de los arreglos o si se usan de manera privada
    // como es el caso de los índices del bucle
    // Por pura curiosidad estoy guardando los ids de los hilos de ejecusión para 
    // contar cuantos hilos se crearon para resolver esta tarea, los hilos los guardo en el arreglo tids
    // la manera en que lo estoy haciendo es super ineficiente, pero solo lo hago por un tema didáctivo. 
    #pragma omp parallel for \
    shared(a, b, c, tids) private(i)\
    schedule(static, chunk)
    for (i = 0; i < N; i++)
    {
        tids[i] = omp_get_thread_num();;
        c[i] = a[i] + b[i];
    }
    
    // Este bucle lo uso para unicamente contar los diferentes ids de los hilos
    unordered_set<int> set;
    for (int i = 0; i < sizeof(tids)/ sizeof(tids[0]); i++) {
        set.insert(tids[i]);
    }

    cout << "Se sumaron los arreglos usando " << set.size() << " hilos" << endl;
    cout << "Imprimiendo los primeros " << mostrar << " valores del arreglo a: ";
    imprimeArreglo(a);
    cout << "Imprimiendo los primeros " << mostrar << " valores del arreglo b: ";
    imprimeArreglo(b);
    cout << "Imprimiendo los primeros " << mostrar << " valores del arreglo c: ";
    imprimeArreglo(c);
}

// Esta es la función que declaré al principio, esta función es la
// que imprime los primeros elementos del arreglo en cuestión
void imprimeArreglo(float *d)
{
    for (int x = 0; x < mostrar; x++) 
    {
        cout << d[x] << "-";
    }
    cout << endl;
}