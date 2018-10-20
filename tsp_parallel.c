#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

const int no_points = 50;
const int max = 1000;
int points[no_points][2];

//struct Compare { int val; int index; }; //for selection sort
//#pragma omp declare reduction(minimum : struct Compare : omp_out = omp_in.val > omp_out.val ? omp_in : omp_out) //for selection sort

int square(int num)
{
    return num * num;
}

//To calculate the total distance travelled
double fitness(int gene[no_points])
{
    double distance;
    for (int i=0;i<(no_points-1);i++)
    {
        distance += sqrt((square(points[gene[i]][0]-points[gene[i+1]][1]))+(square(points[gene[i]][1]-points[gene[i+1]][1])));
    }
    return distance;
}

//To swap arrays
void swap(int gene1[no_points],int gene2[no_points])
{
    int temp[no_points];
    for (int i=0;i<no_points;i++)
    {
        temp[i] = gene1[i];
        gene1[i] = gene2[i];
        gene2[i] = temp[i];
    }
}

//To copy arrays
void copy(int gene1[no_points],int gene2[no_points])
{
    for (int i=0;i<no_points;i++)
    {
        gene1[i] = gene2[i];
    }
}

void print(int gene[no_points])
{
    for(int i=0;i<no_points;i++)
    {
        printf("%d ",gene[i]);
    }
    printf("\n");
}

void merge(int no_genes, double fit[no_genes], int genes[no_genes][no_points], int low, int mid, int high)
{
    int i,j,k;
    int n1 = mid - low + 1; 
    int n2 =  high - mid;
    int L[n1], R[n2], G1[n1][no_points], G2[n2][no_points];
    for (i = 0; i < n1; i++) 
    {
        L[i] = fit[low + i]; 
        copy(G1[i],genes[low + i]);
    }
    for (i = 0; i < n2; i++) 
    {
        R[i] = fit[mid + 1+ i]; 
        copy(G2[i],genes[mid+1+i]);
    }
    i=0;
    j=0;
    k=low;
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            fit[k] = L[i];
            copy(genes[k],G1[i]);
            i++; 
        } 
        else
        { 
            fit[k] = R[j]; 
            copy(genes[k],G2[j]);
            j++; 
        } 
        k++; 
    } 

    while (i < n1) 
    {
        fit[k] = L[i]; 
        copy(genes[k],G1[i]);
        i++; 
        k++; 
    }

    while (j < n2) 
    { 
        fit[k] = R[j]; 
        copy(genes[k],G2[j]);
        j++; 
        k++; 
    } 
}

void mergeSort(int no_genes, double fit[no_genes], int genes[no_genes][no_points], int low, int high)
{
    if (low<high)
    {
        int mid = low+(high-low)/2;
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                mergeSort(no_genes, fit, genes, low, mid);
            }
            #pragma omp section
            {
                mergeSort(no_genes, fit, genes, mid + 1, high);
            }
        } 
        merge(no_genes, fit, genes, low, mid, high);
    }
    else   
        return;
}

int main()
{
    double start_time = omp_get_wtime();

    #pragma omp parallel for
    for (int i=0;i<no_points;i++)
    {
        points[i][0] = rand()%max;
        points[i][1] = rand()%max;
    }
    int no_genes = 10000;
    int genes[no_genes][no_points];

    #pragma omp parallel for
    for (int i=0;i<no_genes;i++)
    {
        int occupied[no_points];
        for (int i=0;i<no_points;i++)
            occupied[i] = 0;
        for (int num=1;num<=no_points;num++)
        {
            int pos = rand()%no_points;
            while (occupied[pos]!=0)
                pos = (pos + 1)%no_points;
            genes[i][pos] = num;
            occupied[pos]=1;
        }
    }
    
    //Calculating fitness
    double fit[no_genes];
    #pragma omp parallel for
    for (int i=0;i<no_genes;i++)
    {
        fit[i] = fitness(genes[i]);
    }

    /*Selection sort
    for (int i=0;i<no_genes-1;i++)
    {
        struct Compare min;
        min.val = fit[i];
        min.index = i;
        //#pragma omp parallel for reduction(minimum:min)
        for (int j=i+1;j<no_genes;j++)
        {
            if (fit[j] < min.val)
                min.val = fit[j];
                min.index = j;
        }
        if (min.index != i)
        {
            int temp = fit[min.index];
            fit[min.index] = fit[i];
            fit[i] = temp;
            swap(genes[min.index],genes[i]);
        }
    }*/

    mergeSort(no_genes, fit, genes, 0, no_genes-1);

    int no_iterations = 500;
    for (int count=0;count<no_iterations;count++)
    {
        //Crossing over of genes to produce new genes
        int half_genes = no_genes/2;
        #pragma omp parallel for
        for (int i=0;i<half_genes;i+=2)
        {
            copy(genes[i+half_genes],genes[i]);
            copy(genes[i+1+half_genes],genes[i+1]);
            for (int j=0;j<(no_points/2);j++)
            {
                int x = genes[i+half_genes][j];
                int index = 0;
                while (genes[i+1][index]!=x)
                {
                    index++;
                }
                int temp = genes[i+1][j];
                genes[i+1][j] = genes[i+1][index];
                genes[i+1][index] = temp;
            }
            for (int j=(no_points/2);j<no_points;j++)
            {
                int x = genes[i+1+half_genes][j];
                int index = 0;
                while (genes[i][index]!=x)
                {
                    index++;
                }
                int temp = genes[i][j];
                genes[i][j] = genes[i][index];
                genes[i][index] = temp;
            }
        }

        #pragma omp parallel for
        for (int i=0;i<no_genes;i++)
        {
            fit[i] = fitness(genes[i]);
        }
        
        mergeSort(no_genes, fit, genes, 0, no_genes-1);
    }

    printf("Gene with maximum fitness: \n");
    print(genes[0]);
    printf("Fitness: %f\n",fit[0]);

    double total_time = omp_get_wtime() - start_time;
    printf("The total time taken is: %f\n",total_time);

}