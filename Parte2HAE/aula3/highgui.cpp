// highgui.cpp
// computador$ compila highgui -ocv -v3
#include "../projeto.hpp"

int linhas = 3;
int colunas = 3;

int height = 480;
int width = 640;

int estado=0;
void on_mouse(int event, int c, int l, int flags, void* userdata) {
    if (event==EVENT_LBUTTONDOWN) {
        for(int i = 0; i < linhas; ++i) {
            for (int j = 0; j < colunas; ++j) {
                if ((l < (i+1)*(height/linhas)) && (l > (i)*(height/linhas))) {
                    if ((c < (j+1)*(width/colunas)) && (c > (j)*(width/colunas))) {
                        estado = 3*i + j + 1;
                    }
                }
            }
        }
    } 
    else if (event==EVENT_LBUTTONUP) {
        estado=0;
    }
}

int main() {
    COR cinza(128,128,128);
    COR vermelho(0,0,255);

    Mat_<COR> imagem(height, width, cinza);

    namedWindow("janela",WINDOW_NORMAL);
    resizeWindow("janela",2*imagem.cols,2*imagem.rows);
    setMouseCallback("janela", on_mouse);
    imshow("janela",imagem);

    while ((waitKey(1)&0xff)!=27) { // ESC=27 sai do programa
        imagem.setTo(cinza);

        imshow("janela",imagem);
    }
}