import matplotlib.pyplot as plt
import numpy as np

n = np.linspace(1, 15, 15)

# disabled RTS/CTS
t = []
t.append(np.mean([3.05909]))
t.append(np.mean([2.84077, 2.81157]))
t.append(np.mean([2.13756, 2.16421, 2.13248]))
t.append(np.mean([1.69075, 1.64759, 1.6679, 1.67679]))
t.append(np.mean([1.36834, 1.34549, 1.43181, 1.40007, 1.35946]))
t.append(np.mean([1.26679, 1.02943, 1.26426, 1.25918, 1.21856, 1.03958]))
t.append(np.mean([2.47774, 2.47393, 2.42316, 2.48409, 2.43458, 2.43204, 2.4422]))
t.append(np.mean([2.51963, 2.39142, 2.41808, 2.48535, 2.42062, 2.43458, 2.64505, 2.47393]))
t.append(np.mean([2.38381, 2.37746, 2.43077, 2.42189, 2.4587, 2.45616, 2.48409, 2.42823, 2.45616]))
t.append(np.mean([2.39904, 2.4422, 2.46124, 2.43839, 2.39269, 2.52343, 2.40158, 2.45489, 2.49678, 2.44601]))
t.append(np.mean([2.49551, 2.41046, 2.43331, 2.4422, 2.47393, 2.40919, 2.48028, 2.51963, 2.42189, 2.52724, 2.41046]))
t.append(np.mean([2.45616, 2.48662, 2.49551, 2.43712, 2.44093, 2.41046, 2.55517, 2.40539, 2.46378, 2.51074, 2.41681, 2.51963]))
t.append(np.mean([2.44727, 2.44347, 2.42443, 2.48409, 2.39777, 2.49424, 2.38127, 2.48662, 2.4587, 2.45235, 2.51074, 2.40412, 2.45743]))
t.append(np.mean([2.44981, 2.48535, 2.4422, 2.48409, 2.49297, 2.44981, 2.47266, 2.44981, 2.46758, 2.42697, 2.48916, 2.4295, 2.44854, 2.44601]))
t.append(np.mean([2.42823, 2.4257, 2.46251, 2.50186, 2.42823, 2.46885, 2.38127, 2.44093, 2.45616, 2.3965, 2.41935, 2.46885, 2.41554, 2.49805, 2.44727]))

print(t)

plt.figure()
plt.plot(n, np.array(t))
plt.grid()
plt.show()