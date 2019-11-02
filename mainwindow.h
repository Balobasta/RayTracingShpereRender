#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

namespace Ui {
class MainWindow;
}

struct Vec3{
    double x;
    double y;
    double z;
    Vec3 operator + (Vec3 vec){
        return Vec3{this->x + vec.x, this->y + vec.y, this->z + vec.z};
    }
    Vec3 operator - (Vec3 vec){
        return Vec3{this->x - vec.x, this->y - vec.y, this->z - vec.z};
    }
    double operator * (Vec3 vec){
        return (this->x*vec.x + this->y*vec.y + this->z*vec.z);
    }
    double length(){
        return sqrt((*this) * (*this));
    }
    double cosA(Vec3 vec){
        return ((*this) * vec)/((*this).length()*vec.length());
    }
    void normalize(){
        double len = (*this).length();
        x /= len;
        y /= len;
        z /= len;
    }
};

struct Light{
    Vec3 pos;
    double intensivity;
};

struct Sphere{
    Vec3 center;
    double radius;
    QColor color;
    // задать кф диффузного зеркального отражений, кф преломления и тд
    double kd;
    double kr;
    bool rayIntersect(Vec3 dir, Vec3 point, Vec3 &p, bool camera_ray){
        Vec3 PC = point - center;
        double A = dir * dir;
        double B = 2*(PC * dir);
        double C = (PC * PC) - radius*radius;
        double D = B*B - 4*A*C;
        if (D < 0.0){
            p = Vec3{INFINITY, INFINITY, INFINITY};
            return false;
        }
        if (D == 0.0){
            double t = (-B)/(2*A);
            if ((camera_ray && (t < 1)) || ((!camera_ray) && (t < 0.0))){
                p = Vec3{INFINITY, INFINITY, INFINITY};
                return false;
            }
            p = Vec3{point.x + t*dir.x, point.y + t*dir.y, point.z + t*dir.z};
            return true;
        }
        double t1 = (-B + sqrt(D))/(2*A);
        if ((camera_ray && (t1 < 1)) || ((!camera_ray) && (t1 < 0.0))){
            p = Vec3{INFINITY, INFINITY, INFINITY};
            return false;
        }
        double t2 = (-B - sqrt(D))/(2*A);
        if ((camera_ray && (t2 < 1)) || ((!camera_ray) && (t2 < 0.0))){
            p = Vec3{INFINITY, INFINITY, INFINITY};
            return false;
        }
        Vec3 p1 = Vec3{point.x + t1*dir.x, point.y + t1*dir.y, point.z + t1*dir.z};
        Vec3 p2 = Vec3{point.x + t2*dir.x, point.y + t2*dir.y, point.z + t2*dir.z};
        if (p1.length() < p2.length()){
            p = p1;
        }else p = p2;
        return true;
    }
    Vec3 normal(Vec3 point){
        return (point - center);
    }
} ;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *event);
    void toBaseSystem(double &x, double &y);
    void fromCanvasToBase(double &x, double &y, double &z);
    // написать рекрсивную функцию отражения лучей для разных типов отражения//
    // задать перечисление типов отражения
    bool cast_Ray(Vec3 dir, Vec3 pos, Vec3 &hit, Sphere &s, bool cameraRay);
    QColor recCast(Vec3 pos, Sphere currSphere, int numOfRays, int deep);
private:
    Ui::MainWindow *ui;
    Vec3 pos = Vec3{0,0,0};
    Sphere s = Sphere{Vec3{0, 1, 6}, 1, Qt::white, 1, 0};
    Sphere s1 = Sphere{Vec3{0, -2, 6}, 1, Qt::red, 1, 0};
    Sphere s2 = Sphere{Vec3{2, 1, 4}, 1, Qt::blue, 1, 0};
    QList<Sphere> sL;
    Light l = Light{Vec3{-5, 5, 0}, 1};
    Light l1 = Light{Vec3{3, 5, 0}, 1};
    QList<Light> lL;
    double Vh = 1.0, Vw = 1.0, d = 1.0;
    int numRays = 20;
    int deep = 2;
    bool first = false;
};

#endif // MAINWINDOW_H
