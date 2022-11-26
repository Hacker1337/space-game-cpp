import matplotlib.pyplot as plt
x = []
y = []
with open("dump.traj.txt", 'r') as file:
    while (line := file.readline()) != '':
        x.append(float((line.strip().split() + [0])[0]))
        y.append(float((line.strip().split() + [0, 0])[1]))
plt.plot(x, y)
plt.scatter([250, 1250, 2000], [250, 550, 0])
plt.show()
