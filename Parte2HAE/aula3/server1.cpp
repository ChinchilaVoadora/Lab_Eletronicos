//server8.cpp
//testa sendVb e receiveVb
#include "../projeto.hpp"

int main(void) {
    SERVER server;
    
    server.waitConnection();
    
    Mat_<COR> img(480, 640);
    
    VideoCapture w(0);
	if (!w.isOpened()) { 
		fprintf(stderr,"Erro: Abertura de webcam 0.");
    	exit(1);
	}
	
	w.set(CAP_PROP_FRAME_WIDTH,640);
	w.set(CAP_PROP_FRAME_HEIGHT,480);
	w.set(CAP_PROP_FPS, 15);
	
	//namedWindow("janela");
	while (true)  {
		w >> img;  //  get  a  new  frame  from  camera
		//imshow("janela",img);
        server.sendImgComp(img);

        uint32_t error;
        server.receiveUint(error);

        if (error == 1000)
            break;
	}
}