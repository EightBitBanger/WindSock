
#include "windsock.h"


int main() {
    
    WindSock wSock;
    
    wSock.setAddress("127.0.0.1");
    wSock.setPort(80);
    
    //wSock.startServerListener();
    
    wSock.connectToServer();
    
    
    
    
    return 1;
}


















