import time
import numpy as np
import random 
import math

no_points = 5000
max_area = 1000
points = np.zeros((no_points,2),dtype=np.int)

def gen_random(i):
    points[i][0] = random.randint(0,max_area)
    points[i][1] = random.randint(0,max_area)

def gen_genes(i):
    occupied = np.zeros((no_points),dtype=np.int)
    occupied[0] = 1
    for num in range(2,no_points+1):
        x = random.randint(0,no_points-1)
        while (occupied[x]!=0):
            x = (x+1)%no_points
        genes[i][x] = num
        occupied[x] = 1

def fitness(i):
    distance = 0
    for j in range(no_points-1):
        d = math.sqrt((points[genes[i][j]-1][0]-points[genes[i][j+1]-1][0])**2 + (points[genes[i][j]-1][1]-points[genes[i][j+1]-1][1])**2)
        distance += d
    fit[i] = distance

start_time = time.time()
for i in range(no_points):
    gen_random(i)
no_genes = 60
genes = np.ones((no_genes,no_points),dtype=np.int)
for i in range(no_genes):
    gen_genes(i)
fit = np.zeros((no_genes),dtype=np.int)
for i in range(no_genes):
    fitness(i)

genes = genes[fit.argsort()]
fit = sorted(fit)

no_iterations = 5
for iterations in range(no_iterations):
    half_genes = no_genes/2
    for i in range(0,half_genes,2):
        for j in range(no_points):
            genes[i+half_genes][j] = genes[i][j]
            genes[i+half_genes+1][j] = genes[i+1][j]
        for j in range(0,no_points/2):
            x = genes[i+half_genes][j]
            index = 0
            while(genes[i+1][index]!=x):
                index+=1
            genes[i+1][index] = genes[i+1][j]
            genes[i+1][j] = x
        for j in range(no_points/2,no_points):
            x = genes[i+half_genes+1][j]
            index = 0
            while(genes[i][index]!=x):
                index+=1
            genes[i][index] = genes[i][j]
            genes[i][j] = x
    
    fit = np.zeros((no_genes),dtype=np.int)
    for i in range(no_genes):
        fitness(i)
        
    genes = genes[fit.argsort()]
    fit = sorted(fit)

print "Gene with maximum fitness: "
print genes[0]
print "fitness: ",fit[0]

print "Total time taken = ",(time.time() - start_time)