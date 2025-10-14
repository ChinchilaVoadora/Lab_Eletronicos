//raspcam.cpp
//compila raspcam_ocv -ocv (usando "compila" do Cekeikon) OU 
//g++ raspcam_ocv.cpp -o raspcam_ocv `pkg-config opencv --libs --cflags` -O3 -s
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main() {
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
int ch=(signed char)(waitKey(30)); // E necessario (signed char)
if (ch>=0)  break;
}
}