#include <iostream>
#include <vector>

using namespace std;

bool * HDD = new bool [12];

int main() {
    bool mirror[] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0};
    for (int i = 0; i < 12; i++) {
        HDD[i] = mirror[i];
    }
    vector<int> a {size(mirror)};
    //
    for (int i = 0; i < 12; i++) {
        cout << HDD[i] << " ";
    }
    //
    cout << endl;
    for (int i = 0; i < 12; i++) {
        cout << HDD[i] << " ";
    }
    return 0;
}

