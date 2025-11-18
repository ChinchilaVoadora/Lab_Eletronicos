//client8.cpp
//testa sendVb e receiveVb
#include "../projeto.hpp"
#include "../procimagem.hpp"
#include <vector>
/*
#include <tensorflow/cc/client/client_session.h>
#include <tensorflow/cc/saved_model/loader.h>
#include <tensorflow/core/framework/tensor.h>
#include <tensorflow/core/platform/env.h>
using namespace tensorflow;*/

#include <opencv2/dnn/dnn.hpp>

using namespace ml;

#define mnist_size 14

int linhas = 3;
int colunas = 3;

int height = 240;
int width = 320;

int estado=0;
bool changeAuto=false;
int autoMode=0; // 0 - Manual; 1 - Automático
bool changeLocked=false;
bool locked=false;  // Proibe o carro de se locomover

bool infoMode=0;
bool changeInfo=false;

COR verde(50,200,50);
COR vermelho(50,50,250);
COR cinza(128, 128, 128);
COR preto(0, 0, 0);
COR branco(255, 255, 255);
COR laranja(0, 100, 255);

Mat_<FLT> croppedNumber(mnist_size, mnist_size);

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
        if (l > height) {
            if (c < width/3) {
                changeAuto=true;
            }
            else if (c > 280 && c < 300) {
                changeLocked=true;
            }
            else if (c > 120 && c < 215)
                changeInfo=true;
        }
    } 
    else if (event==EVENT_LBUTTONUP) {
        estado=0;
    }
}

void drawController(Mat_<COR>& controle) {
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
    if (locked) {
        line(controle, Point(10, 10), Point(width-10, height-10), Scalar(0, 0, 255), 5);
        line(controle, Point(width-10, 10), Point(10, height-10), Scalar(0, 0, 255), 5);
    }
}

void drawConfig(Mat_<COR>& config) {
    config.setTo(preto);
    putText(config, "AUTO", Point(5, 25), 1, 1, branco);
    if (autoMode == 0) {
        rectangle(config, Point(65, 10), Point(100, 30), vermelho, -1);
        circle(config, Point(100, 20), 10, vermelho, -1);
        circle(config, Point(65, 20), 10, cinza, -1);
    }
    else if (autoMode == 1) {
        circle(config, Point(65, 20), 10, verde, -1);
        rectangle(config, Point(65, 10), Point(100, 30), verde, -1);
        circle(config, Point(100, 20), 10, cinza, -1);
    }

    if (locked == 0) {
        circle(config, Point(290, 12), 5, cinza);
        rectangle(config, Point(283, 13), Point(297, 27), cinza, -1);
        circle(config, Point(290, 20), 3, preto, -1);
    }
    else if (locked == 1) {
        circle(config, Point(290, 12), 5, laranja);
        rectangle(config, Point(283, 13), Point(297, 27), laranja, -1);
        circle(config, Point(290, 20), 3, preto, -1);
    }


    putText(config, "INFO", Point(123, 25), 1, 1, branco);
    if (infoMode == 0) {
        rectangle(config, Point(175, 10), Point(210, 30), vermelho, -1);
        circle(config, Point(210, 20), 10, vermelho, -1);
        circle(config, Point(175, 20), 10, cinza, -1);
    }
    else if (infoMode == 1) {
        circle(config, Point(175, 20), 10, verde, -1);
        rectangle(config, Point(175, 10), Point(210, 30), verde, -1);
        circle(config, Point(210, 20), 10, cinza, -1);
    }
}

void drawInfo(Mat_<COR>& info, float fps, float m, int label, int indice, MNIST &mnist) {
    info.setTo(preto);

    putText(info, "FPS", Point(0, 25), 1, 1, cinza);
    putText(info, to_string(int(fps)), Point(35, 25), 1, 1, branco);

    putText(info, "LBL", Point(62, 25), 1, 1, cinza);
    if (label == -1) 
        putText(info, " - ", Point(100, 25), 1, 1, branco);
    else
        putText(info, to_string(int(label)), Point(95, 25), 1, 1, branco);

    putText(info, "STT", Point(110, 25), 1, 1, cinza);
    putText(info, to_string(int(m)), Point(145, 25), 1, 1, branco);

    putText(info, "IMG/MATCH", Point(165, 25), 1, 1, cinza);
    Mat_<FLT> match = mnist.ax.row(indice).reshape(1, mnist_size);
    float tresh_match = mean(match)[0];
    float tresh_number = mean(croppedNumber)[0];
    for (int i = 0; i < mnist_size; i++) {
        for (int j = 0; j < mnist_size; j++) {
            if (croppedNumber[i][j] > tresh_number) 
                circle(info, Point(265+j, 12+i), 1, branco);
            else
                circle(info, Point(265+j, 12+i), 1, preto);

            if (match[i][j] > tresh_match) 
                circle(info, Point(285+j, 12+i), 1, branco);
            else
                circle(info, Point(285+j, 12+i), 1, preto);
        }
    }
}

