#include <iostream>
#include <spot/misc/version.hh>

using namespace std;

int main() {
    cout << "Spot version " << spot::version() << endl;

    return 0;
}