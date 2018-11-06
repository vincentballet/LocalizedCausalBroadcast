import numpy as np
from tqdm import tqdm
import pickle
from get_time import get_time

# repetitions
repetitions = range(5)

# n processes
ns = list(range(2, 7))

# messages
m = 10000

# results: map n proc -> array over repetitions
results = {n: [] for n in ns}

# doing the computations
for n in ns:
    for _ in tqdm(repetitions):
        results[n].append(get_time(m, n))

# saving data
open('results_n.pkl', 'wb').write(pickle.dumps(results))
