//procimagem.h 2024
#include <opencv2/opencv.hpp>
#include <iostream>
#include <float.h>
#include <chrono>
#include <thread>
using namespace std;
using namespace cv;


Mat_<float> filtro2d(Mat_<float> ent, Mat_<float> ker, int borderType=BORDER_DEFAULT)
{ Mat_<float> sai;
  filter2D(ent,sai,-1,ker,Point(-1,-1),0.0,borderType);
  return sai;
}

Mat_<Vec3f> filtro2d(Mat_<Vec3f> ent, Mat_<float> ker, int borderType=BORDER_DEFAULT)
{ Mat_<Vec3f> sai;
  filter2D(ent,sai,-1,ker,Point(-1,-1),0.0,borderType);
  return sai;
}

template<typename T> class ImgXyb: public Mat_<T> {
public:
  using Mat_<T>::Mat_; //inherit constructors

  T backg;
  int lc=0, cc=0;
  int minx=0, maxx=this->cols-1, miny=1-this->rows, maxy=0;

  void centro(int _lc, int _cc) {
    lc=_lc; cc=_cc;
    minx=-cc; maxx=this->cols-cc-1;
    miny=-(this->rows-lc-1); maxy=lc;
  }

  T& operator()(int x, int y) { // modo XY centralizado
    unsigned li=lc-y; unsigned ci=cc+x;
    if (li<unsigned(this->rows) && ci<unsigned(this->cols))
      return (*this).Mat_<T>::operator()(li,ci);
    else return backg;
  }
};

void le(Mat_<float>& a, string nome) {
  int nl,nc,e;
  FILE *arq=fopen(nome.c_str(),"r");
  if (arq==NULL) { erro("Erro abertura arquivo"); }
  e=fscanf(arq,"%d%d",&nl,&nc);
  if (e!=2) { erro("Erro leitura nl nc"); }
  a.create(nl,nc);
  for (int l=0; l<nl; l++)
    for (int c=0; c<nc; c++)
      e=fscanf(arq,"%f",&a(l,c));
      if (e!=1)  { erro("Erro leitura valor"); }
  fclose(arq);
}

void imp(Mat_<float> a, string nome) {
  FILE *arq=fopen(nome.c_str(),"w");
  if (arq==NULL) { erro("Erro abertura arquivo"); }
  fprintf(arq,"%d %d\n",a.rows,a.cols);
  for (int l=0; l<a.rows; l++) {
    for (int c=0; c<a.cols; c++)
      fprintf(arq,"%f ",a(l,c));
    fprintf(arq,"\n");
  }
  fclose(arq);
}

//<<<<<<<<<<<<<<<<<< Funcoes da aula classificacao <<<<<<<<<<<<<<<<<<<<<<
// Acha o argumento (indice) do maior valor
template<class T>
int argMax(Mat_<T> a) {
  T maximo=a(0); int indmax=0;
  for (int i=1; i<a.total(); i++)
    if (a(i)>maximo) {
      maximo=a(i); indmax=i;
    }
  return indmax;
}

double tempo() {
  return chrono::duration_cast<chrono::duration<double>>
     (chrono::system_clock::now().time_since_epoch()).count();
}

#define xdebug { string st = "File="+string(__FILE__)+" line="+to_string(__LINE__)+"\n"; cout << st; }

