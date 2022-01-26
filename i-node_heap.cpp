#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <regex>

using namespace std;
bool HDD[30]; // При изменении обратить внимание на конструктор файла и метод открытия
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

void CleanerHDD(map<int, int>& node){
    for (auto i : node) {
        int start = i.first, end = i.second;
        for (; start < end; start++) HDD[start] = false;
    }
}

struct Properties{
    bool isDir;
    char * name = nullptr;
    int size;
    bool isPtr = false;
    File* parent = nullptr;
    unsigned char mod = 0;
    map<int, int> nodeHDD;
    int strongLinks = 0;
    int startRAM = -1;

    string GetName() const{
        string sname;
        for (int i = 0; name[i]!= 0; i++){
            sname+=name[i];
        }
        return sname;
    }
};

char * ToChar(const string& str, const string& str2){
    int n = 0;
    char * cname = new char[str.size() + str2.size() + 1] {};
    for (auto i : str){
        cname[n++] = i;
    }
    for (auto i : str2){
        cname[n++] = i;
    }
    return cname;
}

class Note {
public:
	virtual void open() = 0;
	virtual Properties getProperties() = 0;
    virtual Note* copy(Note* file) = 0;
};


class File :public Note {
public:
    Properties properties;
	explicit File(Properties _properties) : properties(_properties){
        properties.mod |= isReadable | isWriteable | isExecutable;
        if (properties.size > 0){
            PusherHDD(properties.size, properties.nodeHDD);
            allFiles.push_back(&properties);
        }
    };
    ~File(){
        close();
        CleanerHDD(properties.nodeHDD);
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
        copiedProp.name = ToChar(copiedProp.GetName(), "_copy");
        copiedProp.nodeHDD.clear();
        copiedProp.startRAM = -1;
        Note * newfile = new File(copiedProp);
        return newfile;
    }
    void open() override {
        if (static_cast<bool>(properties.mod & isExecutable && properties.startRAM == -1)){
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
    Properties properties{true, ToChar(name, ""), 0};
	vector<Note *> List;

    explicit Directory(string _dirName) : name(move(_dirName)) {};
    ~Directory(){
        for (auto i : List){
            Delete(i->getProperties().GetName());
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
            if (i->getProperties().GetName() == target) {
                for (auto j : allFiles){
                    if ( j->GetName() == i->getProperties().GetName()) allFiles.erase(remove(allFiles.begin(), allFiles.end(), j), allFiles.end());   // erase(remove(allFiles.begin(), allFiles.end(), j), allFiles.end());
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
        auto X = new File({false, ToChar(fileName, ""), size});
        X->properties.strongLinks++;
        List.push_back(X);
	};

    void createFileWeakPtr(const string& ptrName, const string& parentName) {
        for (auto i : List){
            if (i->getProperties().GetName() == parentName) {
                auto X = new File({false, ToChar(ptrName, ""), 0, true, dynamic_cast<File *>(i)});
                List.push_back(X);
            }
        }
    };
    void createFileStrongPtr(const string& ptrName, const string& parentName) {
        for (auto i : List){
            if (i->getProperties().name == parentName) {
                auto X = new File({false, ToChar(ptrName, ""), 0, true, dynamic_cast<File *>(i)});
                dynamic_cast<File *>(i)->AddLink();
                X->properties.size = i->getProperties().size;
                X->properties.nodeHDD = i->getProperties().nodeHDD;
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
        auto * copy = new Directory(ToChar(properties.GetName(), "_copy"));
        for (auto i : List){
            copy->List.push_back(i->copy(i));
        }
        return copy;
    }
    void createDir(const string& newDirName){
        auto X = new Directory(ToChar(newDirName, ""));
        List.push_back(X);
    }
    Properties getProperties() override {
        return properties;
	}
};

void printDir(Directory * dir) {
    for (auto x : dir->List) {
        cout << x->getProperties().GetName() << endl;
    }
}

void goTo(const string& target){
    for (auto i : currentDir->List) {
        if (i->getProperties().isDir && i->getProperties().GetName() == target) {
            currentDir = dynamic_cast<Directory *>(i);
            movement.push_back(currentDir);
            break;
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
    for (auto i : currentDir->List){
        if (i->getProperties().GetName() == target && !i->getProperties().isPtr) {
            dynamic_cast<File *>(i)->open();
            return;
        }
        else if (i->getProperties().GetName() == target && i->getProperties().isPtr) {
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
    cout << "No such file (or parent file)"<< endl;
}

void closeFile(const string& target){
    for (auto i : currentDir->List){
        if (i->getProperties().GetName() == target && !i->getProperties().isPtr) {
            dynamic_cast<File *>(i)->close();
            break;
        }
        else if (i->getProperties().GetName() == target && i->getProperties().isPtr) {
            if (i->getProperties().size!=0)dynamic_cast<File *>(i)->close();
            else i->getProperties().parent->close();
            break;
        }
    }
}

void copy(const string & target){
    for (auto i : currentDir->List){
        if (i->getProperties().GetName() == target) {
            currentDir->List.push_back(i->copy(i));
        }
    }
}

string path(){
    string s;
    for (auto i: movement){
        s += i->getProperties().GetName();
        s+="/";
    }
    return s;
}
// [rwe]
void chmod(const string &target) {
    for (auto i : currentDir->List){
        if (i->getProperties().GetName() == target && !i->getProperties().isDir) {
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
            if (regex_match(i->getProperties().GetName(), r)){
                arr.push_back(i->getProperties().GetName());
            }
        }
        return arr;
    }
    for (auto i : currentDir->List){
        if (i->getProperties().GetName().find(target) != string::npos){
            arr.push_back(i->getProperties().GetName());
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
        }else if (command == "touchstrong") {
            cin >> name;
            cin >> parrentName;
            currentDir->createFileStrongPtr(name, parrentName);
        }else if (command == "rm"){
            cin >> name;
            currentDir->Delete(name);
        }else if (command == "mw"){
            memView();
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