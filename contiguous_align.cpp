#include <iostream>
#include <utility>
#include <vector>
#include <regex>

using namespace std;
bool HDD[30];
bool RAM[30];
struct Properties;
class Note;
class Directory;
class File;
vector<Properties *> allFiles;
vector<Directory *> movement;
Directory * currentDir;
Directory * rootDir;
unsigned char isReadable;
unsigned char isExecutable;
unsigned char isWriteable;
unsigned char isArchived;
unsigned char isHidden;

struct Properties{
    bool isDir;
    string name;
    int size, startHDD, startRAM;
    bool isPtr = false;
    File* parent = nullptr;
    unsigned char mod = 0;
    int strongLinks = 0;
};

class Note {
public:
	virtual void open() = 0;
	virtual Properties getProperties() = 0;
    virtual Note* copy(Note* file) = 0;
};

void defragm(){
    int holeStart = 0;
    int holeEnd = 0;
    bool flag = false;
    int i = 0;
    while (i != 30){
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

class File :public Note {
public:
    Properties properties;
	explicit File(Properties _properties) : properties(move(_properties)){
        properties.mod |= isReadable | isWriteable | isExecutable;
        if (properties.size > 0){
            for (int k = 0; k < 2; k++) {
                int start = 0, end = 0;
                for (auto i: HDD) {
                    if (end - start == properties.size) {
                        properties.startHDD = start;
                        allFiles.push_back(&properties);
                        for (; start < end; start++) {
                            HDD[start] = true;
                        }
                        k = 3;
                        break;
                    } else if (!i && end - start < properties.size) {
                        end++;
                    } else if (i && end - start < properties.size) {
                        start = end + 1;
                        end++;
                    }
                }
                if (k != 3) defragm();
                if (k == 2) {
                    cout << "not enough storage" << endl;
                }
            }
        }
    };
    ~File(){
        close();
        for (int i = properties.startHDD; i < properties.size + properties.startHDD; i++) {
            HDD[i] = false;
        }
    }
    void del(){
        if (properties.isPtr){
            if (properties.size!=0){
                properties.parent->properties.strongLinks--;
                if (properties.parent->properties.strongLinks==0) delete this;
                return;
            } else {
                return;
            }
        }if (properties.strongLinks > 1){
            properties.strongLinks--;
            return;
        } else if (properties.strongLinks == 1) {
            delete this;
        }
    }
    void chmod(){
        properties.mod ^= isExecutable;
    }
    Note * copy(Note* file) override {
        Properties copiedProp = file->getProperties();
        copiedProp.name+= "_copy";
        return new File(copiedProp);
    }
	void open() override {
        if (static_cast<bool>(properties.mod & isExecutable)){
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
        } else {
            cout << "file not executable" << endl;
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
    void AddLink(){
        properties.strongLinks++;
    }
    void DeductLink(){
        properties.strongLinks--;
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
                else dynamic_cast<File *>(i)->del();
                List.erase(remove(List.begin(), List.end(), i), List.end());
                return;
            }
        }
        cout << "No such file or directory" << endl;
	}
	void createFile(const string& fileName, int size) {
        auto X = new File({false, fileName, size, -1});
        X->properties.strongLinks++;
		List.push_back(X);
	};

    void createFileWeakPtr(const string& ptrName, const string& parentName) {
        for (auto i : List){
            if (i->getProperties().name == parentName) {
                auto X = new File({false, ptrName, 0, -1, -1, true, dynamic_cast<File *>(i)});
                List.push_back(X);
            }
        }
    };
    void createFileStrongPtr(const string& ptrName, const string& parentName) {
        for (auto i : List){
            if (i->getProperties().name == parentName) {
                auto X = new File({false, ptrName, 0, -1, -1, true, dynamic_cast<File *>(i)});
                dynamic_cast<File *>(i)->AddLink();
                X->properties.size = i->getProperties().size;
                X->properties.startHDD = i->getProperties().startHDD;
                X->properties.startRAM = i->getProperties().startRAM;
                allFiles.push_back(&X->properties);
                List.push_back(X);
            }
        }
    };

    Note * copy(Note * file) override{
        // create dir
        // fill dir
        // return * dir
        auto * copy = new Directory(name + "_copy");
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
    currentDir = movement.back();
}

void memView(){
    cout << "HDD: [ ";
    for (bool i : HDD){
         cout << i;
    }
    cout << " ]" << endl;
    cout << "RAM: [ ";
    for (bool i : RAM){
        cout << i;
    }
    cout << " ]" << endl;
}

void openFile(const string& target){
    for (auto i : currentDir->List) {
        if (i->getProperties().name == target && !i->getProperties().isPtr) {
            dynamic_cast<File *>(i)->open();
            return;
        } else if (i->getProperties().name == target && i->getProperties().isPtr) {
            if (i->getProperties().size!=0){
                dynamic_cast<File *>(i)->open();
                return;
            } else{
                for (auto j : allFiles){
                    if (j == &i->getProperties().parent->properties) {
                        i->getProperties().parent->open();
                        return;
                    }
                }
            }
            cout << "No parent" << endl;
            return;
        }
    }
    /*
 touch file 4
 touchweak ptr file
 rm file
 open ptr
    */
    cout << "No such file (or parent file)"<< endl;
}

void closeFile(const string& target){
    for (auto i : currentDir->List){
        if (i->getProperties().name == target && !i->getProperties().isPtr) {
            dynamic_cast<File *>(i)->close();
            break;
        }
        else if (i->getProperties().name == target && i->getProperties().isPtr) {
            i->getProperties().parent->close();
            break;
        }
    }
}

void copy(const string & target){
    for (auto i : currentDir->List){
        if (i->getProperties().name == target) {
            currentDir->List.push_back(i->copy(i));
        }
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
// [rwe]
void chmod(const string &target) {
    for (auto i : currentDir->List){
        if (i->getProperties().name == target && !i->getProperties().isDir) {
            dynamic_cast<File *>(i)->chmod();
        }
    }
}
vector<string> find(string& target){
    vector<string> arr;
    char first = target[0];
    regex r(R"(\w*.txt)");
    if (first == '*'){
        for (auto i : currentDir->List){
            if (regex_match(i->getProperties().name, r)){
                arr.push_back(i->getProperties().name);
            }
        }
        return arr;
    }
    for (auto i : currentDir->List){
        if (i->getProperties().name.find(target) != string::npos){
            arr.push_back(i->getProperties().name);
        }
    }
    return arr;
}

int main()
{
    rootDir = new Directory("rootDir");
    currentDir = rootDir;
    movement.push_back(currentDir);
    string command, name, parrentName;
    int size = 3;
    isReadable = 0x01;
    isExecutable = 0x02;
    isWriteable = 0x04;
    isArchived = 0x08;
    isHidden = 0x10;

    while (command != "exit"){
        cout << path() << ":";
        cin >> command;
        // mkdir, touch, open, cd, copy, touchweak, ls, delete
        if (command == "mkdir"){
            cin >> name;
            currentDir->createDir(name);
        }else if (command == "touch"){
            cin >> name;
            cin >> size;
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
        }else if (command == "touchstrong"){
            cin >> name;
            cin >> parrentName;
            currentDir->createFileStrongPtr(name, parrentName);
        }else if (command == "rm"){
            cin >> name;
            currentDir->Delete(name);
        }else if (command == "mw"){
            memView();
        }else if (command == "defrag"){
            defragm();
        }else if (command == "find"){
            cin >> name;
            for (const auto& i : find(name)){
                cout << i << endl;
            }
        }else if (command == "chmod"){
            cin >> name;
            chmod(name);
        }else{
            cout << "No such command" << endl;
        }
        }
    return 0;
}