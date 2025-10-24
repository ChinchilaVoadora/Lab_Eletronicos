//server8.cpp
//testa sendVb e receiveVb
#include "projeto.hpp"

int main(void) {
    SERVER server;
    
    server.waitConnection();
    
    Mat_<COR> img(480, 640);
    
    VideoCapture w(0);
	if (!w.isOpened()) { 
		fprintf(stderr,"Erro: Abertura de webcam 0.");
    	exit(1);
	}
	
	w.set(CAP_PROP_FRAME_WIDTH,320);
	w.set(CAP_PROP_FRAME_HEIGHT,240);
	w.set(CAP_PROP_FPS, 15);
	
	Mat_<Vec3b> a;
	namedWindow("janela");
	while (true)  {
		w >> a;  //  get  a  new  frame  from  camera
		imshow("janela",a);
        server.sendImg(a);
		int ch=(signed char)(waitKey(30)); // E necessario (signed char)

		if (ch>=0)  
            break;

        uint32_t error;
        server.receiveUint(error);

        if (error != 0)
            break;
	}
}