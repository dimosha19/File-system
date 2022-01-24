#include <iostream>
#include <map>

using namespace std;

bool * HDD = new bool [12];

void PusherHDD(int size, map<int, int>& node){
    int start = -1, end = -1;
    int i = 0;
    while (i <= 30 || size >= 0){
        if (!HDD[i] && start == -1){
            start = i;
            size -=1;
            HDD[i] = true;
            if (size == 0){
                end = start + 1;
                node[start] = end;
                return;
            }
        }
        else if (!HDD[i] && start != -1){
            size -= 1;
            HDD[i] = true;
            if (size == 0){
                end = i + 1;
                node[start] = end;
                return;
            }
        }
        else if (HDD[i] && start != -1){
            end = i;
            node[start] = end;
            if (size > 0) return PusherHDD(size, node);
        }
        i++;
    }
}

void Cleaner(map<int, int>& node){
    for (auto i : node) {
        int start = i.first, end = i.second;
        for (; start < end; start++) HDD[start] = false;
    }
}

int main() {
    bool mirror[] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0};
    for (int i = 0; i < 12; i++) {
        HDD[i] = mirror[i];
    }
    map<int, int> node;

    PusherHDD(7, node);

    for (auto i : node){
        cout << i.first << " " << i.second << endl;
    }
    for (int i = 0; i < 12; i++) {
        cout << HDD[i] << " ";
    }
    Cleaner(node);
    cout << endl;
    for (int i = 0; i < 12; i++) {
        cout << HDD[i] << " ";
    }

    return 0;
}