import csv  # for reading a csv file
from matplotlib import pyplot as plt  # for plotting

h, w = plt.figaspect(1)
fig, axes = plt.subplots(2, 2, sharey=True, figsize=(h, w))

algorithms = ["momentum", "nesterov", "adagrad", "adam"]

for (algorithm, axis) in zip(algorithms, axes.reshape(-1)):
    k = []
    f = []

    with open(f"logistic-l1-l2-{algorithm}.csv") as csvfile:
        csvReader = csv.reader(csvfile, delimiter=",")
        next(csvReader)  # skip the header
        for row in csvReader:
            k.append(int(row[0]))
            f.append(float(row[2]))

    axis.plot(k, f)
    axis.set_title(algorithm)
    axis.set_xlabel(r"Iteration ($k$)")
    axis.set_ylabel(r"Total Loss")
    axis.grid()

plt.tight_layout()
plt.savefig("logistic-l1-l2.svg")
plt.savefig("logistic-l1-l2.pdf")
