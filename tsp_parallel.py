import multiprocessing
from multiprocessing import Process
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

def gen_genes(i,return_dict):
    occupied = np.zeros((no_points),dtype=np.int)
    occupied[0] = 1
    for num in range(2,no_points+1):
        x = random.randint(0,no_points-1)
        while (occupied[x]!=0):
            x = (x+1)%no_points
        genes[i][x] = num
        occupied[x] = 1
    return_dict[i] = genes[i]

def fitness(i,return_dict):
    distance = 0
    for j in range(no_points-1):
        d = math.sqrt((points[genes[i][j]-1][0]-points[genes[i][j+1]-1][0])**2 + (points[genes[i][j]-1][1]-points[genes[i][j+1]-1][1])**2)
        distance += d
    return_dict[i] = distance

def crossover(i,return_dict1):
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
    return_dict1[i] = genes[i]
    return_dict1[i+1] = genes[i+1]
    return_dict1[i+half_genes] = genes[i+half_genes]
    return_dict1[i+1+half_genes] = genes[i+1+half_genes]

if __name__ == '__main__':
    start_time = time.time()

    for i in range(no_points):
        gen_random(i)
    
    manager = multiprocessing.Manager()
    return_dict = manager.dict()
    
    no_genes = 60
    p = []
    genes = np.ones((no_genes,no_points),dtype=np.int)
    for i in range(no_genes):
        p1 = multiprocessing.Process(target=gen_genes, args=(i,return_dict))
        p.append(p1)
    for i in range(no_genes):
        p[i].start()
    for i in range(no_genes):
        p[i].join()
    for i in range(no_genes):
        genes[i] = return_dict[i]

    fit = np.zeros((no_genes),dtype=np.int)
    p = []
    for i in range(no_genes):
        p1 = multiprocessing.Process(target=fitness, args=(i,return_dict))
        p.append(p1)
    for i in range(no_genes):
        p[i].start()
    for i in range(no_genes):
        p[i].join()
    for i in range(no_genes):
        fit[i] = return_dict[i]

    genes = genes[fit.argsort()]
    fit = sorted(fit)

    no_iterations = 5
    for iterations in range(no_iterations):
        half_genes = no_genes/2
        return_dict1 = manager.dict()
        p=[]
        for i in range(0,half_genes,2):
            p1 = multiprocessing.Process(target=crossover, args=(i,return_dict1))
            p.append(p1)
        for i in range(0,half_genes/2):
            p[i].start()
        for i in range(0,half_genes/2):
            p[i].join()
        for i in range(i):
            genes[i] = return_dict1[i]
        
        fit = np.zeros((no_genes),dtype=np.int)
        p = []
        for i in range(no_genes):
            p1 = multiprocessing.Process(target=fitness, args=(i,return_dict))
            p.append(p1)
        for i in range(no_genes):
            p[i].start()
        for i in range(no_genes):
            p[i].join()
        for i in range(no_genes):
            fit[i] = return_dict[i]
        
        genes = genes[fit.argsort()]
        fit = sorted(fit)
    
    print "Gene with maximum fitness: "
    print genes[0]
    print "fitness: ",fit[0]

    print "Total time taken = ",(time.time() - start_time)
