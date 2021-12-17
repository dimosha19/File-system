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
                    if ( j->name == i->getProperties().name) allFiles.erase(remove(allFiles.begin(), allFiles.end(), j), allFiles.end());
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
        if (i->getProperties().name == target) dynamic_cast<File *>(i)->open();
    }
}

void closeFile(const string& target){
    for (auto i : currentDir->List){
        if (i->getProperties().name == target) dynamic_cast<File *>(i)->close();
    }
}

void copy(const string & target){
    for (auto i : currentDir->List){
        if (i->getProperties().name == target) {
            currentDir->List.push_back(i->copy(i));
        };
    }
}

void path(){
    for (auto i: movement){
        cout << i->getProperties().name << "/";
    }
    cout << endl;
}

int main()
{
    rootDir = new Directory("rootDir");
    currentDir = rootDir;
    movement.push_back(rootDir);

    /*
    currentDir->createFile("file", 3);
    currentDir->createDir("1");
    changeCurrentDir("1");
    currentDir->createFile("fi", 2);
    currentDir->createDir("di");
    currentDir = rootDir;
    copy("1");
    goTo("1 copy");
    memView();

    printDir(currentDir); */
    /*  auto* Dir1 = new Directory;
	Dir1->CreateFile("File1", 1);
	Dir1->CreateFile("File2", 1);
	Directory dir;
	dir.List.push_back(Dir1);
	dir.CreateFile("MainDirFile", 10);
    dir.Print();
*/
    /*
    currentDir->createFile("file 1", 5);
    memView();
    currentDir->copy("file 1");
    memView();
    printDir(currentDir); */
    /*
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
    memView(); */
    /*
    currentDir->createFile("newFile", 10);
    currentDir->createFile("newFile1", 10);
    memView();

    openFile("newFile");
    openFile("newFile1");
    memView();

    closeFile("newFile");
    memView(); */
    /*
    currentDir->createDir("new dir");
    goTo("new dir");
    currentDir->createFile("file1", 4);
    currentDir->createFile("file2", 4);
    printDir(currentDir);
    memView();
    cout << endl;
    rootDir->Delete("new dir");
    cout << currentDir->name;
    printDir(currentDir);
    memView(); */



    return 0;
};