import subprocess

def get_time(m = 500, n = 5):
    """ Get the time required to send m messages over n processes """
    proc = subprocess.Popen(("python run_performance.py .. %d %d 2>&1 > /dev/null" % (m, n)).split())
    proc.wait()
    return float(open('time.out', 'r').read())

