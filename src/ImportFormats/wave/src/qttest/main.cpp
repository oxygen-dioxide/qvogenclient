#include <QDataStream>
#include <QDebug>
#include <QFile>

int main() {
    QFile file("temp.txt");
    QDataStream in(&file);
    QDataStream out(&file);

    if (!file.open(QIODevice::ReadWrite)) {
        return -1;
    }

    out.writeRawData("123", 3);

    char buffer[3];
    in.readRawData(buffer, 2);

    QByteArray bytes(buffer, 2);

    qDebug() << bytes.toHex();

    return 0;
}
