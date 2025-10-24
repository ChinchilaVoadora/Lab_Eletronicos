//client8.cpp
//testa sendVb e receiveVb
#include "projeto.hpp"

int main(int argc, char *argv[]) {
    if (argc!=2) erro("client6 servidorIpAddr\n");

    CLIENT client(argv[1]);

    Mat_<COR> img;


    while (true) {
        client.receiveImg(img);
        imshow("janela", img);

        uint32_t m = 0;
        if (img.total() != 76800)
            m = 1;

        client.sendUint(m);

        int ch=(signed char)(waitKey(30)); // E necessario (signed char)
		if (ch>=0)  
            break;
    }
}