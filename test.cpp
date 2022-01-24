#include <iostream>
#include <vector>

using namespace std;

bool * HDD = new bool [12];
int fat[12];

int FirstPointerFinder(){
    for (int i = 0; i < 12; i++) { // TODO изменить range
        if (!HDD[i]) return i;
    }
    return -3;
}

void PusherHDD(int size, int first){
    int originalSize = size;
    int pointers[size];
    int n = 0;
    for (int i = first; i < 12; i++){ // TODO изменить range
        if (!HDD[i] && size > 0){
            HDD[i] = true;
            size--;
            pointers[n++] = i;
        }
    }
    for (int i = 0; i < originalSize - 1; i++){
        fat[pointers[i]] = pointers[i+1];
    }
    fat[pointers[originalSize - 1]] = -1;

}

void CleanerHDD(int first){
    while (fat[first] != -1){
        HDD[first] = false;
        first = fat[first];
    }
    HDD[first] = false;
}


int main() {
    bool mirror[] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0};
    for (int i = 0; i < 12; i++) {
        HDD[i] = mirror[i];
    }
    //
    for (int i = 0; i < 12; i++) {
        cout << HDD[i] << " ";
    }
    for (int & i : fat) {
        i = -2;
    }
    cout << endl;
    PusherHDD(4, FirstPointerFinder());
    for (int & i : fat) {
        cout << i << " ";
    }
    cout << endl;
    for (int i = 0; i < 12; i++) {
        cout << HDD[i] << " ";
    }
    cout << endl;
    PusherHDD(3, FirstPointerFinder());
    for (int & i : fat) {
        cout << i << " ";
    }
    cout << endl;
    CleanerHDD(7);
    for (int i = 0; i < 12; i++) {
        cout << HDD[i] << " ";
    }
    return 0;
}
