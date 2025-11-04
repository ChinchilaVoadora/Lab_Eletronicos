//client8.cpp
//testa sendVb e receiveVb
#include "../projeto.hpp"
#include <vector>

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
    if (argc!=4) erro("client6 servidorIpAddr\n");

    CLIENT client(argv[1]);

    Mat_<COR> controle(height, width);
    COR cinza(128,128,128);
    COR vermelho(0,0,255);

    Mat_<COR> camera(height, width);

    Mat_<COR> janela(height, width*2);

    Mat_<COR> template_cor = imread("quadrado.png", 1);
    Mat_<FLT> T_cc, T_ncc;

    converte(template_cor, T_cc);
    converte(template_cor, T_ncc);

    T_cc = somaAbsDois(dcReject(T_cc));
    T_ncc = somaAbsDois(dcReject(T_ncc, 128.0/255.0));


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

    while ((waitKey(1)&0xff)!=27) { // ESC=27 sai do programa
        t1 = timeSinceEpoch();
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
        //camera = imread("uou.jpg", 1);
        //resize(camera, camera, Size(width, height), 1, 1, INTER_NEAREST);
        if (camera.empty()) {
            std::cerr << "Erro: imagem não carregada!" << std::endl;
            return -1;
        }

        Mat_<FLT> img_flt;
        converte(camera, img_flt);

        float treshold_ncc = 0.001;
        float treshold_cc = 0.001;

        int location_size = 10; // Nao procurar por outros picos com menos dessa distancia de um pico

        int store = 20;

        std::vector<Point> points_ncc;
        std::vector<float> values_ncc;
        std::vector<int> scales_ncc;

        points_ncc.resize(store);
        values_ncc.resize(store);
        scales_ncc.resize(store);

        std::vector<Point> points_cc;
        std::vector<float> values_cc;
        std::vector<int> scales_cc;

        points_cc.resize(store);
        values_cc.resize(store);
        scales_cc.resize(store);

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

                if (maxVal_cc > treshold_cc) {
                    for (int k = 0; k < store; ++k) {
                        //std::cout << "CC k: " << k << " size: " << values_cc.size() << endl;
                        #pragma omp critical
                        if (maxVal_cc > values_cc[k]) {
                            /*std::cout << "Escala " << scale << " Achou maior posiçao em " << 
                            maxLoc_cc.x << " " << maxLoc_cc.y << " sendo a " << k << " posicao" <<endl;
                            std::cout << "Achou " << maxVal_cc << " antes era " << values_cc[k] << endl << endl;
                            */
                            values_cc.pop_back();
                            values_cc.insert(values_cc.begin() + k, maxVal_cc);

                            points_cc.pop_back();
                            points_cc.insert(points_cc.begin() + k, maxLoc_cc);

                            scales_cc.pop_back();
                            scales_cc.insert(scales_cc.begin() + k, this_scale);

                            circle(R_cc, maxLoc_cc, 1, Scalar(-1), location_size*5);
                            /*for (int i = maxLoc_cc.x - location_size; i < maxLoc_cc.x + location_size; ++i)
                                for (int j = maxLoc_cc.y - location_size; j < maxLoc_cc.y + location_size; ++i)
                                    if (i > 0 && i < height && j > 0 && j < w)    
                                        R_cc[j][i] = 0;
                            */
                            //imshow("janela", R_cc);
                            //waitKey(0);
                            //circle(camera, maxLoc_cc, 10, Scalar(255, 0, 0), location_size);

                            continue_searching = true;
                            k = store + 1;
                        } 
                    }
                }
            }
            
            
            bool continue_searching2 = true;
            while (continue_searching2) {
                minMaxLoc(R_ncc, &minVal_ncc, &maxVal_ncc, &minLoc_ncc, &maxLoc_ncc);
                treshold_ncc == maxVal_ncc * 0.95;
                continue_searching2 = false;
                
                if (maxVal_ncc >= treshold_ncc) {
                    for (int k = 0; k < store; ++k) {
                        //std::cout << "NCC k: " << k << " size: " << values_ncc.size() << endl;
                        #pragma omp critical
                        if (maxVal_ncc > values_ncc[k]) {
                            values_ncc.pop_back();
                            values_ncc.insert(values_ncc.begin() + k, maxVal_ncc);

                            points_ncc.pop_back();
                            points_ncc.insert(points_ncc.begin() + k, maxLoc_ncc);

                            scales_ncc.pop_back();
                            scales_ncc.insert(scales_ncc.begin() + k, this_scale);

                            circle(R_ncc, maxLoc_ncc, 1, Scalar(-1), location_size*5);
                            //circle(camera, maxLoc_ncc, 10, Scalar(0, 255, 0), location_size);

                            continue_searching2 = true;
                            k = store + 1;
                        } 
                    }
                }
            }
            /*imshow("janela", img_flt/3+R_cc*50);
            waitKey(0);
            imshow("janela", img_flt/3+R_ncc*2);
            waitKey(0);*/
        }

        //std::cout << values_cc[0] << " " << values_cc[1] << " " << values_cc[2] << endl;
        //std::cout << points_cc[0] << " " << points_cc[1] << " " << points_cc[2] << endl;
        //std::cout << scales_cc[0] << " " << scales_cc[1] << " " << scales_cc[2] << endl;

        for (int n = 0; n < store; ++n) {
            for (int m = 0; m < store; ++m) {
                if (points_cc[n].x - location_size <= points_ncc[m].x && 
                    points_cc[n].x + location_size >= points_ncc[m].x && scales_ncc[m] == scales_cc[n])
                    if (points_cc[n].y - location_size <= points_ncc[m].y && points_cc[n].y + location_size >= points_ncc[m].y) {
                        Point center = Point((points_cc[n].x + points_ncc[m].x)/2, (points_cc[n].y + points_ncc[m].y)/2);
                        rectangle(camera, Point(center.x - scales_cc[n]/2, center.y - scales_cc[n]/2), 
                                    Point(center.x + scales_cc[n]/2, center.y + scales_cc[n]/2), 
                                    Scalar(255, 0, 255), 3, 3);
                        n = store+1;
                        m = store+1;
                    }

            }
        }

        /*for (int i = 0; i < store; i++) {
            Point center = points_cc[i];
            rectangle(camera, Point(center.x - scales_cc[i]/2, center.y - scales_cc[i]/2), 
                                    Point(center.x + scales_cc[i]/2, center.y + scales_cc[i]/2), 
                                    Scalar(0, 0, 255), 1, 1);

            center = points_ncc[i];
            rectangle(camera, Point(center.x - scales_ncc[i]/2, center.y - scales_ncc[i]/2), 
                                    Point(center.x + scales_ncc[i]/2, center.y + scales_ncc[i]/2), 
                                    Scalar(0, 255, 0), 1, 1);
        }*/

        uint32_t m = estado;
        client.sendUint(m);

        hconcat(controle, camera, janela);

        if (argv[3][0] == 'c')
            vo << camera;
        else if (argv[3][0] == 't') 
            vo << janela;


        imshow("janela", janela);
        t2 = timeSinceEpoch();
        std::cout << "FPS: " << 1/(t2-t1) << endl;
    }

    // Parar
    uint32_t m = 1000;
    client.sendUint(m);
}