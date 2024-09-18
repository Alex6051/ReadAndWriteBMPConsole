#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <string>
using namespace std;

class Image {
public:
    virtual bool openImage(const string& fileName) = 0;
    virtual void displayImage() const = 0;
    virtual void closeImage() = 0;
    virtual ~Image() = default;
};

class BMP : public Image {
public:
    bool openImage(const string& fileName) override {
        bmpFile.open(fileName, ios::binary);
        if (!bmpFile.is_open()) {
            cerr << "������ �������� ����� BMP: " << fileName << endl;
            return false;
        }

        bmpFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        bmpFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
        if (fileHeader.bfType != 0x4D42) { //'BM' � ASCII
            cerr << "������������ ������ BMP �����" << endl;
            return false;
        }

        if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
            cerr << "�������������� ������ 24-������ � 32-������ BMP �����������" << endl;
            return false;
        }

        size_t imageSize = infoHeader.biWidth * infoHeader.biHeight * (infoHeader.biBitCount / 8);
        pixelData.resize(imageSize);
        bmpFile.read(reinterpret_cast<char*>(pixelData.data()), imageSize);

        return true;
    }

    void displayImage() const override {
        int bytesPerPixel = infoHeader.biBitCount / 8;
        for (int y = infoHeader.biHeight - 1; y >= 0; y--) {
            for (int x = 0; x < infoHeader.biWidth; x++) {
                int index = (x + y * infoHeader.biWidth) * bytesPerPixel;

                uint8_t blue = pixelData[index];
                uint8_t green = pixelData[index + 1];
                uint8_t red = pixelData[index + 2];
                if (red == 0 && green == 0 && blue == 0) 
                    cout << "#";
                else if (red == 255 && green == 255 && blue == 255) 
                    cout << ".";
                else {
                    cerr << "������: ����������� �������� ������������ �����" << endl;
                    return;
                }
            }
            cout << endl;
        }
    }

    void closeImage() override {
        if (bmpFile.is_open()) 
            bmpFile.close();
    }

private:
    ifstream bmpFile;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    vector<uint8_t> pixelData;
};

class ImageFactory {
public:
    static unique_ptr<Image> createImageObject(const string& fileName) {
        if (fileName.substr(fileName.find_last_of('.') + 1) == "bmp") {
            return make_unique<BMP>();
        }
        else {
            cerr << "������ ����������� �� ��������������" << endl;
            return nullptr;
        }
    }
};

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    string inputUserText;

    while (1) {
        cout << "�������� \"�����\" ��� ���������� ��������� ���� ������� ���� � ����������� : ";
        getline(cin, inputUserText);
        if (inputUserText == "�����" || inputUserText == "�����" || inputUserText == "Exit" || inputUserText == "exit")
            break;

        unique_ptr<Image> reader = ImageFactory::createImageObject(inputUserText);
        if (reader && reader->openImage(inputUserText)) {
            reader->displayImage();
            reader->closeImage();
        }
    }
    return 0;
}
