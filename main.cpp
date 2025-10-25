#include <QtWidgets>

template<typename T>
static T ограничить(T v, T lo, T hi, bool* было_ограничение = nullptr) {
    T vv = std::max(lo, std::min(hi, v));
    if (было_ограничение && vv != v) *было_ограничение = true;
    return vv;
}

static bool почтиРавно(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

struct RGB { int r=255, g=255, b=255; };
struct HSV { int h=0, s=0, v=100; };
struct CMYK { int c=0, m=0, y=0, k=0; };

static HSV rgb_в_hsv(const RGB& rgb) {
    double r = rgb.r / 255.0;
    double g = rgb.g / 255.0;
    double b = rgb.b / 255.0;
    double mx = std::max({r,g,b});
    double mn = std::min({r,g,b});
    double d  = mx - mn;
    double h = 0.0;
    if (!почтиРавно(d, 0.0)) {
        if (почтиРавно(mx, r)) h = 60.0 * std::fmod(((g - b) / d), 6.0);
        else if (почтиРавно(mx, g)) h = 60.0 * (((b - r) / d) + 2.0);
        else h = 60.0 * (((r - g) / d) + 4.0);
    }
    if (h < 0.0) h += 360.0;
    double s = почтиРавно(mx, 0.0) ? 0.0 : (d / mx);
    double v = mx;
    HSV out;
    out.h = static_cast<int>(std::round(h));
    if (out.h == 360) out.h = 0;
    out.s = static_cast<int>(std::round(s * 100.0));
    out.v = static_cast<int>(std::round(v * 100.0));
    return out;
}

static RGB hsv_в_rgb(const HSV& hsv) {
    double H = ограничить(hsv.h, 0, 359) / 60.0;
    double S = ограничить(hsv.s, 0, 100) / 100.0;
    double V = ограничить(hsv.v, 0, 100) / 100.0;
    double C = V * S;
    double X = C * (1 - std::fabs(std::fmod(H, 2.0) - 1));
    double m = V - C;
    double r1=0,g1=0,b1=0;
    int hseg = static_cast<int>(std::floor(H)) % 6;
    switch (hseg) {
        case 0: r1=C; g1=X; b1=0; break;
        case 1: r1=X; g1=C; b1=0; break;
        case 2: r1=0; g1=C; b1=X; break;
        case 3: r1=0; g1=X; b1=C; break;
        case 4: r1=X; g1=0; b1=C; break;
        case 5: r1=C; g1=0; b1=X; break;
    }
    RGB out;
    out.r = static_cast<int>(std::round((r1 + m) * 255.0));
    out.g = static_cast<int>(std::round((g1 + m) * 255.0));
    out.b = static_cast<int>(std::round((b1 + m) * 255.0));
    out.r = ограничить(out.r, 0, 255);
    out.g = ограничить(out.g, 0, 255);
    out.b = ограничить(out.b, 0, 255);
    return out;
}

static CMYK rgb_в_cmyk(const RGB& rgb) {
    double r = rgb.r / 255.0;
    double g = rgb.g / 255.0;
    double b = rgb.b / 255.0;
    double k = 1.0 - std::max({r,g,b});
    double c=0, m=0, y=0;
    if (почтиРавно(k, 1.0)) { c = m = y = 0.0; }
    else {
        c = (1.0 - r - k) / (1.0 - k);
        m = (1.0 - g - k) / (1.0 - k);
        y = (1.0 - b - k) / (1.0 - k);
    }
    CMYK out;
    out.c = static_cast<int>(std::round(c*100.0));
    out.m = static_cast<int>(std::round(m*100.0));
    out.y = static_cast<int>(std::round(y*100.0));
    out.k = static_cast<int>(std::round(k*100.0));
    out.c = ограничить(out.c, 0, 100);
    out.m = ограничить(out.m, 0, 100);
    out.y = ограничить(out.y, 0, 100);
    out.k = ограничить(out.k, 0, 100);
    return out;
}

static RGB cmyk_в_rgb(const CMYK& cmyk) {
    double c = ограничить(cmyk.c, 0, 100) / 100.0;
    double m = ограничить(cmyk.m, 0, 100) / 100.0;
    double y = ограничить(cmyk.y, 0, 100) / 100.0;
    double k = ограничить(cmyk.k, 0, 100) / 100.0;
    double r = (1.0 - c) * (1.0 - k);
    double g = (1.0 - m) * (1.0 - k);
    double b = (1.0 - y) * (1.0 - k);
    RGB out;
    out.r = static_cast<int>(std::round(r * 255.0));
    out.g = static_cast<int>(std::round(g * 255.0));
    out.b = static_cast<int>(std::round(b * 255.0));
    out.r = ограничить(out.r, 0, 255);
    out.g = ограничить(out.g, 0, 255);
    out.b = ограничить(out.b, 0, 255);
    return out;
}

class VidjetCveta : public QWidget {
    Q_OBJECT
public:
    VidjetCveta(QWidget* parent=nullptr) : QWidget(parent) {
        setWindowTitle("ColorLab7 — CMYK ↔ RGB ↔ HSV");
        создатьИнтерфейс();
        установитьRGB({255,255,255}, true);
    }

private:
    QLabel* уведомление_;
    QLabel* предпросмотр_;
    QLineEdit* hexРедактор_;
    QPushButton* выбратьЦвет_;

    QSpinBox *rSB_, *gSB_, *bSB_;
    QSlider  *rSL_, *gSL_, *bSL_;

    QSpinBox *hSB_, *sSB_, *vSB_;
    QSlider  *hSL_, *sSL_, *vSL_;

    QSpinBox *cSB_, *mSB_, *ySB_, *kSB_;
    QSlider  *cSL_, *mSL_, *ySL_, *kSL_;

    bool обновление_ = false;

    void создатьИнтерфейс() {
        auto mainLay = new QVBoxLayout(this);

        уведомление_ = new QLabel(this);
        уведомление_->setVisible(false);
        уведомление_->setStyleSheet("QLabel { background: #FFF3CD; color: #664D03; border: 1px solid #FFEEBA; padding: 6px; border-radius: 4px; }");
        mainLay->addWidget(уведомление_);

        auto topLay = new QHBoxLayout;
        предпросмотр_ = new QLabel;
        предпросмотр_->setFixedSize(140, 80);
        предпросмотр_->setFrameShape(QFrame::Box);
        предпросмотр_->setLineWidth(1);

        hexРедактор_ = new QLineEdit;
        hexРедактор_->setPlaceholderText("#RRGGBB");
        auto hexBtn = new QPushButton("Применить HEX");
        выбратьЦвет_ = new QPushButton("Палитра…");

        auto topRight = new QVBoxLayout;
        auto hexRow = new QHBoxLayout;
        hexRow->addWidget(new QLabel("Шестнадцатеричный:"));
        hexRow->addWidget(hexРедактор_);
        hexRow->addWidget(hexBtn);
        auto btnRow = new QHBoxLayout;
        btnRow->addWidget(выбратьЦвет_);
        btnRow->addStretch(1);
        topRight->addLayout(hexRow);
        topRight->addLayout(btnRow);

        topLay->addWidget(предпросмотр_);
        topLay->addLayout(topRight, 1);
        mainLay->addLayout(topLay);

        mainLay->addWidget(создатьГруппуRGB());
        mainLay->addWidget(создатьГруппуHSV());
        mainLay->addWidget(создатьГруппуCMYK());

        connect(hexBtn, &QPushButton::clicked, this, &VidjetCveta::применитьHex);
        connect(выбратьЦвет_, &QPushButton::clicked, this, &VidjetCveta::открытьПалитру);
    }

    QWidget* создатьГруппуRGB() {
        auto box = new QGroupBox("RGB  (0..255)");
        auto g = new QGridLayout(box);

        rSB_ = сделатьСпин(0,255); rSL_ = сделатьСлайдер(0,255);
        gSB_ = сделатьСпин(0,255); gSL_ = сделатьСлайдер(0,255);
        bSB_ = сделатьСпин(0,255); bSL_ = сделатьСлайдер(0,255);

        добавитьСтроку(g, 0, "R", rSB_, rSL_);
        добавитьСтроку(g, 1, "G", gSB_, gSL_);
        добавитьСтроку(g, 2, "B", bSB_, bSL_);

        связать(rSB_, rSL_, [this]{ приИзмененииRGB(); });
        связать(gSB_, gSL_, [this]{ приИзмененииRGB(); });
        связать(bSB_, bSL_, [this]{ приИзмененииRGB(); });

        return box;
    }

    QWidget* создатьГруппуHSV() {
        auto box = new QGroupBox("HSV  (H:0..359, S/V:%)");
        auto g = new QGridLayout(box);

        hSB_ = сделатьСпин(0,359); hSL_ = сделатьСлайдер(0,359);
        sSB_ = сделатьСпин(0,100); sSL_ = сделатьСлайдер(0,100);
        vSB_ = сделатьСпин(0,100); vSL_ = сделатьСлайдер(0,100);

        добавитьСтроку(g, 0, "H", hSB_, hSL_);
        добавитьСтроку(g, 1, "S", sSB_, sSL_);
        добавитьСтроку(g, 2, "V", vSB_, vSL_);

        связать(hSB_, hSL_, [this]{ приИзмененииHSV(); });
        связать(sSB_, sSL_, [this]{ приИзмененииHSV(); });
        связать(vSB_, vSL_, [this]{ приИзмененииHSV(); });

        return box;
    }

    QWidget* создатьГруппуCMYK() {
        auto box = new QGroupBox("CMYK  (C/M/Y/K:%)");
        auto g = new QGridLayout(box);

        cSB_ = сделатьСпин(0,100); cSL_ = сделатьСлайдер(0,100);
        mSB_ = сделатьСпин(0,100); mSL_ = сделатьСлайдер(0,100);
        ySB_ = сделатьСпин(0,100); ySL_ = сделатьСлайдер(0,100);
        kSB_ = сделатьСпин(0,100); kSL_ = сделатьСлайдер(0,100);

        добавитьСтроку(g, 0, "C", cSB_, cSL_);
        добавитьСтроку(g, 1, "M", mSB_, mSL_);
        добавитьСтроку(g, 2, "Y", ySB_, ySL_);
        добавитьСтроку(g, 3, "K", kSB_, kSL_);

        связать(cSB_, cSL_, [this]{ приИзмененииCMYK(); });
        связать(mSB_, mSL_, [this]{ приИзмененииCMYK(); });
        связать(ySB_, ySL_, [this]{ приИзмененииCMYK(); });
        связать(kSB_, kSL_, [this]{ приИзмененииCMYK(); });

        return box;
    }

    QSpinBox* сделатьСпин(int lo, int hi) {
        auto sb = new QSpinBox;
        sb->setRange(lo, hi);
        sb->setAccelerated(true);
        return sb;
    }

    QSlider* сделатьСлайдер(int lo, int hi) {
        auto sl = new QSlider(Qt::Horizontal);
        sl->setRange(lo, hi);
        return sl;
    }

    void добавитьСтроку(QGridLayout* g, int row, const QString& name, QSpinBox* sb, QSlider* sl) {
        g->addWidget(new QLabel(name), row, 0);
        g->addWidget(sb, row, 1);
        g->addWidget(sl, row, 2);
    }

    template<typename F>
    void связать(QSpinBox* sb, QSlider* sl, F onChange) {
        connect(sb, qOverload<int>(&QSpinBox::valueChanged), sl, &QSlider::setValue);
        connect(sl, &QSlider::valueChanged, sb, &QSpinBox::setValue);
        connect(sb, qOverload<int>(&QSpinBox::valueChanged), this, onChange);
    }

    void показатьУведомление(const QString& msg) {
        уведомление_->setText(msg);
        уведомление_->setVisible(true);
        QTimer::singleShot(2000, this, [this]{ уведомление_->setVisible(false); });
    }

    void установитьRGB(const RGB& rgb, bool инициализация=false) {
        if (обновление_) return;
        обновление_ = true;

        rSB_->setValue(rgb.r);
        gSB_->setValue(rgb.g);
        bSB_->setValue(rgb.b);

        HSV hsv = rgb_в_hsv(rgb);
        CMYK cmyk = rgb_в_cmyk(rgb);

        hSB_->setValue(hsv.h);
        sSB_->setValue(hsv.s);
        vSB_->setValue(hsv.v);

        cSB_->setValue(cmyk.c);
        mSB_->setValue(cmyk.m);
        ySB_->setValue(cmyk.y);
        kSB_->setValue(cmyk.k);

        QColor qc(rgb.r, rgb.g, rgb.b);
        обновитьПредпросмотр(qc);
        hexРедактор_->setText(qc.name(QColor::HexRgb).toUpper());

        обновление_ = false;
        Q_UNUSED(инициализация);
    }

    void приИзмененииRGB() {
        if (обновление_) return;
        bool было=false;
        RGB rgb;
        rgb.r = ограничить(rSB_->value(), 0, 255, &было);
        rgb.g = ограничить(gSB_->value(), 0, 255, &было);
        rgb.b = ограничить(bSB_->value(), 0, 255, &было);
        установитьRGB(rgb);
        if (было) показатьУведомление("Предупреждение: значения RGB были ограничены.");
    }

    void приИзмененииHSV() {
        if (обновление_) return;
        bool было=false;
        HSV hsv;
        hsv.h = ограничить(hSB_->value(), 0, 359, &было);
        hsv.s = ограничить(sSB_->value(), 0, 100, &было);
        hsv.v = ограничить(vSB_->value(), 0, 100, &было);
        RGB rgb = hsv_в_rgb(hsv);
        установитьRGB(rgb);
        if (было) показатьУведомление("Предупреждение: значения HSV были ограничены.");
    }

    void приИзмененииCMYK() {
        if (обновление_) return;
        bool было=false;
        CMYK cmyk;
        cmyk.c = ограничить(cSB_->value(), 0, 100, &было);
        cmyk.m = ограничить(mSB_->value(), 0, 100, &было);
        cmyk.y = ограничить(ySB_->value(), 0, 100, &было);
        cmyk.k = ограничить(kSB_->value(), 0, 100, &было);
        RGB rgb = cmyk_в_rgb(cmyk);
        установитьRGB(rgb);
        if (было) показатьУведомление("Предупреждение: значения CMYK были ограничены.");
    }

    void обновитьПредпросмотр(const QColor& c) {
        QPixmap pm(предпросмотр_->size());
        pm.fill(c);
        предпросмотр_->setPixmap(pm);
    }

    void применитьHex() {
        QString s = hexРедактор_->text().trimmed();
        if (!s.startsWith('#')) s.prepend('#');
        QColor c(s);
        if (!c.isValid()) {
            QMessageBox::warning(this, "Неверный HEX", "Введите цвет в формате #RRGGBB.");
            return;
        }
        установитьRGB({c.red(), c.green(), c.blue()});
    }

    void открытьПалитру() {
        QColor start(rSB_->value(), gSB_->value(), bSB_->value());
        QColor c = QColorDialog::getColor(start, this, "Выбор цвета", QColorDialog::DontUseNativeDialog);
        if (c.isValid()) установитьRGB({c.red(), c.green(), c.blue()});
    }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    VidjetCveta w;
    w.resize(640, 520);
    w.show();
    return app.exec();
}


#include "main.moc"
