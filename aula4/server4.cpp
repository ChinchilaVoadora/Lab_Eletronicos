#include "../projeto.hpp"

#include <wiringPi.h>
#include <softPwm.h>

int main(void) {
    SERVER server;
    
    server.waitConnection();
    
    Mat_<COR> img(240, 320);

    wiringPiSetup();

	if (softPwmCreate(0, 0, 100)) exit(1);
	if (softPwmCreate(1, 0, 100)) exit(1);
	if (softPwmCreate(2, 0, 100)) exit(1);
	if (softPwmCreate(3, 0, 100)) exit(1);
    
    VideoCapture w(0);
	if (!w.isOpened()) { 
		fprintf(stderr,"Erro: Abertura de webcam 0.");
    	exit(1);
	}
	
	w.set(CAP_PROP_FRAME_WIDTH,320);
	w.set(CAP_PROP_FRAME_HEIGHT,240);
	w.set(CAP_PROP_FPS, 15);
	
	//namedWindow("janela");
	while (true)  {
		w >> img;  //  get  a  new  frame  from  camera
		//imshow("janela",img);
        server.sendImgComp(img);

        uint32_t state;
        server.receiveUint(state);
        int strong = 90;
        float left_wheel_factor = 1;
        float right_wheel_factor = 1;
        switch (state) {
            case 0:
                softPwmWrite(0, 0);  softPwmWrite(1, 0);     
                softPwmWrite(2, 0); softPwmWrite(3,  0);
                break;
            case 1:
                softPwmWrite(0, 0);  softPwmWrite(1, strong*left_wheel_factor);     
                softPwmWrite(2, strong/2*right_wheel_factor); softPwmWrite(3,  0);
                break;
            case 2:
                softPwmWrite(0, 0);  softPwmWrite(1, strong*left_wheel_factor);     
                softPwmWrite(2, strong*right_wheel_factor); softPwmWrite(3,  0);
                break;
            case 3:
                softPwmWrite(0, 0);  softPwmWrite(1, strong/2*left_wheel_factor);     
                softPwmWrite(2, strong*right_wheel_factor); softPwmWrite(3,  0);
                break;
            case 4:
                softPwmWrite(0, 0);  softPwmWrite(1, strong*left_wheel_factor);     
                softPwmWrite(2, 0); softPwmWrite(3,  strong*right_wheel_factor);
                break;
            case 5:
                softPwmWrite(0, strong);  softPwmWrite(1, strong);     
                softPwmWrite(2, strong); softPwmWrite(3,  strong);
                break;
            case 6:
                softPwmWrite(0, strong*left_wheel_factor); softPwmWrite(1, 0);     
                softPwmWrite(2, strong*right_wheel_factor);  softPwmWrite(3, 0);
                break;
            case 7:
                softPwmWrite(0, strong*left_wheel_factor);  softPwmWrite(1, 0);     
                softPwmWrite(2, 0); softPwmWrite(3,  strong/2*right_wheel_factor);
                break;
            case 8:
                softPwmWrite(0, strong*left_wheel_factor); softPwmWrite(1, 0);     
                softPwmWrite(2, 0);  softPwmWrite(3, strong*right_wheel_factor);
                break;
            case 9:
                softPwmWrite(0, strong/2*left_wheel_factor);  softPwmWrite(1, 0);     
                softPwmWrite(2, 0); softPwmWrite(3,  strong*right_wheel_factor);
                break;
            default:
                softPwmWrite(0, strong);  softPwmWrite(1, strong);     
                softPwmWrite(2, strong); softPwmWrite(3,  strong);
                break;
        }

        if (state == 1000)
            break;
	}
    
    softPwmStop(0);
	softPwmStop(1);
	softPwmStop(2);
	softPwmStop(3);
}