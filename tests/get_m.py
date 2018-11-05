import numpy as np
from tqdm import tqdm
import pickle
from get_time import get_time

# repetitions
repetitions = range(5)

# n processes
n = 5

# messages
ms = np.arange(500, 1000, 50)

# results: map m messages -> array over repetitions
results = {m: [] for m in ms}

# doing the computations
for m in ms:
    for _ in tqdm(repetitions):
        results[m].append(get_time(m, n))

# saving data
open('results_m.pkl', 'wb').write(pickle.dumps(results))
