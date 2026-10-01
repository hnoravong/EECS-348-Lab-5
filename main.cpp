#include <iostream>
#include <fstream>

using namespace std;


int main() {
    ifstream file("input.txt");
    int N;
    file >> N;
    cout << "N = " << N << endl;
    return 0;
}