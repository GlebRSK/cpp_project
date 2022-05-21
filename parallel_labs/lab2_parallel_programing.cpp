#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <ctime>

using namespace std;

#define N 8

string passwd = "";
thread threads[3];
mutex mut_passwd;

void Symble(int i){ 
    char s;
        unique_lock<mutex> locker(mut_passwd);
        if (i == 0){ s = (char)(rand() % 26 + 65);}
        if (i == 1){ s = (char)(rand() % 10 + 48);}
        if (i == 2){ s = (char)(rand() % 26 + 97);}
        if (passwd.length() < N){
        passwd += s;
        }else{
            mut_passwd.unlock();
        }
        mut_passwd.unlock();
}

int main() {
    srand(time(0));
    while (passwd.length() < N){
        for (int i = 0; i < 3; i++){
            threads[i] =  thread(Symble,i);
        }
        for (int i = 0; i < 3; i++){
            threads[i].join();
        }
    }
    cout << passwd << endl;
    return 0;
}
