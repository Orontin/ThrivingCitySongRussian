#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QString>
#include <QList>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QList>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

bool replaceLinesFromFile(const QString &sourcePath, const QString &targetPath) {
    // 1. Читаем нужные строки из исходного файла
    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть исходный файл:" << sourcePath;
        return false;
    }

    QTextStream sourceStream(&sourceFile);

    QString line2, line44, line45;
    int lineNumber = 1;
    while (!sourceStream.atEnd()) {
        QString line = sourceStream.readLine();
        if (lineNumber == 2) {
            line2 = line;
        } else if (lineNumber == 44) {
            line44 = line;
        } else if (lineNumber == 45) {
            line45 = line;
        }
        // После 45-й строки можно прекратить чтение (если файл очень большой)
        if (lineNumber >= 45) {
            break;
        }
        ++lineNumber;
    }
    sourceFile.close();

    // 2. Читаем все строки целевого файла в список
    QFile targetFile(targetPath);
    if (!targetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть целевой файл для чтения:" << targetPath;
        return false;
    }

    QTextStream targetStream(&targetFile);
    QStringList lines;
    while (!targetStream.atEnd()) {
        lines.append(targetStream.readLine());
    }
    targetFile.close();

    // 3. Заменяем строки, если они существуют (индексация списка с 0)
    if (lines.size() >= 2) {
        lines[1] = line2;
    } else {
        qWarning() << "Целевой файл не содержит строки 2, замена не выполнена." << sourcePath << targetPath;
    }

    if (lines.size() >= 44) {
        lines[43] = line44;
    } else {
        qWarning() << "Целевой файл не содержит строки 44, замена не выполнена." << sourcePath << targetPath;
    }

    if (lines.size() >= 45) {
        lines[44] = line45;
    } else {
        qWarning() << "Целевой файл не содержит строки 45, замена не выполнена." << sourcePath << targetPath;
    }

    // 4. Записываем изменения обратно в целевой файл
    if (!targetFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Не удалось открыть целевой файл для записи:" << targetPath;
        return false;
    }

    QTextStream writeStream(&targetFile);
    for (int i = 0; i < lines.size(); ++i) {
        writeStream << lines[i];
        if (i != lines.size() - 1) {
            writeStream << '\n';  // добавляем перевод строки, кроме последней строки
        }
    }
    targetFile.close();

    return true;
}

QList<QStringList> parseNestedListFromFile(const QString& filePath)
{
    QList<QStringList> result;

    QFile file(filePath);

    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            QString fileContent = stream.readAll();

            file.close();

            QString content = fileContent.trimmed();
            content = content.replace("[[", "");
            content = content.replace("]]", "");
            QStringList rows = content.split("],[");
            for (int i = 0; i < rows.size(); ++i) {
                QString rowString = rows[i];

                rowString.remove(0, 1);
                rowString.chop(1);
                QStringList elements = rowString.split("\",\"", Qt::KeepEmptyParts);
                result.append(elements);
            }
            file.close();
        }
    }

    return result;
}

struct KeyAndQStringList {
    QString key;
    QStringList list;
    int maximum;
};

