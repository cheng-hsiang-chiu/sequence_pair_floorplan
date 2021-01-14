import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import sys
import matplotlib as mpl

mpl.rcParams['agg.path.chunksize'] = 10000


inputfile = sys.argv[1]

cost = []
with open(inputfile) as f:
  costs = f.readlines()
  for c in costs:
    cost.append(int(c))



x = np.arange(0, len(cost), 1)
y = cost
print(x)
print(y)
#end = int(max(cost))
#start = int(min(cost))

#yticks = np.arange(start, end, 50000)
#plt.yticks(yticks)
plt.plot(y[:1000])

#fig, ax = plt.subplots()
#ax.plot(x, y)
#start, end = ax.get_ylim()
#ax.yaxis.set_ticks(np.arange(start, end, 10000))
#ax.semilogy(x, y)

plt.savefig("test.png")
#plt.show()

