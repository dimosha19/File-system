#include <iostream>
#include <utility>
#include <vector>

using namespace std;
bool HDD[100];
bool RAM[100];
class Note;
class File;
class Directory;
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
    int weigth;
    int * startFile;
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
        properties = _properties;
    };
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
    Properties properties{true, name, 0, nullptr};

	vector<Note*> List;

    explicit Directory(string _dirName) : name(move(_dirName)) {};

	void open() override {
		cout << "Opening Dir" << endl;
	}
	void Delete(const string& target) {
        for (auto i : List) {
            if (i->getProperties().name == target) {
                List.erase(remove(List.begin(), List.end(), i), List.end());
                break;
            }
            cout << "No such file or directory" << endl;
        }
	}
	void createFile(string fileName, int size) {
		auto * X = new File({false, move(fileName), size, nullptr});
		List.push_back(X);
	};
    void createDir(const string& newDirName){
        auto * X = new Directory(newDirName);
        List.push_back(X);
    }
    Properties getProperties() override {
        return properties;
	}
};

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


    return 0;
};