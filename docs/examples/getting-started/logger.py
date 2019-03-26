import csv # for reading a csv file
from matplotlib import pyplot as plt # for plotting

k = []; t = []; f = []

with open("logger.csv") as csvfile:
  csvReader = csv.reader(csvfile, delimiter = ",")
  next(csvReader) # skip the header
  for row in csvReader:
    k.append(int(row[0]))
    t.append(float(row[1]))
    f.append(float(row[2]))

h, w = plt.figaspect(0.5)
fig = plt.figure(figsize = (h, w))
ax = plt.subplot(1, 2, 1)
plt.plot(k, f)
plt.xlabel("$k$")
plt.ylabel("$f(\cdot)$")
ax.grid()
ax = plt.subplot(1, 2, 2)
plt.plot(t, f)
plt.xlabel("$t$ [ms]")
ax.grid()
plt.tight_layout()
plt.savefig("logger.svg")
plt.savefig("logger.pdf")
