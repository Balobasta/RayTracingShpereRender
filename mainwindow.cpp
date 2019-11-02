#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sL.append(s);
    sL.append(s1);
    sL.append(s2);
    lL.append(l);
    lL.append(l1);
    QTime t;
    srand(static_cast<uint>(t.msecsSinceStartOfDay()));
    ui->centralWidget->setFixedSize(500, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    if ( first) return;
    QPainter painter(this);
    for( double x_ = -this->width()/2; x_ < this->width()/2; x_ += 1.0){
        for( double y_ = -this->width()/2; y_ < this->width()/2; y_ += 1.0){
            double z = d;
            double x = x_;
            double y = y_;
            fromCanvasToBase(x,y,z);
            Vec3 v = Vec3{x, y, z};
            Vec3 dir = v - this->pos;
            Sphere sp;
            Vec3 hit;
            if (!cast_Ray(dir, pos, hit, sp, true)){
                QPen pen(Qt::black,1);
                painter.setPen(pen);
                x = x_;
                y = y_;
                toBaseSystem(x, y);
                painter.drawPoint(static_cast<int>(x),static_cast<int>(y));
                continue;
            }
            QColor c = recCast(hit, sp, numRays, deep);
            QPen pen(c,1);
            painter.setPen(pen);
            x = x_;
            y = y_;
            toBaseSystem(x, y);
            painter.drawPoint(static_cast<int>(x),static_cast<int>(y));
        }
    }
    first = true;
}

void MainWindow::toBaseSystem(double &x, double &y)
{
    x = this->width()/2 + x;
    y = this->height()/2 - y;
}

void MainWindow::fromCanvasToBase(double &x, double &y, double &z)
{
    x *= Vw/this->width();
    y *= Vh/this->height();
    z = d;
}

bool MainWindow::cast_Ray(Vec3 dir, Vec3 pos, Vec3 &hit, Sphere &s, bool cameraRay)
{
    hit = Vec3{INFINITY, INFINITY, INFINITY};
    for ( int i = 0; i < sL.size(); i++){
        Vec3 p;
        if (sL[i].rayIntersect(dir, pos, p, cameraRay) && ((pos - p).length() < (pos - hit).length()) ){
            s = sL[i];
            hit = p;
        }
    }
    if (hit.x == INFINITY || hit.y == INFINITY || hit.z == INFINITY)
        return false;
    return true;
}

QColor MainWindow::recCast(Vec3 pos, Sphere currSphere, int numOfRays, int deep)
{
    double R = 0, G = 0, B = 0;
    for( int i = 0; i < lL.size(); i++){
        Vec3 lightray = lL[i].pos - pos;
        Vec3 tmp;
        Sphere tm;
        if (cast_Ray(lightray, pos, tmp, tm, false) && ((tmp - lL[i].pos).length() < lightray.length())){
            continue;
        }
        Vec3 n = currSphere.normal(pos);
        double cos_a = n.cosA(lightray);
        QColor c(currSphere.color);
        int r_;
        int g_;
        int b_;
        c.getRgb(&r_, &g_, &b_);
        double r = r_ * 1.0;
        double g = g_ * 1.0;
        double b = b_ * 1.0;
        r *= cos_a;
        g *= cos_a;
        b *= cos_a;
        if (r < 0) r = 0;
        if (r > 255) r = 255;
        if (g < 0) g = 0;
        if (g > 255) g = 255;
        if (b < 0) b = 0;
        if (b > 255) b = 255;
        R += r;
        G += g;
        B += b;
        if (R > 255) R = 255;
        if (G > 255) G = 255;
        if (B > 255) B = 255;
    }
    R *= currSphere.kd;
    G *= currSphere.kd;
    B *= currSphere.kd;
    if (deep == 0)
        return QColor(static_cast<int>(R), static_cast<int>(G), static_cast<int>(B));
    double Rr = 0, Gr = 0, Br = 0;
    for ( int i = 0; i < numOfRays; i++){
        Vec3 n = currSphere.normal(pos);
        Vec3 vct = Vec3{rand()*1.0, rand()*1.0, rand()*1.0};
        vct.normalize();
        Vec3 hit;
        Sphere s;
        if (!cast_Ray(vct, pos, hit, s, false))
            continue;
        QColor c = recCast(hit, s, numRays, deep - 1);
        int r_;
        int g_;
        int b_;
        c.getRgb(&r_, &g_, &b_);
        double r = r_ * 1.0;
        double g = g_ * 1.0;
        double b = b_ * 1.0;
        Rr += r;
        Gr += g;
        Br += b;
        if (Rr > 255) Rr = 255;
        if (Gr > 255) Gr = 255;
        if (Br > 255) Br = 255;
        if ((Rr == 255.0) && (Gr == 255.0) && (Br == 255.0)) break;
    }
    R += (Rr * currSphere.kr);
    G += (Gr * currSphere.kr);
    B += (Br * currSphere.kr);
    if (R > 255) R = 255;
    if (G > 255) G = 255;
    if (B > 255) B = 255;
    return QColor(static_cast<int>(R), static_cast<int>(G), static_cast<int>(B));
}
