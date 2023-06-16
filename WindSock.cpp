
#include "windsock.h"


int main() {
    
    WindSock wSock;
    
    wSock.setAddress("127.0.1.1");
    wSock.setPort(27000);
    
    wSock.startListen();
    
    
    
    //initiateServer();
    //initiateClient();
    
    std::cout << "complete" << std::endl << std::endl;
    std::string test;
    std::cin >> test;
    
    return 1;
}


















