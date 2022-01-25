#include <chrono>
#include <fstream>
using namespace std::chrono;
ofstream file(R"(C:\Users\Dmitriy\CLionProjects\tests\testzone\test.txt)");
unsigned int CreatingFileTiming[100];
unsigned int DeletingFileTiming[100];
unsigned int CreatingDirectoryTiming[100];
unsigned int DeletingDirectoryTiming[50];

int cf = 0;
int df = 0;
int cd = 0;
int dd = 0;

//testing files
for (int i = 0; i < 100; i++){
auto start = steady_clock::now();
currentDir->createFile("file", 2);
auto finish = steady_clock::now();
auto duration = finish - start;
CreatingFileTiming[cf++] = duration_cast<nanoseconds>(duration).count();
}
for (int i = 0; i < 100; i++){
auto start = steady_clock::now();
currentDir->Delete("file");
auto finish = steady_clock::now();
auto duration = finish - start;
DeletingFileTiming[df++] = duration_cast<nanoseconds>(duration).count();
}
for (unsigned int i : CreatingFileTiming){
file << i << " ";
}
file << endl;
for (unsigned int i : DeletingFileTiming){
file << i << " ";
}
file << endl;
//testing directory's
for (int i = 0; i < 100; i++){
auto start = steady_clock::now();
currentDir->createDir("dir");
//        goTo("dir");
auto finish = steady_clock::now();
auto duration = finish - start;
CreatingDirectoryTiming[cd++] = duration_cast<nanoseconds>(duration).count();
}
for (unsigned int i : CreatingDirectoryTiming){
file << i << " ";
}
file << endl;

for (int i = 0; i < 100; i++){
currentDir->Delete("dir");
}
for (int i = 0; i < 50; i++){
currentDir->createDir("dir" + to_string(i));
goTo("dir" + to_string(i));
currentDir->createFile("file", 2);
back();
}
for (int i = 0; i < 50; i++){
auto start = steady_clock::now();
currentDir->Delete("dir" + to_string(i));
auto finish = steady_clock::now();
auto duration = finish - start;
DeletingDirectoryTiming[dd++] = duration_cast<nanoseconds>(duration).count();
}

for (unsigned int i : DeletingDirectoryTiming){
file << i << " ";
}