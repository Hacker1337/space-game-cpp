import matplotlib.pyplot as plt
dumps = []

with open("demos/demo_man.txt", 'r') as man:
    dumps = [line.strip() for line in man.readlines()]

for dump in dumps:
    x = []
    y = []
    planets = [[], []]
    with open(f"demos/{dump}.txt", 'r') as file:
        title = file.readline()
        line = file.readline()
        n_pl = int(line[:line.find(' ')])
        for i in range(n_pl):
            plx, ply = file.readline().strip().split(' ')
            planets[0].append(float(plx))
            planets[1].append(float(ply))
        file.readline()
        while (line := file.readline()) != '':
            x.append(float((line.strip().split() + [0])[0]))
            y.append(float((line.strip().split() + [0, 0])[1]))
    plt.figure(figsize=(10, 10))
    plt.title(title)
    plt.plot(x, y)
    plt.scatter(*planets)
    plt.savefig(f"demos/{dump}.png")
