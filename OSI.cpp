#include <iostream>
#include <utility>
#include <vector>
#include "memory"

using namespace std;
bool HDD[100];
bool RAM[100];
struct Properties;
class Note;
class File;
class Directory;
vector<Properties *> allFiles;
Directory * currentDir;

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
    int size, start;
};

class Note {
public:
	virtual void open() = 0;
	virtual Properties getProperties() = 0;
};

class File :public Note {
public:
    Properties properties;
	explicit File(Properties _properties) : properties(move(_properties)){
        int start = 0, end = 0;
        for (auto i : HDD){
            if (end - start + 1 == properties.size) {
                properties.start = start;
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
    };
    ~File(){
        for (int i = properties.start; i < properties.size + properties.start; i++){
            HDD[i] = false;
        }
    }
	void open() override {
		cout << "Opening File" << endl;
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

	void open() override {
		cout << "Opening Dir" << endl;
	}
	void Delete(const string& target) {
        for (auto i : List) {
            if (i->getProperties().name == target) {
                for (auto j : allFiles){
                    if ( j->name == i->getProperties().name) allFiles.erase(remove(allFiles.begin(), allFiles.end(), j), allFiles.end());
                }
                delete dynamic_cast<File *>(i);
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
                if (k->start >= holeEnd) {
                    for (int j = k->start; j < k->start + k->size; j++) {
                        HDD[j] = false;
                    }
                    k->start -= (holeEnd - holeStart);
                    for (int j = k->start; j < k->start + k->size; j++) {
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

void changeCurrentDir(const string& target){
    for (auto i : currentDir->List) {
        if (i->getProperties().isDir && i->getProperties().name == target) currentDir = dynamic_cast<Directory *>(i);
    }
}

void memView(){
    cout << "HDD: [ ";
    for (int i = 0; i < 30; i++){
         cout << HDD[i];
    }
    cout << " ]" << endl;
}

int main()
{
/*  auto* Dir1 = new Directory;
	Dir1->CreateFile("File1", 1);
	Dir1->CreateFile("File2", 1);
	Directory dir;
	dir.List.push_back(Dir1);
	dir.CreateFile("MainDirFile", 10);
    dir.Print();
*/

    auto * rootDir = new Directory("rootDir");
    currentDir = rootDir;

    currentDir->createFile("N1", 2);
    memView();
    currentDir->createFile("N2", 2);
    memView();
    currentDir->createFile("N3", 2);
    memView();
    currentDir->createFile("N4", 2);
    memView();
    currentDir->createFile("N5", 2);
    memView();
    currentDir->Delete("N2");
    memView();
    currentDir->Delete("N4");
    memView();
    defragm();
    memView();
    return 0;
};