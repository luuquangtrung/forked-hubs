import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as mpatches

n = 10

l1 = []
l2 = []
l3 = []
l4 = []
l5 = []
avg = [l1, l2, l3, l4, l5]
for i in range(1, n+1):
    with open("logs/r%d_p1000.txt" %i) as f:
        lines = f.readlines()

    for line in lines:
        if line[0] == 'V':
            avgConv = line.split()
            l1.append(float(avgConv[1]))

    with open("logs/r%d_p800.txt" %i) as f:
        lines = f.readlines()

    for line in lines:
        if line[0] == 'V':
            avgConv = line.split()
            l2.append(float(avgConv[1]))

    with open("logs/r%d_p600.txt" %i) as f:
        lines = f.readlines()

    for line in lines:
        if line[0] == 'V':
            avgConv = line.split()
            l3.append(float(avgConv[1]))

    with open("logs/r%d_p400.txt" %i) as f:
        lines = f.readlines()

    for line in lines:
        if line[0] == 'V':
            avgConv = line.split()
            l4.append(float(avgConv[1]))

    with open("logs/r%d_p200.txt" %i) as f:
        lines = f.readlines()

    for line in lines:
        if line[0] == 'V':
            avgConv = line.split()
            l5.append(float(avgConv[1]))

fig = plt.figure()
ax = fig.add_subplot(111)
width = 0.2
ind = np.arange(n)

plotBar = ax.bar(ind-0.2, avg[0], width, color='blue', align='center')
plotBar2 = ax.bar(ind-0.1, avg[1], width, color='red', align='center')
plotBar3 = ax.bar(ind, avg[2], width, color='green', align='center')
plotBar4 = ax.bar(ind+0.1, avg[3], width, color='yellow', align='center')
plotBar5 = ax.bar(ind+0.2, avg[4], width, color='cyan', align='center')

ax.set_xlim(-(width+0.5), (n)+width)
ax.set_ylim(0, max(avg[0])+0.5)
ax.set_ylabel('Throughput [Mbps]')
ax.set_title('Varying the packet size')
xTickMarks = ['1 node', '2 nodes', '3 nodes', '4 nodes', '5 nodes',
    '6 nodes', '7 nodes', '8 nodes', '9 nodes', '10 nodes']
ax.set_xticks(ind+width/2)
xtickNames = ax.set_xticklabels(xTickMarks)
plt.setp(xtickNames, rotation=60, fontsize=10)

ax.legend( (plotBar[0], plotBar2[0], plotBar3[0], plotBar4[0], plotBar5[0]),
    ('1000 B', '800 B', '600 B', '400 B', '200 B'))


fig.savefig('images/packetSize.png')
plt.show()
