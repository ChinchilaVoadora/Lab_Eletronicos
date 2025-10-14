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

	w.set(CAP_PROP_FRAME_WIDTH,160);
	w.set(CAP_PROP_FRAME_HEIGHT,120);
	w.set(CAP_PROP_FPS, 20);
	Mat_<Vec3b> a;
	namedWindow("janela");

	VideoWriter vo("experimento1.avi",CV_FOURCC('X','V','I','D'),
 					15,Size(160,120)); 

	if (!vo.isOpened()) {
		fprintf(stderr,"Erro: Nao foi possivel abrir arquivo de saída.\n");
		exit(1);
	}

	while (true)  {
		w >> a;  //  get  a  new  frame  from  camera
		imshow("janela",a);
		
		vo << a;  // grava o frame no vídeo
		
		
		int ch=(signed char)(waitKey(30)); // E necessario (signed char)
		if (ch>=0)  break;
	}
}
