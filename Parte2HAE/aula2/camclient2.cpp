//client8.cpp
//testa sendVb e receiveVb
#include "projeto.hpp"

int main(int argc, char *argv[]) {
    if (argc!=2) erro("client6 servidorIpAddr\n");

    CLIENT client(argv[1]);

    Mat_<COR> img(480, 640);

	namedWindow("janela2");
    while (true) {
        client.receiveImgComp(img);
            
        imshow("janela2", img);
        
        int ch=(signed char)(waitKey(30)); // E necessario (signed char)

		if (ch>=0)  
            break;

        uint32_t m = 0;
        //if (img.total() != 76800)
        //    m = 1;

        client.sendUint(m);
    }

    uint32_t m = 1;
    client.sendUint(m);
}