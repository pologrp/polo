import csv  # for reading a csv file
from matplotlib import pyplot as plt  # for plotting

k = []
t = []
f = []

with open("terminator.csv") as csvfile:
    csvReader = csv.reader(csvfile, delimiter=",")
    next(csvReader)  # skip the header
    for row in csvReader:
        k.append(int(row[0]))
        t.append(float(row[1]))
        f.append(float(row[2]))

h, w = plt.figaspect(0.5)
fig, axes = plt.subplots(1, 2, sharey=True, figsize=(h, w))

# f vs k
axes[0].plot(k, f)
axes[0].set_xlabel(r"Iteration ($k$)")
axes[0].set_ylabel(r"Total Loss")
axes[0].grid()

# f vs t
axes[1].plot(t, f)
axes[1].set_xlabel(r"Time ($t$) [ms]")
axes[1].grid()

plt.tight_layout()
plt.savefig("terminator.svg")
plt.savefig("terminator.pdf")
