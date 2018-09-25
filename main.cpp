#include <iostream>
#include <string>
#include <stdlib.h>

using std::string;
using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    argc--;
    if(argc < 2)
    {
        cout << "Usage: " << argv[0] << " n membership [extra params...]" << endl;
    }
    else if(argc == 2)
    {
        int n = atoi(argv[1]);
        string membership = argv[2];
        cout << n << membership;
    }
    cout << argc;
    return 0;
}
