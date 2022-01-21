#include <iostream>
#include <utility>
#include <vector>
#include "memory"

using namespace std;
bool HDD[100];
bool RAM[100];
struct Properties;
class Note;
class Directory;
class File;
vector<Properties *> allFiles;
vector<Directory *> movement;
Directory * currentDir;
Directory * rootDir;

/*class Block {
public:
	Block(int id) :_id(id) {};
	int _id;
	Block* Next=nullptr;
};

class LinkList {
public:
	Block* First;

	LinkList() {
		First = new Block(-1);
	}

	void PushBack(int id ) {
		Block* ptr;
		ptr = First;
		while (ptr != nullptr) {
			if (ptr->Next == nullptr) { break; }
			ptr = ptr->Next;
		}
		ptr->Next = new Block(id);
	}
	void Print() {
		Block* ptr;
		ptr = First->Next;
		while (ptr != nullptr) {
			std::cout << ptr->_id << std::endl;
			ptr = ptr->Next;
		}
	}
};
*/

struct Properties{
    bool isDir;
    string name;
    int size, startHDD, startRAM;
    bool isPtr = false;
    string parentName = "None";
};

class Note {
public:
	virtual void open() = 0;
	virtual Properties getProperties() = 0;
    virtual Note* copy(Note* file) = 0;
};

class File :public Note {
public:
    Properties properties;
	explicit File(Properties _properties) : properties(move(_properties)){
        if (!properties.isPtr){
            int start = 0, end = 0;
            for (auto i : HDD){
                if (end - start + 1 == properties.size) {
                    properties.startHDD = start;
                    allFiles.push_back(&properties);
                    for (;start <= end; start++){
                        HDD[start] = true;
                    }
                    break;
                }
                else if (!i && end - start < properties.size) end++;
                else if (i && end - start < properties.size) {
                    start = end + 1;
                    end++;
                }
            }
        } else {

        }
    };
    ~File(){
        for (int i = properties.startHDD; i < properties.size + properties.startHDD; i++){
            HDD[i] = false;
        }
    }
    Note * copy(Note* file) override {
        return new File({false, file->getProperties().name + " copy", file->getProperties().size, -1});
    }
	void open() override {
        int start = 0, end = 0;
        for (auto i : RAM){
            if (end - start + 1 == properties.size) {
                properties.startRAM = start;
                allFiles.push_back(&properties);
                for (;start <= end; start++){
                    RAM[start] = true;
                }
                break;
            }
            else if (!i && end - start < properties.size) end++;
            else if (i && end - start < properties.size) {
                start = end + 1;
                end++;
            }
        }
	}
    void close() const{
        for (int i = properties.startRAM; i < properties.size + properties.startRAM; i++){
            RAM[i] = false;
        }
    }
    Properties getProperties() override {
        return properties;
	}

};
 
class Directory :public Note {

public:
    string name;
    Properties properties{true, name, 0, -1};
	vector<Note *> List;

    explicit Directory(string _dirName) : name(move(_dirName)) {};
    ~Directory(){
        for (auto i : List){
            Delete(i->getProperties().name);
        }
        currentDir = rootDir;
    }
    Directory(const Directory &directory){

    }
	void open() override {
		cout << "Opening Dir" << endl;
	}
	void Delete(const string& target) {
        for (auto i : List) {
            if (i->getProperties().name == target) {
                for (auto j : allFiles){
                    if ( j->name == i->getProperties().name) allFiles.erase(remove(allFiles.begin(), allFiles.end(), j), allFiles.end());   // erase(remove(allFiles.begin(), allFiles.end(), j), allFiles.end());
                }
                if (i->getProperties().isDir) delete dynamic_cast<Directory *>(i);
                else delete dynamic_cast<File *>(i);
                List.erase(remove(List.begin(), List.end(), i), List.end());
                return;
            }
        }
        cout << "No such file or directory" << endl;
	}
	void createFile(const string& fileName, int size) {
        auto X = new File({false, fileName, size, -1});
		List.push_back(X);
	};

    void createFileWeakPtr(const string& ptrName, const string& parentName) {
        auto X = new File({false, ptrName, 0, -1, -1, true, parentName});
        List.push_back(X);
    };

