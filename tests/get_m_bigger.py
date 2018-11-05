import numpy as np
from tqdm import tqdm
import pickle
from get_time import get_time

# repetitions
repetitions = range(5)

# n processes
n = 3

# messages
ms = np.arange(1000, 5000, 1000)

# results: map m messages -> array over repetitions
results = {m: [] for m in ms}

# doing the computations
for m in ms:
    for _ in tqdm(repetitions):
        results[m].append(get_time(m, n))

# saving data
open('results_m_bigger.pkl', 'wb').write(pickle.dumps(results))
