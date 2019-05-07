import csv  # for reading a csv file
from matplotlib import pyplot as plt  # for plotting

h, w = plt.figaspect(1)
fig, axes = plt.subplots(2, 2, sharey=True, figsize=(h, w))

algorithms = ["momentum", "nesterov", "adagrad", "adam"]

for (algorithm, axis) in zip(algorithms, axes.reshape(-1)):
    k = []
    f = []

    with open(f"logistic-{algorithm}.csv") as csvfile:
        csvReader = csv.reader(csvfile, delimiter=",")
        next(csvReader)  # skip the header
        for row in csvReader:
            k.append(int(row[0]))
            f.append(float(row[2]))

    axis.plot(k, f)
    axis.set_title(algorithm)
    axis.set_xlabel(r"$k$")
    axis.set_ylabel(r"$f(\cdot)$")
    axis.grid()

plt.tight_layout()
plt.savefig("logistic.svg")
plt.savefig("logistic.pdf")