QString splitIntoLinesMax50(const QString& input) {
    QStringList result;
    QStringList lines = input.split("\\n");

    for (const QString& line : lines) {
        if (line.isEmpty()) {
            result.append(QString());
            continue;
        }

        QStringList podlines = line.split(' ');

        QString newl;
        for (QString string : podlines) {
            if (QString(newl + " " + string).size() < 70) {
                newl = newl + " " + string;
            } else {
                result.push_back(newl);
                newl = " " + string;
            }
        }

        if (newl.size() > 0) {
            result.push_back(newl);
            newl = "";
        }
    }

    return result.join("\n ");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QList<QStringList> list = parseNestedListFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/config.unity3d/LanguageConfig.txt");

    QString maximumString;
    for (int i = 0; i < list.size(); i++) {
        if (list[i][0].size() > maximumString.size()) {
            maximumString = list[i][0];
        }
    }
    maximumString = maximumString + "          ";

    QList<KeyAndQStringList> listKeyAndQStringList;
    for (int i = 0; i < list.size(); i++) {
        QString string = list[i][0];
        if (!string.contains("Unity")) {
            string = string.remove("0");
            string = string.remove("1");
            string = string.remove("2");
            string = string.remove("3");
            string = string.remove("4");
            string = string.remove("5");
            string = string.remove("6");
            string = string.remove("7");
            string = string.remove("8");
            string = string.remove("9");
            string.chop(1);
        }

        bool isSearched = false;
        for (KeyAndQStringList &obj : listKeyAndQStringList) {
            if (obj.key == string) {
                QStringList lll;
                lll = list[i][1].split("\\n");
                for (QString l : lll) {
                    obj.list.push_back(l);
                }
                lll = list[i][2].split("\\n");
                for (QString l : lll) {
                    obj.list.push_back(l);
                }
                lll = list[i][3].split("\\n");
                for (QString l : lll) {
                    obj.list.push_back(l);
                }
                obj.list.push_back(list[i][1]);
                obj.list.push_back(list[i][2]);
                obj.list.push_back(list[i][3]);
                isSearched = true;
                break;
            }
        }

        if (!isSearched) {
            KeyAndQStringList obj;
            obj.key = string;
            QStringList lll;
            lll = list[i][1].split("\\n");
            for (QString l : lll) {
                obj.list.push_back(l);
            }
            lll = list[i][2].split("\\n");
            for (QString l : lll) {
                obj.list.push_back(l);
            }
            lll = list[i][3].split("\\n");
            for (QString l : lll) {
                obj.list.push_back(l);
            }
            obj.list.push_back(list[i][1]);
            obj.list.push_back(list[i][2]);
            obj.list.push_back(list[i][3]);
            listKeyAndQStringList.push_back(obj);
        }
    }

    QString maximum_size = 0;
    for (int i = 0; i < listKeyAndQStringList.size(); i++) {
        int maximum = 0;
        for (QString lll : listKeyAndQStringList[i].list) {
            if (maximum < lll.size()) {
                maximum = lll.size();
            }
        }
        listKeyAndQStringList[i].maximum = maximum;
        if (maximum_size.size() < maximum) {
            maximum_size = QString::number(maximum);
        }
    }
    maximum_size = maximum_size + "          ";

    QFile fileout1;
    QTextStream out1(&fileout1);
    fileout1.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/CN1.txt"));
    fileout1.open(QIODevice::WriteOnly | QIODevice::Text);
    for (int i = 0; i < list.size(); i++) {
        QString space;
        for (int j = 0; j < maximumString.size() - list[i][0].size(); j++) {
            space = space + " ";
        }

        QString string = list[i][0];
        if (!string.contains("Unity")) {
            string = string.remove("0");
            string = string.remove("1");
            string = string.remove("2");
            string = string.remove("3");
            string = string.remove("4");
            string = string.remove("5");
            string = string.remove("6");
            string = string.remove("7");
            string = string.remove("8");
            string = string.remove("9");
            string.chop(1);
        }

        int maximum = 0;
        for (KeyAndQStringList &obj : listKeyAndQStringList) {
            if (obj.key == string) {
                maximum = obj.maximum;
                break;
            }
        }

        QString space2;
        for (int j = 0; j < maximum_size.size() - QString::number(maximum).size(); j++) {
            space2 = space2 + " ";
        }

        bool isN = false;
        if (list[i][1].contains("\\n") || list[i][2].contains("\\n") || list[i][3].contains("\\n")) {
            isN = true;
        }

        out1 << (isN == true ? "n_truee" : "n_false") << "          " << maximum << space2 << list[i][0] << space << list[i][1] << '\n';
        //out1 << list[i][1] << '\n';
    }
    fileout1.close();

    QFile fileout1_;
    QTextStream out1_(&fileout1_);
    fileout1_.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/CN2.txt"));
    fileout1_.open(QIODevice::WriteOnly | QIODevice::Text);
    for (int i = 0; i < list.size(); i++) {
        QString space;
        for (int j = 0; j < maximumString.size() - list[i][0].size(); j++) {
            space = space + " ";
        }

        QString string = list[i][0];
        if (!string.contains("Unity")) {
            string = string.remove("0");
            string = string.remove("1");
            string = string.remove("2");
            string = string.remove("3");
            string = string.remove("4");
            string = string.remove("5");
            string = string.remove("6");
            string = string.remove("7");
            string = string.remove("8");
            string = string.remove("9");
            string.chop(1);
        }

        int maximum = 0;
        for (KeyAndQStringList &obj : listKeyAndQStringList) {
            if (obj.key == string) {
                maximum = obj.maximum;
                break;
            }
        }

        QString space2;
        for (int j = 0; j < maximum_size.size() - QString::number(maximum).size(); j++) {
            space2 = space2 + " ";
        }

        bool isN = false;
        if (list[i][1].contains("\\n") || list[i][2].contains("\\n") || list[i][3].contains("\\n")) {
            isN = true;
        }

        out1_ << (isN == true ? "n_truee" : "n_false") << "          " << maximum << space2 << list[i][0] << space << list[i][2] << '\n';
        //out1_ << list[i][2] << '\n';
    }
    fileout1_.close();

    QFile fileout1__;
    QTextStream out1__(&fileout1__);
    fileout1__.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/ENG.txt"));
    fileout1__.open(QIODevice::WriteOnly | QIODevice::Text);
    for (int i = 0; i < list.size(); i++) {
        QString space;
        for (int j = 0; j < maximumString.size() - list[i][0].size(); j++) {
            space = space + " ";
        }

        QString string = list[i][0];
        if (!string.contains("Unity")) {
            string = string.remove("0");
            string = string.remove("1");
            string = string.remove("2");
            string = string.remove("3");
            string = string.remove("4");
            string = string.remove("5");
            string = string.remove("6");
            string = string.remove("7");
            string = string.remove("8");
            string = string.remove("9");
            string.chop(1);
        }


        int maximum = 0;
        for (KeyAndQStringList &obj : listKeyAndQStringList) {
            if (obj.key == string) {
                maximum = obj.maximum;
                break;
            }
        }

        QString space2;
        for (int j = 0; j < maximum_size.size() - QString::number(maximum).size(); j++) {
            space2 = space2 + " ";
        }

        bool isN = false;
        if (list[i][1].contains("\\n") || list[i][2].contains("\\n") || list[i][3].contains("\\n")) {
            isN = true;
        }

        out1__ << (isN == true ? "n_truee" : "n_false") << "          " << maximum << space2 << list[i][0] << space << list[i][3] << '\n';
        //out1__ << list[i][3] << '\n';
    }
    fileout1__.close();

    QStringList listRUS;
    QStringList listENG;
    QStringList listCN1;

    QFile fileinRUS;
    fileinRUS.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/RUS.txt"));
    fileinRUS.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inRUS(&fileinRUS);
    int iRUS = 0;
    while (!inRUS.atEnd()) {
        QString row = inRUS.readLine();
        listRUS.push_back(row);
        iRUS++;
    }
    fileinRUS.close();

    QFile fileinENG;
    fileinENG.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/ENG.txt"));
    fileinENG.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inENG(&fileinENG);
    int iENG = 0;
    while (!inENG.atEnd()) {
        QString row = inENG.readLine();
        listENG.push_back(row);
        iENG++;
    }
    fileinENG.close();

    QFile fileinCN1;
    fileinCN1.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/CN1.txt"));
    fileinCN1.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inCN1(&fileinCN1);
    int iCN1 = 0;
    while (!inCN1.atEnd()) {
        QString row = inCN1.readLine();
        listCN1.push_back(row);
        iCN1++;
    }
    fileinCN1.close();

    QFile fileoutSIZE;
    QTextStream outSIZE(&fileoutSIZE);
    fileoutSIZE.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/SIZE.txt"));
    fileoutSIZE.open(QIODevice::WriteOnly | QIODevice::Text);
    for (int i = 0; i < listENG.size(); i++) {
        QString string = list[i][0];
        if (!string.contains("Unity")) {
            string = string.remove("0");
            string = string.remove("1");
            string = string.remove("2");
            string = string.remove("3");
            string = string.remove("4");
            string = string.remove("5");
            string = string.remove("6");
            string = string.remove("7");
            string = string.remove("8");
            string = string.remove("9");
            string.chop(1);
        }

        int maximum = 0;
        for (KeyAndQStringList &obj : listKeyAndQStringList) {
            if (obj.key == string) {
                maximum = obj.maximum;
                break;
            }
        }

        QStringList listRUSto;
        listRUSto = listRUS[i].split("\\n");
        //listRUSto.push_back(listRUS[i]);
        for (int j = 0; j < listRUSto.size(); j++) {
            if (maximum < listRUSto[j].size()) {
                if (!string.contains("Scroll")/* && !string.contains("Unity")*/ /*&& maximum == 8*/) {
                    QString string = QString::number(maximum);
                    QString space = " ";
                    for (int k = 0; k < 4 - string.size(); k++) {
                        space = space + " ";
                    }
                    outSIZE << " line: " << i + 1 << " enter: " << j << " maximum: " << maximum << space << "|" << listRUSto[j] << '\n';
                }
            }
        }
    }
    fileoutSIZE.close();

    QFile filein1;
    filein1.setFileName(QString("../../../resource/Text/LanguageConfig.prefab/RUS.txt"));
    filein1.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in1(&filein1);
    int i = 0;
    while (!in1.atEnd()) {
        QString row = in1.readLine();
        if (i != 0 && i != 1 && i != 2) {
            list[i][1] = row;
            list[i][2] = row;
            list[i][3] = row;
            list[i][4] = list[i][1] + "|" + list[i][2] + "|" + list[i][3];
        }
        i++;
    }
    filein1.close();

    QFile fileout2;
    QTextStream out2(&fileout2);
    fileout2.setFileName(QString("../../../resource/Assets/new/Song_Data/StreamingAssets/config.unity3d/LanguageConfig.txt"));
    fileout2.open(QIODevice::WriteOnly | QIODevice::Text);
    out2 << "[[";
    QStringList list_;
    for (int i = 0; i < list.size(); i++) {
        QString rowString = "\"";
        rowString = rowString + list[i].join("\",\"");
        rowString = rowString + "\"";
        list_.push_back(rowString);
    }
    out2 << list_.join("],[");
    out2 << "]]";
    fileout2.close();

    // ------------------------------------ //

    QList<QStringList> list_update = parseNestedListFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/config.unity3d/UpdateConfig.txt");

    QStringList listRUS_update_name;
    QStringList listENG_update_name;
    QStringList listCN1_update_name;

    QFile fileinENG_update_name;
    fileinENG_update_name.setFileName(QString("../../../resource/Text/UpdateConfig.prefab/ENG_name.txt"));
    fileinENG_update_name.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inENG_update_name(&fileinENG_update_name);
    int iENG_update_name = 0;
    for (int i = 0; i < list_update.size(); i++) {
        inENG_update_name << list_update[i][3] << "\n";
    }
    fileinENG_update_name.close();

    QFile fileinCN1_update_name;
    fileinCN1_update_name.setFileName(QString("../../../resource/Text/UpdateConfig.prefab/CN1_name.txt"));
    fileinCN1_update_name.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inCN1_update_name(&fileinCN1_update_name);
    int iCN1_update_name = 0;
    for (int i = 0; i < list_update.size(); i++) {
        inCN1_update_name << list_update[i][2] << "\n";
    }
    fileinCN1_update_name.close();

    QStringList listRUS_update_content;
    QStringList listENG_update_content;
    QStringList listCN1_update_content;

    QFile fileinENG_update_content;
    fileinENG_update_content.setFileName(QString("../../../resource/Text/UpdateConfig.prefab/ENG_content.txt"));
    fileinENG_update_content.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inENG_update_content(&fileinENG_update_content);
    int iENG_update_content = 0;
    for (int i = 0; i < list_update.size(); i++) {
        inENG_update_content << list_update[i][5] << "\n";
    }
    fileinENG_update_content.close();

    QFile fileinCN1_update_content;
    fileinCN1_update_content.setFileName(QString("../../../resource/Text/UpdateConfig.prefab/CN1_content.txt"));
    fileinCN1_update_content.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inCN1_update_content(&fileinCN1_update_content);
    int iCN1_update_content = 0;
    for (int i = 0; i < list_update.size(); i++) {
        inCN1_update_content << list_update[i][4] << "\n";
    }
    fileinCN1_update_content.close();

    QFile filein1_name_1;
    filein1_name_1.setFileName(QString("../../../resource/Text/UpdateConfig.prefab/RUS_name.txt"));
    filein1_name_1.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in1_name_1(&filein1_name_1);
    int i_name_1 = 0;
    while (!in1_name_1.atEnd()) {
        QString row = in1_name_1.readLine();
        if (i_name_1 != 0 && i_name_1 != 1 && i_name_1 != 2) {
            list_update[i_name_1][2] = row;
            list_update[i_name_1][3] = list_update[i_name_1][2];
        }
        i_name_1++;
    }
    filein1_name_1.close();

    QFile filein1_content_1;
    filein1_content_1.setFileName(QString("../../../resource/Text/UpdateConfig.prefab/RUS_content.txt"));
    filein1_content_1.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in1_content_1(&filein1_content_1);
    int i_content_1 = 0;
    while (!in1_content_1.atEnd()) {
        QString row = in1_content_1.readLine();
        if (i_content_1 != 0 && i_content_1 != 1 && i_content_1 != 2) {
            list_update[i_content_1][4] = splitIntoLinesMax50(row);
            list_update[i_content_1][5] = list_update[i_content_1][4];
        }
        i_content_1++;
    }
    filein1_content_1.close();

    QFile fileout2_update;
    QTextStream out2_update(&fileout2_update);
    fileout2_update.setFileName(QString("../../../resource/Assets/new/Song_Data/StreamingAssets/config.unity3d/UpdateConfig.txt"));
    fileout2_update.open(QIODevice::WriteOnly | QIODevice::Text);
    out2_update << "[[";
    QStringList list__update;
    for (int i = 0; i < list_update.size(); i++) {
        QString rowString_update = "\"";
        rowString_update = rowString_update + list_update[i].join("\",\"");
        rowString_update = rowString_update + "\"";
        list__update.push_back(rowString_update);
    }
    out2_update << list__update.join("],[");
    out2_update << "]]";
    fileout2_update.close();

    // ------------------------------------ //

    QList<QStringList> list_update_test = parseNestedListFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/config.unity3d/UpdateTestConfig.txt");

    QStringList listRUS_update_test_name;
    QStringList listENG_update_test_name;
    QStringList listCN1_update_test_name;

    QFile fileinENG_update_test_name;
    fileinENG_update_test_name.setFileName(QString("../../../resource/Text/UpdateTestConfig.prefab/ENG_name.txt"));
    fileinENG_update_test_name.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inENG_update_test_name(&fileinENG_update_test_name);
    int iENG_update_test_name = 0;
    for (int i = 0; i < list_update_test.size(); i++) {
        inENG_update_test_name << list_update_test[i][3] << "\n";
    }
    fileinENG_update_test_name.close();

    QFile fileinCN1_update_test_name;
    fileinCN1_update_test_name.setFileName(QString("../../../resource/Text/UpdateTestConfig.prefab/CN1_name.txt"));
    fileinCN1_update_test_name.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inCN1_update_test_name(&fileinCN1_update_test_name);
    int iCN1_update_test_name = 0;
    for (int i = 0; i < list_update_test.size(); i++) {
        inCN1_update_test_name << list_update_test[i][2] << "\n";
    }
    fileinCN1_update_test_name.close();

    QStringList listRUS_update_test_content;
    QStringList listENG_update_test_content;
    QStringList listCN1_update_test_content;

    QFile fileinENG_update_test_content;
    fileinENG_update_test_content.setFileName(QString("../../../resource/Text/UpdateTestConfig.prefab/ENG_content.txt"));
    fileinENG_update_test_content.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inENG_update_test_content(&fileinENG_update_test_content);
    int iENG_update_test_content = 0;
    for (int i = 0; i < list_update_test.size(); i++) {
        inENG_update_test_content << list_update_test[i][5] << "\n";
    }
    fileinENG_update_test_content.close();

    QFile fileinCN1_update_test_content;
    fileinCN1_update_test_content.setFileName(QString("../../../resource/Text/UpdateTestConfig.prefab/CN1_content.txt"));
    fileinCN1_update_test_content.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream inCN1_update_test_content(&fileinCN1_update_test_content);
    int iCN1_update_test_content = 0;
    for (int i = 0; i < list_update_test.size(); i++) {
        inCN1_update_test_content << list_update_test[i][4] << "\n";
    }
    fileinCN1_update_test_content.close();

    QFile filein1_name_2;
    filein1_name_2.setFileName(QString("../../../resource/Text/UpdateTestConfig.prefab/RUS_name.txt"));
    filein1_name_2.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in1_name_2(&filein1_name_2);
    int i_name_2 = 0;
    while (!in1_name_2.atEnd()) {
        QString row = in1_name_2.readLine();
        if (i_name_2 != 0 && i_name_2 != 1 && i_name_2 != 2) {
            list_update_test[i_name_2][2] = row;
            list_update_test[i_name_2][3] = list_update_test[i_name_2][2];
        }
        i_name_2++;
    }
    filein1_name_2.close();

    QFile filein1_content_2;
    filein1_content_2.setFileName(QString("../../../resource/Text/UpdateTestConfig.prefab/RUS_content.txt"));
    filein1_content_2.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in1_content_2(&filein1_content_2);
    int i_content_2 = 0;
    while (!in1_content_2.atEnd()) {
        QString row = in1_content_2.readLine();
        if (i_content_2 != 0 && i_content_2 != 1 && i_content_2 != 2) {
            list_update_test[i_content_2][4] = splitIntoLinesMax50(row);
            list_update_test[i_content_2][5] = list_update_test[i_content_2][4];
        }
        i_content_2++;
    }
    filein1_content_2.close();

    QFile fileout2_update_test;
    QTextStream out2_update_test(&fileout2_update_test);
    fileout2_update_test.setFileName(QString("../../../resource/Assets/new/Song_Data/StreamingAssets/config.unity3d/UpdateTestConfig.txt"));
    fileout2_update_test.open(QIODevice::WriteOnly | QIODevice::Text);
    out2_update_test << "[[";
    QStringList list__update_test;
    for (int i = 0; i < list_update_test.size(); i++) {
        QString rowString_update_test = "\"";
        rowString_update_test = rowString_update_test + list_update_test[i].join("\",\"");
        rowString_update_test = rowString_update_test + "\"";
        list__update_test.push_back(rowString_update_test);
    }
    out2_update_test << list__update_test.join("],[");
    out2_update_test << "]]";
    fileout2_update_test.close();

    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao2_EN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao2_EN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_1.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_1.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_1_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_1_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_2.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_2.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_2_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao1_2_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao2.json"                                                               ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao2.json"                                                               );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao2_CN2.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/mappiece.unity3d/kuang_jianzao2_CN2.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_zhou.json"                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_zhou.json"                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_zhu.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_zhu.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_bao.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_bao.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_fan.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_fan.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_fan1.json"                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_fan1.json"                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_jiang.json"                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_jiang.json"                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_li.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_li.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_li1.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_li1.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_liang.json"                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_liang.json"                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_liu.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_liu.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_lixianzhong.json"                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_lixianzhong.json"                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_ouyang.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_ouyang.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_shishi.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_shishi.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_sima.json"                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_sima.json"                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_su.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_su.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_wang.json"                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_wang.json"                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_yan.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_yan.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_yu.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_yu.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_yu1.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_celebrity.unity3d/famous_saying_yu1.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_24.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_24.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_25.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_25.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_26.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_26.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_27.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_27.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_28.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_28.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_29.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_29.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_1_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_1_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_2_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_2_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_3_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_3_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_4_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_4_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_5_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_5_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_6_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_6_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_7_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_7_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_8_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_8_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_9_CN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_9_CN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_10_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_10_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_11_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_11_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_12_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_12_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_13_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_13_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_14_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_14_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_15_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_15_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_16_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_16_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_17_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_17_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_18_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_18_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_19_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_19_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_20_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_20_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_21_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_21_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_22_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_22_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_23_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_23_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_24_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_24_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_25_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_25_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_26_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_26_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_27_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_27_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_28_CN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_28_CN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_EN.json"                                                              ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/shengzhi_EN.json"                                                              );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_1.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_1.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_2.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_2.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_3.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_3.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_4.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_4.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_5.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_5.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_6.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_6.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_7.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_7.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_8.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_8.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_9.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_9.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_9EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_9EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_10.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_10.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_11.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_11.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_12.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_12.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_13.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_13.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_14.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_14.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_15.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_15.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_16.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_16.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_17.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_17.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_18.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_18.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_19.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_19.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_20.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_20.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_21.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_21.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_22.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_22.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_23.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_edict.unity3d/zhao_23.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/40EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/40EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/1CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/1CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/1EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/1EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/2CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/2CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/2EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/2EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/3CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/3CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/3EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/3EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/4CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/4CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/4EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/4EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/5CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/5CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/5EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/5EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/6CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/6CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/6EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/6EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/7CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/7CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/7EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/7EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/8CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/8CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/8EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/8EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/9CN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/9CN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/9EN.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/9EN.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/10CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/10CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/10EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/10EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/11CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/11CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/11EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/11EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/12CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/12CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/12EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/12EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/13CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/13CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/13EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/13EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/14CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/14CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/14EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/14EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/15CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/15CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/15EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/15EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/16CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/16CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/16EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/16EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/17CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/17CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/17EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/17EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/18CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/18CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/18EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/18EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/19CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/19CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/19EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/19EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/20CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/20CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/20EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/20EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/21CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/21CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/21EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/21EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/22CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/22CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/22EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/22EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/23CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/23CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/23EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/23EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/24CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/24CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/24EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/24EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/25CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/25CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/25EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/25EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/26CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/26CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/26EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/26EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/27CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/27CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/27EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/27EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/28CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/28CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/28EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/28EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/29CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/29CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/29EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/29EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/30CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/30CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/30EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/30EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/31CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/31CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/31EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/31EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/32CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/32CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/32EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/32EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/33CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/33CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/33EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/33EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/34CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/34CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/34EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/34EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/35CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/35CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/35EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/35EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/36CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/36CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/36EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/36EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/37CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/37CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/37EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/37EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/38CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/38CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/38EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/38EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/39CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/39CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/39EN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/39EN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_guide.unity3d/40CN.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_guide.unity3d/40CN.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info40_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info40_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info40_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info40_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_ks.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_ks.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_main.json"                                                              ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_main.json"                                                              );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_01.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_01.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_02.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_02.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_03.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_03.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_04.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_04.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_05.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_05.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_06.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/credits_modian_06.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon1_CN1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon1_CN1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon1_CN2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon1_CN2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon1_EN.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon1_EN.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon2_CN1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon2_CN1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon2_CN2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon2_CN2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon2_EN.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon2_EN.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon3_CN1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon3_CN1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon3_CN2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon3_CN2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon3_EN.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon3_EN.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon4_CN1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon4_CN1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon4_CN2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon4_CN2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon4_EN.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info_icon4_EN.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info0_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info0_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info0_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info0_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info1_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info1_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info1_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info1_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info1_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info1_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info2_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info2_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info2_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info2_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info2_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info2_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info3_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info3_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info3_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info3_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info3_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info3_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info4_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info4_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info4_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info4_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info4_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info4_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info5_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info5_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info5_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info5_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info5_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info5_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info6_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info6_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info6_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info6_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info6_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info6_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info7_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info7_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info7_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info7_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info7_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info7_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info8_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info8_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info8_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info8_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info8_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info8_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info9_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info9_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info9_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info9_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info9_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info9_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info10_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info10_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info10_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info10_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info10_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info10_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info11_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info11_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info11_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info11_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info11_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info11_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info12_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info12_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info12_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info12_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info12_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info12_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info13_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info13_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info13_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info13_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info13_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info13_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info14_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info14_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info14_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info14_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info14_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info14_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info15_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info15_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info15_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info15_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info15_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info15_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info16_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info16_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info16_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info16_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info16_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info16_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info17_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info17_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info17_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info17_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info17_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info17_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info18_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info18_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info18_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info18_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info18_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info18_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info19_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info19_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info19_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info19_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info19_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info19_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info20_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info20_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info20_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info20_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info20_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info20_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info21_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info21_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info21_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info21_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info21_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info21_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info22_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info22_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info22_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info22_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info22_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info22_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info23_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info23_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info23_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info23_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info23_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info23_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info24_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info24_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info24_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info24_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info24_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info24_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info25_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info25_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info25_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info25_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info25_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info25_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info26_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info26_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info26_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info26_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info26_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info26_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info27_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info27_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info27_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info27_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info27_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info27_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info28_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info28_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info28_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info28_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info28_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info28_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info29_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info29_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info29_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info29_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info29_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info29_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info30_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info30_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info30_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info30_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info30_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info30_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info31_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info31_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info31_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info31_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info31_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info31_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info32_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info32_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info32_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info32_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info32_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info32_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info33_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info33_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info33_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info33_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info33_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info33_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info34_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info34_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info34_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info34_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info34_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info34_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info35_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info35_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info35_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info35_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info35_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info35_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info36_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info36_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info36_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info36_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info36_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info36_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info37_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info37_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info37_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info37_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info37_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info37_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info38_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info38_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info38_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info38_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info38_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info38_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info39_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info39_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info39_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info39_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info39_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info39_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_help.unity3d/info40_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_help.unity3d/info40_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size4_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size4_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size1_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size1_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size1_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size1_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size1_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size1_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size2_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size2_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size2_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size2_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size2_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size2_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size3_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size3_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size3_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size3_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size3_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size3_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size4_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size4_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size4_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/sprite_smallmap.unity3d/endless_size4_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/罢官前_EN.json"                                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/罢官前_EN.json"                                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button1_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/anpai_button2_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/build_kuang3_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_1_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_2_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_3_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_4_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_5_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_6_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_7_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_8_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_9_CN1 .json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_9_CN1 .json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_9_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_9_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_10_CN1 .json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_10_CN1 .json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_10_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/buildlist_10_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding_CN1.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding_CN1.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding_CN2.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding_CN2.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding_EN.json"                                                  ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_chaibuilding_EN.json"                                                  );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding_CN1.json"                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding_CN1.json"                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding_CN2.json"                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding_CN2.json"                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding_EN.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_closebuilding_EN.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_open.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_open.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_open_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_open_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_open_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_open_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_open_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_open_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_quanxuan_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild_CN1.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild_CN1.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild_CN2.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild_CN2.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild_EN.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_rebuild_EN.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild_CN1.json"                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild_CN1.json"                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild_CN2.json"                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild_CN2.json"                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild_EN.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_startbuild_EN.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild_CN1.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild_CN1.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild_CN2.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild_CN2.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild_EN.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/button_stopbuild_EN.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/choose_face1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/choose_face1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/choose_face1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/choose_face1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/choose_face2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/choose_face2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/choose_face2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/choose_face2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/duilian1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/duilian1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/duilian2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/duilian2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/duilian3.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/duilian3.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/end.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/end.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/GG.json"                                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/GG.json"                                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/GG2.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/GG2.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/huigu_renwu2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_fanrong_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_laozuo_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keji_minsheng_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keju_end.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keju_end.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keju_end_CN1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keju_end_CN1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keju_end_CN2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keju_end_CN2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/keju_end_EN.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/keju_end_EN.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/leftdown_bg3_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc_CN1.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc_CN1.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc_EN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc_EN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc2_CN1.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc2_CN1.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc2_EN.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/sougua_npc2_EN.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1.json"                                                               ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1.json"                                                               );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1_CN1.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1_CN1.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1_CN2.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1_CN2.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1_EN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju1_EN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju2_CN1.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju2_CN1.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju2_CN2.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju2_CN2.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju2_EN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_jieju2_EN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1_CN1.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1_CN1.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1_CN2.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1_CN2.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1_EN.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_kingiscoming1_EN.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/ui_wonder_complete.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/ui_wonder_complete.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/zhengce_icon1_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/罢官前.json"                                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/罢官前.json"                                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/罢官前_CN1.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/罢官前_CN1.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_battle.unity3d/罢官前_CN2.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_battle.unity3d/罢官前_CN2.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_shortcuts_EN.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_shortcuts_EN.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/logo_big_CN1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/logo_big_CN1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/logo_big_EN.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/logo_big_EN.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/UpdatePlan_CN1.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/UpdatePlan_CN1.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/UpdatePlan_EN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/UpdatePlan_EN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_load_CN1.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_load_CN1.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_load_CN2.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_load_CN2.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_load_EN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_load_EN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_save_CN1.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_save_CN1.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_save_CN2.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_save_CN2.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_save_EN.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_save_EN.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_setting_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_setting_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_setting_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_setting_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_setting_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_setting_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_shortcuts_CN1.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_shortcuts_CN1.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_common.unity3d/word_shortcuts_CN2.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_common.unity3d/word_shortcuts_CN2.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/word_map_EN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/word_map_EN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size1_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size1_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size1_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size1_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size1_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size1_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size2_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size2_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size2_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size2_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size2_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size2_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size3_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size3_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size3_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size3_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size3_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size3_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size4_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size4_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size4_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size4_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size4_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size4_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size5_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size5_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size5_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size5_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size5_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_size5_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word1_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word1_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word1_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word1_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word1_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word1_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word2_CN1.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word2_CN1.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word2_CN2.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word2_CN2.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word2_EN.json"                                                      ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/endless_word2_EN.json"                                                      );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete_CN1.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete_CN1.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete_CN2.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete_CN2.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete_EN.json"                                                     ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/story_complete_EN.json"                                                     );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/word_load_CN2.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/word_load_CN2.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_gamemode.unity3d/word_map_CN1.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_gamemode.unity3d/word_map_CN1.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_init.unity3d/logo_big_EN.json"                                                               ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_init.unity3d/logo_big_EN.json"                                                               );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_init.unity3d/logo_big_CN1.json"                                                              ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_init.unity3d/logo_big_CN1.json"                                                              );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_init.unity3d/logo_big_CN2.json"                                                              ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_init.unity3d/logo_big_CN2.json"                                                              );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_language.unity3d/EN.json"                                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_language.unity3d/EN.json"                                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_language.unity3d/CN.json"                                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_language.unity3d/CN.json"                                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_language.unity3d/CN2.json"                                                                   ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_language.unity3d/CN2.json"                                                                   );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_words_EN.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_words_EN.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/AD_bg_CN1.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/AD_bg_CN1.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/AD_bg_CN2.json"                                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/AD_bg_CN2.json"                                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/AD_bg_EN.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/AD_bg_EN.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language1.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language1.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language1_CN2.json"                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language1_CN2.json"                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language1_EN.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language1_EN.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language2.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language2.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language2_CN2.json"                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language2_CN2.json"                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language2_EN.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language2_EN.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language3.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language3.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language3_CN2.json"                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language3_CN2.json"                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language3_EN.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language3_EN.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language4.json"                                                    ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language4.json"                                                    );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language4_CN2.json"                                                ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language4_CN2.json"                                                );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/button_main_language4_EN.json"                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/button_main_language4_EN.json"                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/logo_big.json"                                                                 ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/logo_big.json"                                                                 );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/logo_big_CN1.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/logo_big_CN1.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/logo_big_CN1_1.json"                                                           ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/logo_big_CN1_1.json"                                                           );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/logo_big_CN2.json"                                                             ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/logo_big_CN2.json"                                                             );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/logo_big_EN 1.json"                                                            ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/logo_big_EN 1.json"                                                            );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/logo_big_EN.json"                                                              ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/logo_big_EN.json"                                                              );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word1_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word1_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word1_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word1_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word1_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word1_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word1_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word1_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word1_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word1_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word1_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word1_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word2_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word2_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word2_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word2_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word2_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word2_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word2_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word2_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word2_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word2_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word2_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word2_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word3_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word3_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word3_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word3_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word3_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word3_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word3_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word3_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word3_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word3_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word3_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word3_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word4_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word4_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word4_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word4_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word4_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word4_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word4_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word4_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word4_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word4_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word4_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word4_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word5_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word5_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word5_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word5_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word5_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word5_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word5_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word5_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word5_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word5_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word5_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word5_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word6_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word6_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word6_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word6_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word6_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word6_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word6_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word6_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word6_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word6_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word6_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word6_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word7_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word7_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word7_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word7_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word7_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word7_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word7_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word7_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word7_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word7_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word7_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word7_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word8_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word8_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word8_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word8_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word8_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word8_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word8_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word8_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word8_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word8_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word8_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word8_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word9_1_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word9_1_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word9_1_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word9_1_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word9_1_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word9_1_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word9_2_CN1.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word9_2_CN1.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word9_2_CN2.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word9_2_CN2.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word9_2_EN.json"                                                         ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word9_2_EN.json"                                                         );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word10_1_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word10_1_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word10_1_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word10_1_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word10_1_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word10_1_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word10_2_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word10_2_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word10_2_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word10_2_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word10_2_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word10_2_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word11_1_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word11_1_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word11_1_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word11_1_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word11_1_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word11_1_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word11_2_CN1.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word11_2_CN1.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word11_2_CN2.json"                                                       ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word11_2_CN2.json"                                                       );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_word11_2_EN.json"                                                        ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_word11_2_EN.json"                                                        );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_words.json"                                                              ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_words.json"                                                              );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_words_CN1.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_words_CN1.json"                                                          );
    replaceLinesFromFile("../../../resource/Assets/old/Song_Data/StreamingAssets/window_start.unity3d/start_words_CN2.json"                                                          ,  "../../../resource/Assets/new/Song_Data/StreamingAssets/window_start.unity3d/start_words_CN2.json"                                                          );
    return 0;
}
