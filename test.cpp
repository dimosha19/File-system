#include <iostream>
using namespace std;

bool * HDD = new bool [12];

struct Node {
    int startHDD, endHDD;
    int startRAM = -1, endRAM = -1;
    Node* next;

    Node(int _startHDD, int _endHDD) : startHDD(_startHDD), endHDD(_endHDD), next(nullptr) {}
};

struct list {
    Node* first;
    Node* last;

    list() : first(nullptr), last(nullptr) {}

    bool is_empty() {
        return first == nullptr;
    }

    void push_back(int start, int end) {
        Node* p = new Node(start, end);
        if (is_empty()) {
            first = p;
            last = p;
            return;
        }
        last->next = p;
        last = p;
    }

    void print() {
        if (is_empty()) return;
        Node * p = first;
        while (p) {
            cout << p->startHDD << " " << p->endHDD << endl;
            p = p->next;
        }
        cout << endl;
    }
};

void Pusher(int size, list * list){
    int start = -1, end = -1;
    int i = 0;
    while (i <= 12 || size >= 0){
        if (!HDD[i] && start == -1){
            start = i;
            size -=1;
            HDD[i] = true;
            if (size == 0){
                end = start + 1;
                list->push_back(start, end);
                return;
            }
        }
        else if (!HDD[i] && start != -1){
            size -= 1;
            HDD[i] = true;
        }
        else if (HDD[i] && start != -1){
            end = i;
            list->push_back(start, end);
            if (size > 0) return Pusher(size, list);
        }
        i++;
    }
    cout << start << " " << end << endl;
}

void Cleaner(list * list){
    Node * p = list->first;
    while (p) {
        int start = p->startHDD, end = p->endHDD;
        for (; start < end; start++) HDD[start] = false;
        p = p->next;
    }
}

int main() {
    bool mirror[] = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0};
    for (int i = 0; i < 12; i++){
        HDD[i] = mirror[i];
    }
    int test = 7;
    list * l = new list;
    Pusher(7, l);
    for (int i = 0; i < 12; i++) cout << HDD[i] << ", ";
    cout << endl;
    l->print();
    Cleaner(l);
    for (int i = 0; i < 12; i++) cout << HDD[i] << ", ";
    return 0;
}