    Note * copy(Note * file) override{
        // create dir
        // fill dir
        // return * dir
        auto * copy = new Directory(name + " copy");
        for (auto i : List){
            copy->List.push_back(i->copy(i));
        }
        return copy;
    }
    void createDir(const string& newDirName){
        auto X = new Directory(newDirName);
        List.push_back(X);
    }
    Properties getProperties() override {
        return properties;
	}
};

void defragm(){
    int holeStart = 0;
    int holeEnd = 0;
    bool flag = false;
    int i = 0;
    while (i != 100){
        if (!HDD[i] && !flag) {
            flag = true;
            holeStart = i;
        } else if (HDD[i] && flag) {
            flag = false;
            holeEnd = i;
            for (auto k: allFiles) {
                if (k->startHDD >= holeEnd) {
                    for (int j = k->startHDD; j < k->startHDD + k->size; j++) {
                        HDD[j] = false;
                    }
                    k->startHDD -= (holeEnd - holeStart);
                    for (int j = k->startHDD; j < k->startHDD + k->size; j++) {
                        HDD[j] = true;
                    }
                }
            }
            holeStart = 0;
            holeEnd = 0;
            i = 0;
        }
        i++;
    }
}

void printDir(Directory * dir) {
    for (auto x : dir->List) {
        cout << x->getProperties().name << endl;
    }
}

void goTo(const string& target){
    for (auto i : currentDir->List) {
        if (i->getProperties().isDir && i->getProperties().name == target) {
            currentDir = dynamic_cast<Directory *>(i);
            movement.push_back(currentDir);
        }
    }
}
void back(){
    if (movement.size() == 1) return;
    movement.pop_back();
    currentDir = movement[-1];
}

void memView(){
    cout << "HDD: [ ";
    for (int i = 0; i < 30; i++){
         cout << HDD[i];
    }
    cout << " ]" << endl;
    cout << "RAM: [ ";
    for (int i = 0; i < 30; i++){
        cout << RAM[i];
    }
    cout << " ]" << endl;
}

void openFile(const string& target){
    for (auto i : currentDir->List){
        if (i->getProperties().name == target && !i->getProperties().isPtr) {
            dynamic_cast<File *>(i)->open();
            return;
        }
        else if (i->getProperties().name == target && i->getProperties().isPtr) {
            openFile(i->getProperties().parentName);
            return;
        }
    }
    cout << "No such file (or parent file)"<< endl;
}

void closeFile(const string& target){
    for (auto i : currentDir->List){
        if (i->getProperties().name == target && !i->getProperties().isPtr) dynamic_cast<File *>(i)->close();
        else if (i->getProperties().name == target && i->getProperties().isPtr) closeFile(i->getProperties().parentName);
    }
}

void copy(const string & target){ //TODO проверить на копирование ярлыка
    for (auto i : currentDir->List){
        if (i->getProperties().name == target) {
            currentDir->List.push_back(i->copy(i));
        };
    }
}

string path(){
    string s;
    for (auto i: movement){
        s += i->getProperties().name;
        s+="/";
    }
    return s;
}

int main()
{
    rootDir = new Directory("rootDir");
    currentDir = rootDir;
    movement.push_back(rootDir);
    string command, name, parrentName;
    int size = 3;
    while (command != "exit"){
        cout << path() << ": ";
        cin >> command;
        // mkdir, touch, open, cd, copy, touchweak, ls, delete
        if (command == "mkdir"){
            cin >> name;
            currentDir->createDir(name);
        }else if (command == "touch"){
            cin >> name;
            currentDir->createFile(name, size);
        }else if (command == "open"){
            cin >> name;
            openFile(name);
        }else if (command == "close"){
            cin >> name;
            closeFile(name);
        }else if (command == "cd"){
            cin >> name;
            if (name == "..") back();
            else goTo(name);
        }else if (command == "ls"){
            printDir(currentDir);
        }else if (command == "copy"){
            cin >> name;
            copy(name);
        }else if (command == "touchweak"){
            cin >> name;
            cin >> parrentName;
            currentDir->createFileWeakPtr(name, parrentName);
        }else if (command == "delete"){
            cin >> name;
            currentDir->Delete(name);
        }else if (command == "mw"){
            memView();
        }else{
            cout << "No such command" << endl;
        }
        }
    return 0;
};