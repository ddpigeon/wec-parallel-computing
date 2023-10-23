import os
import sys
import matplotlib.pyplot as plt

os.system("g++ task_1.cpp -o task_1 -fopenmp")
os.system("g++ task_2.cpp -o task_2 -fopenmp")
os.system("g++ task_3.cpp -o task_3 -fopenmp")

choice = int(sys.argv[1])

i = 10
points = []
sequential_points = []
parallel_points = []

if choice == 1: 
    while i <= 1E9: 
        #Can change upper limits of i to whatever, personally kept this limit since I ran into memory issues at higher values
        points.append(i)
        pts = os.popen(f"./task_1 {i}").read().split()
        
        parallel_points.append(int(pts[0]))
        sequential_points.append(int(pts[1]))
    
        if str(i)[0] == '5':
            i *= 2
        else:
            i *= 5
    

elif choice == 2: 
    while i <= 2.5E3: 
        #Can change upper limits of i to whatever, personally kept this limit since I ran into memory issues at higher values
        points.append(i)
        pts = os.popen(f"./task_2 {i}").read().split()
        
        parallel_points.append(int(pts[0]))
        sequential_points.append(int(pts[1]))
    
        if str(i)[0] == '1':
            i *= 2.5
        else:
            i *= 2


elif choice == 3: 
    while i <= 1E3: 
        #Can change upper limits of i to whatever, personally kept this limit since I ran into memory issues at higher values
        points.append(i)
        pts = os.popen(f"./task_3 {i}").read().split()
        
        parallel_points.append(int(pts[0]))
        sequential_points.append(int(pts[1]))
    
        if str(i)[0] == '4':
            i *= 2.5
        else:
            i *= 2



print(points)
print(sequential_points)
print(parallel_points)

ratios = []

for j in range(len(points)):
    if parallel_points[j] >= sequential_points[j] or parallel_points[j] == 0:
        ratios.append(1)
    else:
        ratios.append(sequential_points[j] / parallel_points[j])

fig, (ax1, ax2) = plt.subplots(2)
ax1.plot(points, sequential_points, 'bo')
ax1.plot(points, sequential_points, 'b', label="Sequential")
ax1.plot(points, parallel_points, 'ro')
ax1.plot(points, parallel_points, 'r', label="Parallel")
ax1.set(xlabel="Size of array/side of matrix", ylabel="Time in ms", xscale="log")
ax1.set_ylim(bottom=0)
ax1.legend()
fig.suptitle(f"Task {choice}")

ax2.plot(points, ratios)
ax2.set(xlabel="Size of array/side of matrix", ylabel="Ratio of sequential/parallel", xscale = 'log')
ax2.set_ylim(bottom=0)

# remove the 'xscale = log' for linear scale


os.system("rm ./task_1")
os.system("rm ./task_2")
os.system("rm ./task_3")

plt.show()










