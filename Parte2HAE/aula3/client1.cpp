//client8.cpp
//testa sendVb e receiveVb
#include "../projeto.hpp"

int linhas = 3;
int colunas = 3;

int height = 240;
int width = 320;

int estado=0;
void on_mouse(int event, int c, int l, int flags, void* userdata) {
    if (event==EVENT_LBUTTONDOWN) {
        for(int i = 0; i < linhas; ++i) {
            for (int j = 0; j < colunas; ++j) {
                if ((l < (i+1)*(height/linhas)) && (l > (i)*(height/linhas))) {
                    if ((c < (j+1)*(width/colunas)) && (c > (j)*(width/colunas))) {
                        estado = colunas*i + j + 1;
                    }
                }
            }
        }
    } 
    else if (event==EVENT_LBUTTONUP) {
        estado=0;
    }
}

void drawHud(Mat_<COR>& controle) {
    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            int y_min = i*(height/linhas) + 0.1*(height/linhas);
            int y_max = (i+1)*(height/linhas) - 0.1*(height/linhas);

            int x_min = j*(width/colunas) + 0.3*(width/colunas);
            int x_max = (j+1)*(width/colunas) - 0.3*(width/colunas);

            int estado = colunas*i + j + 1;

            switch (estado) {
                case 1:
                    arrowedLine(controle, Point(x_max, y_max), Point(x_min, y_min), Scalar(255,255,255), 3);
                    break;
                case 2:
                    arrowedLine(controle, Point((x_max - (x_max-x_min)/2), y_max), Point((x_max - (x_max-x_min)/2), y_min), Scalar(255,255,255), 3);
                    break;
                case 3:
                    arrowedLine(controle, Point(x_min, y_max), Point(x_max, y_min), Scalar(255,255,255), 3);
                    break;
                case 4:
                    arrowedLine(controle, Point(x_max, (y_max - (y_max-y_min)/2)), Point(x_min, (y_max - (y_max-y_min)/2)), Scalar(255,255,255), 3);
                    break;
                case 5:
                    circle(controle, Point((x_max - (x_max-x_min)/2), (y_max - (y_max-y_min)/2)), 1, Scalar(255,255,255), 15);
                    break;
                case 6:
                    arrowedLine(controle, Point(x_min, (y_max - (y_max-y_min)/2)), Point(x_max, (y_max - (y_max-y_min)/2)), Scalar(255,255,255), 3);
                    break;
                case 7:
                    arrowedLine(controle, Point(x_max, y_min), Point(x_min, y_max), Scalar(255,255,255), 3);
                    break;
                case 8:
                    arrowedLine(controle, Point((x_max - (x_max-x_min)/2), y_min), Point((x_max - (x_max-x_min)/2), y_max), Scalar(255,255,255), 3);
                    break;
                case 9:
                    arrowedLine(controle, Point(x_min, y_min), Point(x_max, y_max), Scalar(255,255,255), 3);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc!=2) erro("client6 servidorIpAddr\n");

    CLIENT client(argv[1]);

    Mat_<COR> controle(height, width);
    COR cinza(128,128,128);
    COR vermelho(0,0,255);

    Mat_<COR> camera(height, width);

    Mat_<COR> janela(height, width*2);

    VideoWriter vo;
    if (argv[3][0] == 'c')
        vo = VideoWriter(argv[2], CV_FOURCC('X','V','I','D'), 15, Size(width,height));
    else if (argv[3][0] == 't') 
        vo = VideoWriter(argv[2], CV_FOURCC('X','V','I','D'), 15, Size(2*width,height));

	namedWindow("janela");
    //resizeWindow("janela",2*imagem.cols,2*imagem.rows);
    setMouseCallback("janela", on_mouse);

    while ((waitKey(1)&0xff)!=27) { // ESC=27 sai do programa
        controle.setTo(cinza);
        camera.setTo(vermelho);

        for (int i = 1; i <= linhas*colunas; ++i) {
            if (estado == i) {
                int x = (i-1) % 3;
                int y = (i-1) / 3;

                for (int l = y*(height/linhas); l < (y+1)*(height/linhas); ++l)
                for (int c = x*(width/colunas); c < (x+1)*(width/colunas); ++c)
                controle(l, c) = vermelho;
            }
        }
        drawHud(controle);


        client.receiveImgComp(camera);

        uint32_t m = estado;
        client.sendUint(m);


        hconcat(controle, camera, janela);

        if (argv[3][0] == 'c')
            vo << camera;
        else if (argv[3][0] == 't') 
            vo << janela;

        imshow("janela",janela);
    }

    // Parar
    uint32_t m = 1000;
    client.sendUint(m);
}