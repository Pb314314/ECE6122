'''
Author: Bo Pang
Class:  ECE6122  2023-10-7
Description: This contains the implementation of Lab2.
'''
import matplotlib.pyplot as plt

# Data points
num_threads = [1, 2, 4, 8, 16]
processing_time = [8330, 4300, 1700, 1186, 536]

# Create the graph
plt.figure(figsize=(8, 6))
plt.plot(num_threads, processing_time, marker='o', linestyle='-')
plt.title('Processing Time vs. Number of Threads')
plt.xlabel('Number of Threads')
plt.ylabel('Processing Time (microseconds)')
plt.grid(True)

# Annotate data points
for i, txt in enumerate(processing_time):
    plt.annotate(txt, (num_threads[i], processing_time[i]), textcoords="offset points", xytext=(0, 10), ha='center')
plt.savefig("time_thread.png")

# Show the graph
plt.show()