bool turning=false;
int estadoTurning=0;
double stopTime=-1;
void turnLeft(int count) {
    estadoTurning = 4;
    turning = true;
    stopTime = timeSinceEpoch() + 0.4*count;
}

void turnRight(int count) {
    estadoTurning = 6;
    turning = true;
    stopTime = timeSinceEpoch() + 0.4*count;
}

void readNumber(flann::Index &ind, MNIST &mnist, Mat_<COR> &number_cor, Mat_<FLT> &number, float& value, int& predictedLabel, int &indice) {
    if (number_cor.empty()) return;
    resize(number_cor, number_cor, Size(32, 32), INTER_AREA);
    converte(number_cor, number);
    
    number = number - 0.05;
    float cutoff = mean(number)[0];
    for(int i = 0; i < number.rows; ++i) {
        for (int j = 0; j < number.cols; ++j) {
            if (number[i][j] > cutoff)
                number[i][j] = 1;
            else
                number[i][j] = 0;
        }
    }        

    int coord_cols = 1000;
    int coord_rows = 1000;
    for (int i = 0; i < number.cols/2; ++i) {
        for (int j = 0; j < number.rows/2; ++j) {
            if (number[j][i] == 0) {
                if (i < coord_cols)
                    coord_cols = i;
                
                if (j < coord_rows)
                    coord_rows = j;
            }
        }
    }

    int width = 0;
    int height = 0;
    for (int i = number.cols/2; i < number.cols; ++i) {
        for (int j = number.rows/2; j < number.rows; ++j) {
            if (number[j][i] == 0) {
                if ((i - coord_cols) > width)
                    width = (i - coord_cols) + 1;
                
                if ((j - coord_rows) > height)
                    height = (j - coord_rows) + 1;
            }
        }
    }

    if (width != 0 && height != 0 && coord_cols != 1000 && coord_rows != 1000) {
        if (height > width) {
            int new_width = height;
            coord_cols = coord_cols - (height - width) / 2 + 1;
            if (coord_cols < 0) coord_cols = 0;
            if (coord_rows < 0) coord_rows = 0;
            if (new_width >= number.cols - coord_cols) new_width = number.cols - coord_cols;
            if (height >= number.rows - coord_rows) height = number.rows - coord_rows;

            number = number(Rect(coord_cols, coord_rows, new_width, height));
        }
        else {
            int new_height = width;
            coord_rows = coord_rows - (width - height) / 2 + 1;
            if (coord_cols < 0) coord_cols = 0;
            if (coord_rows < 0) coord_rows = 0;
            if (width > number.cols - coord_cols) width = number.cols - coord_cols;
            if (new_height > number.rows - coord_rows) new_height = number.rows - coord_rows;

            number = number(Rect(coord_cols, coord_rows, width, new_height));    
        }
    }

    resize(number, number, Size(mnist_size, mnist_size));
    croppedNumber = number;
    Mat flat = number.reshape(1, 1);
    std::vector<int> indices(1);      // nearest neighbor index
    std::vector<float> dists(1);      // distance

    ind.knnSearch(flat,indices,dists,1,flann::SearchParams(128));
    
    predictedLabel = mnist.ay(indices[0]);
    value = dists[0];
    indice = indices[0];
}

