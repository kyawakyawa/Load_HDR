#include <iostream>  // for debug writing
#include <string>    // useful for reading and writing

#include <fstream>   // ifstream, ofstream
#include <sstream>   // std::stringstream
#include <vector>    // std::vector<std::string>

#include "Vec3.h"
#include "FColor.h"


struct Load_HDR {
    int width = 0;
    int height = 0;

    FColor *img = nullptr;

    Load_HDR(std::string filename) {
        std::ifstream reading_hdr(filename,std::ios::in);
        if(!reading_hdr) {
            std::cerr << "failed reading hdr!(file is not found)" << std::endl;
            return;
        }

        std::string header_line;

        std::cerr << "reading " << filename << "..." << std::endl;

        int count_LF = 0;//改行を数える
        while(!reading_hdr.eof()) {
            std::getline(reading_hdr,header_line);
            count_LF++;

            if(header_line[0] == '-' && header_line[1] == 'Y') {
                std::vector<std::string> v;/////////////////
                std::stringstream ss(header_line);//スペースで分割
                std::string buf;
                while(std::getline(ss,buf,' ')) {
                    v.push_back(buf);
                }/////////////////////

                width = std::stoi(v[3]);
                height = std::stoi(v[1]);
                break;
            }
        }

        reading_hdr.close();
        if(width <= 0 || height <= 0) {//上手く幅、高さを読み込めなかった
            std::cerr << "failed reading hdr!(header is wrong)"  << std::endl;
            return;
        }
        std::cerr << "width = " << width << " height = " << height << std::endl;

        img = new FColor[width * height];

        reading_hdr.open(filename,std::ios::binary);//バイナリとして開く

        if(!reading_hdr) {//見つからない
            std::cerr << "failed reading hdr!(file is not found)" << std::endl;
            return;
        }

        int count_LF_ = 0;
        while(count_LF_ < count_LF) {
            char c;
            reading_hdr.read(&c,1);
            if(c == '\n') {
                count_LF_++;
            }
        }//ヘッダまでを無視

        for(int i = 0;i < height;i++) {
            std::vector<unsigned char> r,g,b,e;

            char c;
            reading_hdr.read(&c,1);
            reading_hdr.read(&c,1);//マジックナンバーを無視

            reading_hdr.read(&c,1);
            //int w = (c & 0xff);
            //w <<= 8;
            reading_hdr.read(&c,1);//幅の指定も無視
            //w += (c & 0xff);

            if(!load_bin(reading_hdr,r)) std::cerr << "faied" << std::endl;
            if(!load_bin(reading_hdr,g)) std::cerr << "faied" << std::endl;
            if(!load_bin(reading_hdr,b)) std::cerr << "faied" << std::endl;
            if(!load_bin(reading_hdr,e)) std::cerr << "faied" << std::endl;

            for(int j = 0;j < width;j++) {
                img[i * width + j] = FColor(0.005 * r[j] * std::pow(2,e[j] - 128)
                                              ,0.005 * g[j] * std::pow(2,e[j] - 128)
                                              ,0.005 * b[j] * std::pow(2,e[j] - 128)
                );
            }
        }
    }

    bool load_bin(std::ifstream &reading_hdr,std::vector<unsigned char> &v) {

        while(v.size() < width) {

            if(reading_hdr.eof()) return false;

            char c;
            reading_hdr.read(&c,1);

            unsigned char uc = (c & 0xff);//cをunsignedとして解釈

            if(uc <= 128)  {
                for(int j = 0;j < uc;j++) {
                    if(reading_hdr.eof()) return false;
                    char c;
                    reading_hdr.read(&c,1);
                    unsigned char num = (c & 0xff);
                    v.push_back(num);
                }
            }else {
                if(reading_hdr.eof()) return false;
                char c;
                reading_hdr.read(&c,1);
                unsigned char num = (c & 0xff);//cをunsignedとして解釈
                for(int j = 0;j < uc - 128;j++) {
                    v.push_back(num);
                }
            }
        }
        return true;
    }

    ~Load_HDR() {
        delete[] img;
    }
};
