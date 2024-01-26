#include "graph.h"

void func(int i) {
    cout << "Invocation number " << i << '\n';
    Graph g;
}

class Singleton
{
    private: 
        Singleton() {}
    public:
        // static Singleton& getInstance(void) {
        //     static Singleton instance = Singleton();
        //     return instance;
        // }
        static void do_something(void) {
            cout << "This is a generic action of the Singleton class";
        }
};


int main()
{
    for (int i = 0; i < 10; i++) func(i);
    // auto singletonInstance = Singleton::getInstance();
    Singleton::do_something();
}