void findTemplate(Mat_<FLT>& img_flt, Mat_<FLT>& T_cc, Mat_<FLT>& T_ncc, vector<int>& scales, int location_size, int store, 
                    std::vector<Point>& points_cc, std::vector<float>& values_cc, std::vector<int>& scales_cc, std::vector<Point>& points_ncc, std::vector<float>& values_ncc, std::vector<int>& scales_ncc,
                    float treshold_cc, float treshold_ncc) {
    
    points_cc.clear(); values_cc.clear(); scales_cc.clear();
    points_cc.resize(store); values_cc.resize(store); scales_cc.resize(store);
    points_ncc.clear(); values_ncc.clear(); scales_ncc.clear();
    points_ncc.resize(store); values_ncc.resize(store); scales_ncc.resize(store);

    #pragma omp parallel for shared(points_ncc, values_ncc, scales_ncc, points_cc, values_cc, scales_cc)
    for (int i = 0; i < scales.size(); i++) {
        int this_scale = scales[i];
        Mat_<FLT> res_T_cc, res_T_ncc;

        double minVal_ncc, maxVal_ncc;
        Point minLoc_ncc, maxLoc_ncc;

        double minVal_cc, maxVal_cc;
        Point minLoc_cc, maxLoc_cc;

        Mat_<FLT> R_cc, R_ncc;

        resize(T_cc, res_T_cc, Size(this_scale, this_scale), 1, 1, INTER_NEAREST);
        resize(T_ncc, res_T_ncc, Size(this_scale, this_scale), 1, 1, INTER_NEAREST);

        R_cc = matchTemplateSame(img_flt, res_T_cc, CV_TM_CCORR, 0.0);
        R_ncc = matchTemplateSame(img_flt, res_T_ncc, CV_TM_CCOEFF_NORMED, 0.0);

        bool continue_searching = true;
        while (continue_searching) {
            minMaxLoc(R_cc, &minVal_cc, &maxVal_cc, &minLoc_cc, &maxLoc_cc);
            continue_searching = false;
            for (int k = 0; k < store; ++k) {
                #pragma omp critical
                if (maxVal_cc > values_cc[k]) {
                    values_cc.pop_back();
                    values_cc.insert(values_cc.begin() + k, maxVal_cc);

                    points_cc.pop_back();
                    points_cc.insert(points_cc.begin() + k, maxLoc_cc);

                    scales_cc.pop_back();
                    scales_cc.insert(scales_cc.begin() + k, this_scale);

                    circle(R_cc, maxLoc_cc, 1, Scalar(-1), location_size*5);
                    
                    continue_searching = true;
                    k = store + 1;
                } 
            }
        }

        bool continue_searching2 = true;
        while (continue_searching2) {
            minMaxLoc(R_ncc, &minVal_ncc, &maxVal_ncc, &minLoc_ncc, &maxLoc_ncc);
            
            continue_searching2 = false;
            for (int k = 0; k < store; ++k) {
                #pragma omp critical
                if (maxVal_ncc > values_ncc[k]) {
                    values_ncc.pop_back();
                    values_ncc.insert(values_ncc.begin() + k, maxVal_ncc);

                    points_ncc.pop_back();
                    points_ncc.insert(points_ncc.begin() + k, maxLoc_ncc);

                    scales_ncc.pop_back();
                    scales_ncc.insert(scales_ncc.begin() + k, this_scale);

                    circle(R_ncc, maxLoc_ncc, 1, Scalar(-1), location_size*5);

                    continue_searching2 = true;
                    k = store + 1;
                } 
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc!=4) erro("client6 servidorIpAddr\n");

    //CLIENT client(argv[1]);

    Mat_<COR> camera(height, width);
    Mat_<COR> cameraCopy(height, width);
    Mat_<COR> controle(height, width);
    Mat_<COR> config(height/6, width);
    Mat_<COR> info(height/6, width);
    Mat_<COR> janela1(height, width*2);
    Mat_<COR> janela2(height/6, width*2);

    Mat_<COR> janelaFinal(height*7/6, width*2);

    Mat_<COR> template_cor = imread("./../quadrado.png", 1);
    Mat_<FLT> T_cc, T_ncc;

    converte(template_cor, T_cc);
    converte(template_cor, T_ncc);

    T_cc = somaAbsDois(dcReject(T_cc));
    T_ncc = somaAbsDois(dcReject(T_ncc, 128.0/255.0));

    MNIST mnist(mnist_size, true, true);
    mnist.le("/home/rafitcha20/cekeikon5/tiny_dnn/data");
   
    flann::Index ind(mnist.ax,flann::KDTreeIndexParams(4));

    VideoWriter vo;
    if (argv[3][0] == 'c')
        vo = VideoWriter(argv[2], CV_FOURCC('X','V','I','D'), 15, Size(width,height));
    else if (argv[3][0] == 't') 
        vo = VideoWriter(argv[2], CV_FOURCC('X','V','I','D'), 15, Size(2*width,height));

    
	namedWindow("janela");
    //resizeWindow("janela",2*imagem.cols,2*imagem.rows);
    setMouseCallback("janela", on_mouse);

    int scale = 10;
    float factor = 1.2;
    vector<int> scales;
    while (scale <= width/3 && scale <= height/3) {
        scales.insert(scales.begin(), scale);
        scale = scale * factor;
    }

    double t1 = timeSinceEpoch();
    double t2 = timeSinceEpoch();

    float treshold_ncc = 0.5;
    float treshold_cc = 0.1;

    int location_size = 10;
    int store = 20;

    std::vector<Point> points_ncc;
    std::vector<float> values_ncc;
    std::vector<int> scales_ncc;

    std::vector<Point> points_cc;
    std::vector<float> values_cc;
    std::vector<int> scales_cc;

    uint32_t m = 0;

    float value;
    int label;
    int indice = 0;

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
        drawController(controle);
        drawConfig(config);
        drawInfo(info, (1/(t2 - t1)), m, label, indice, mnist);

        t1 = timeSinceEpoch();

        if (changeAuto) {
            changeAuto = false;
            autoMode = !autoMode;
        }
        if (changeLocked) {
            changeLocked = false;
            locked = !locked;
        }
        if (changeInfo) {
            changeInfo = false;
            infoMode = !infoMode;
        }

        //client.receiveImgComp(camera);
        camera = imread("exemplo.png", 1);
        resize(camera, camera, Size(width, height), 1, 1, INTER_NEAREST);
        if (camera.empty()) {
            std::cerr << "Erro: imagem nao carregada!" << std::endl;
            return -1;
        }
        cameraCopy = camera.clone();
        Mat_<FLT> img_flt;
        converte(camera, img_flt);

        findTemplate(img_flt, T_cc, T_ncc, scales, location_size, store, 
            points_cc, values_cc, scales_cc, points_ncc, values_ncc, scales_ncc, treshold_cc, treshold_ncc);

        Point center = Point((0, 0));
        int scale = 500;
        for (int n = 0; n < store; ++n) {
            for (int m = 0; m < store; ++m) {
                if (points_cc[n].x - location_size <= points_ncc[m].x && 
                    points_cc[n].x + location_size >= points_ncc[m].x && scales_ncc[m] == scales_cc[n])
                    if (points_cc[n].y - location_size <= points_ncc[m].y && points_cc[n].y + location_size >= points_ncc[m].y) {
                        scale = scales_cc[n];
                        center = Point((points_cc[n].x + points_ncc[m].x)/2, (points_cc[n].y + points_ncc[m].y)/2);
                        rectangle(cameraCopy, Point(center.x - scales_cc[n]/2, center.y - scales_cc[n]/2), 
                                    Point(center.x + scales_cc[n]/2, center.y + scales_cc[n]/2), 
                                    Scalar(255, 0, 255), 3, 3);
                        n = store+1;
                        m = store+1;
                    }

            }
        }

        if (infoMode) {
            for (int i = 0; i < store; i++) {
                Point center = points_cc[i];
                rectangle(cameraCopy, Point(center.x - scales_cc[i]/2, center.y - scales_cc[i]/2), 
                                        Point(center.x + scales_cc[i]/2, center.y + scales_cc[i]/2), 
                                        Scalar(0, 0, 255), 1, 1);

                center = points_ncc[i];
                rectangle(cameraCopy, Point(center.x - scales_ncc[i]/2, center.y - scales_ncc[i]/2), 
                                        Point(center.x + scales_ncc[i]/2, center.y + scales_ncc[i]/2), 
                                        Scalar(0, 255, 0), 1, 1);
            }
        }

        Mat_<COR> number_cor;
        Mat_<FLT> number;
        if (center.x != 0 && center.y != 0) {
            number_cor = camera(Rect(int(center.x - scale*0.3/2), int(center.y - scale*0.3/2), int(scale*0.35), int(scale*0.35)));
            //number_cor = imread("zero.png");
            
            readNumber(ind, mnist, number_cor, number, value, label, indice);
            //std::cout << "Reconhecido " << label << endl;
        }

        int number_state = -1;
        if (value > 10) {
            number_state = label;
        }
        else {
            label = -1;
        }
        
        
        if (number_state != -1 && autoMode == 1 && turning == false) {
            if (number_state == 0 || number_state == 1) 
                estado = 5;
            else if (number_state == 2)
                turnLeft(2);
            else if (number_state == 3)
                turnRight(2);
            else if (number_state == 4 || number_state == 5)
                estado = 2;
            else if (number_state == 6 || number_state == 7)
                turnLeft(1);
            else if (number_state == 8 || number_state == 9)
                turnRight(1);
        }


        m = estado;
        if (locked) m = 0;
        if (turning) m = estadoTurning;
        //client.sendUint(m);

        hconcat(controle, cameraCopy, janela1);
        hconcat(config, info, janela2);
        vconcat(janela1, janela2, janelaFinal);

        if (argv[3][0] == 'c')
            vo << camera;
        else if (argv[3][0] == 't') 
            vo << janelaFinal;

        if (turning) 
            if (timeSinceEpoch() >= stopTime) {
                turning = false;
            }      

        if (turning)
            std::cout << "virando\n";

        imshow("janela", janelaFinal);
        //waitKey(0);
        t2 = timeSinceEpoch();
    }

    // Parar
    m = 1000;
    //client.sendUint(m);
}