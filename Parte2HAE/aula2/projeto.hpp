#include "raspberry.hpp"

#include "server.hpp"
#include "client.hpp"
//(... outros includes ...)
//(... outras classes como SERVER e CLIENT e funções ...)
typedef uint8_t BYTE;



bool testaBytes(BYTE* buf, BYTE b, int n) {
//Testa se n bytes da memoria buf possuem valor b
    bool igual=true;

    for (unsigned i=0; i<n; i++)
        if (buf[i]!=b) { igual=false; break; }
        
    return igual;
}