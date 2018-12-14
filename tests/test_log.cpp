#include <stdio.h>
#include <cassert>
#include <stdlib.h>

int main()
{
  // number of threads
  int n;

  // number of messages
  int m;

  // reading first line
  assert(scanf("%d %d\n", &n, &m) == 2);

  // sequence numbers for each thread
  int* seqs = new int[n];

  // zeroing sequence numbers
  for(int i = 0; i < n; i++)
    seqs[i] = 0;

  // reading the rest
  for(int i = 0; i < n * m; i++)
  {
    // thread id
    int t;

    // message id
    int msg;

    // reading a message
    if(scanf("t %d m %d\n", &t, &msg) != 2)
    {
        printf("Cannot read next message, sequence = [");
        for(int j = 0; j < n; j++) printf("%d ", seqs[j]);
        printf("]\n");
        exit(1);
    }

    // sanity check
    assert(t < n);

    // checking that the next message is correct
    if(seqs[t] != msg)
    {
        printf("Got out-of-order log message from thread %d. Expected %d and got %d\n", t, seqs[t], msg);
        exit(1);
    }

    // incrementing corresponding sequence number
    seqs[t]++;
  }

  printf("CORRECT\n");
}
