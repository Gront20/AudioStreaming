#include <QSlider>
#include <QPainter>

class CustomSlider : public QSlider {
public:
    CustomSlider(QWidget *parent = nullptr)
        : QSlider(parent) {

        setRange(0, 100);
        setSingleStep(1);
        setTickInterval(50);
        setTickPosition(QSlider::TicksBelow);
    }

    void paintEvent(QPaintEvent *event) override {
        QSlider::paintEvent(event);
        QPainter painter(this);
        painter.setPen(QPen(Qt::yellow));

        int numTicks = (maximum() - minimum()) / tickInterval();

        QFontMetrics fm(font());

        if (orientation() == Qt::Horizontal) {
            for (int i = 0; i <= numTicks; i += 1) {
                int tickValue = minimum() + (tickInterval() * i);
                QString text = QString::number(tickValue);
                int tickX = i;
                if (i == 2) tickX = 100 - 34;
                if (i == 1) tickX = 50 - 17;
                int tickY = height() - fm.height() + 10;

                painter.drawText(tickX, tickY, text);
            }
        }

        QFont font = this->font();
        font.setPointSize(7);
        this->setFont(font);

    }
};
