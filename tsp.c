#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

const int no_points = 20;
const int max = 1000;
int points[no_points][2];

//To calculate square of a number
int square(int num)
{
    return num * num;
}

//To calculate the total distance travelled
double fitness(int gene[no_points])
{
    double distance;
    #pragma omp parallel for reduction(+:distance)
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

int main()
{
    time_t t;
    srand((unsigned) time(&t));
    //Generating random points
    for (int i=0;i<no_points;i++)
    {
        points[i][0] = rand()%max;
        points[i][1] = rand()%max;
        //printf("%d %d\n",points[i][0],points[i][1]);
    }

    //Generating the initial genes
    int no_genes = 20;
    int genes[no_genes][no_points];
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
        /*for (int j=0;j<no_points;j++)
            printf("%d ",genes[i][j]);
        printf("\n");*/
    }
    
    //Calculating fitness
    double fit[no_genes];
    #pragma omp parallel for
    for (int i=0;i<no_genes;i++)
    {
        fit[i] = fitness(genes[i]);
    }
    //Sorting the genes according to fitness using selection sort
    for (int i=0;i<no_genes-1;i++)
    {
        int position = i;
        for (int j=i+1;j<no_genes;j++)
        {
            if (fit[position] > fit[j])
                position = j;
        }
        if (position != i)
        {
            int temp = fit[position];
            fit[position] = fit[i];
            fit[i] = temp;
            swap(genes[position],genes[i]);
        }
    }

    int no_iterations = 100;
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
        for (int i=0;i<no_genes-1;i++)
        {
            int position = i;
            for (int j=i+1;j<no_genes;j++)
            {
                if (fit[position] > fit[j])
                    position = j;
            }
            if (position != i)
            {
                int temp = fit[position];
                fit[position] = fit[i];
                fit[i] = temp;
                swap(genes[position],genes[i]);
            }
        }
        /*printf("\nGenes after sorting: \n");
        for (int i=0;i<no_genes;i++)
        {
            print(genes[i]);
            printf("%f\n",fit[i]);
        }
        printf("\n");*/
    }

    printf("Gene with maximum fitness: \n");
    print(genes[0]);
    printf("Fitness: %f\n",fit[0]);
}
