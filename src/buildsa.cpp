#include "libsais64.h"
#include <math.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <typeinfo>
#include <string>
#include <cctype>

int main(int argc, char* argv[]) {
  if ((argc != 3) && (argc != 5)){
    std::cout << "Argument count is wrong" << std::endl;
    return 1;
  } else {
    if (argc == 5) {
      std::string arg1(argv[1]);
      if (!(arg1 == "--preftab")) {
        std::cout << "--preftab argument not supplied correctly." << std::endl;
      } else {
        int64_t k;
        try {
          k = std::stoll(argv[2], nullptr, 10);
        } catch (...) {
          std::cout << "k parameter not properly supplied" << std::endl;
          return 1;
        }
        if (k < 1){
          std::cout << "k parameter must be greater than 1" << std::endl;
          return 1;
        }

        std::string arg3(argv[3]);
        std::ifstream InputFile(arg3);
        if (errno != 0){
          std::cout << "Input File Error: " << strerror(errno) << std::endl;
          return 1;
        }
        std::string arg4(argv[4]);
        std::ofstream OutputFile(arg4, std::ios::out | std::ios::binary);
        if (errno != 0){
          std::cout << "Output File Error: " << strerror(errno) << std::endl;
          InputFile.close();
          return 1;
        }
        
        std::string reference;
        std::string tmp;
        std::int16_t readOneRecord = 0;
        std::string alpha = "ACGT";
        //Per https://piazza.com/class/ld0gqwk4xme2vy/post/13_f1 I only read one record where there are multiple in a file
        while ((readOneRecord < 2) && getline (InputFile, tmp)) {
          if ((!tmp.empty()) && (tmp.at(0) != '>')) {
            int64_t tSize = tmp.size(); 
            for (size_t i = 0; i < tSize; ++i) {
              if (tmp[i] == 'n' || tmp[i] == 'N'){
                int64_t val = rand()%4;
                tmp[i] = alpha[val];
              }
            }
            tmp.erase(std::remove_if(tmp.begin(), tmp.end(), ::isspace), tmp.end());
            reference = reference + tmp;
            tmp.clear();
          } else {
            readOneRecord++;
          }
        }
        reference = reference + "$";
        transform(reference.begin(), reference.end(), reference.begin(), ::toupper);
        InputFile.close();       

        int64_t fSize = reference.size();
        assert(fSize > 0);
        int64_t fs = 0;
        int64_t* freq = nullptr;

        auto begin = std::chrono::high_resolution_clock::now();
        std::unique_ptr<int64_t[]> suffArr(new int64_t[reference.size()]);
        int64_t ret = libsais64(reinterpret_cast<uint8_t*>(reference.data()), suffArr.get(), fSize, fs, freq);
        if (ret != 0) {
          std::cout << "suffix array construction failed; exit code = " << ret << "\n";
          return -1;
        } 
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
        std::cout << "Time to make SA: " << duration.count() << " nanoseconds" << std::endl;

        int64_t pCount = int(pow(4, k));
        std::string pref[pCount];
        int64_t pStart[pCount], pEnd[pCount];
        for (size_t i = 0; i < pCount; ++i) {
          pStart[i] = -1;
          pEnd[i] = -1;
          std::string temp = "";
          int64_t val = i;
          for (size_t j = k; j > 1; --j) {
            int64_t power = int(pow(4, j-1));
            temp = temp + alpha[val/power];
            val = val - (val/power*power);
          }
          temp = temp + alpha[val%4];
          pref[i] = temp;
        }


        int64_t start = 0;
        int64_t eval;
        size_t i = 0;
        while (i < pCount && start < fSize){
          if (suffArr[start] <= (fSize-k)) {
            std::string sub = reference.substr(suffArr[start],k);
            if (pref[i] < sub){
              i = i + 1;
            } else if (pref[i] == sub) {
              if (pStart[i] == -1){
                pStart[i] = start;
              }
              pEnd[i] = start;
              start = start + 1;
            } else {
              start = start + 1;
            }
          } else {
            start = start + 1;
          }
        }

        OutputFile.write((char *) &fSize, sizeof(fSize));
        OutputFile.write(reference.c_str(), fSize);
        OutputFile.write((char *) &suffArr[0], fSize*sizeof(suffArr[0]));
        OutputFile.write((char *) &k, sizeof(k));
        OutputFile.write((char *) &pStart[0], pCount*sizeof(pStart[0]));
        OutputFile.write((char *) &pEnd[0], pCount*sizeof(pEnd[0]));

        OutputFile.close();
      }
    } else {
      std::string arg1(argv[1]);
      std::ifstream InputFile(arg1);
      if (errno != 0){
        std::cout << "Input File Error: " << strerror(errno) << std::endl;
        return 1;
      }
      std::string arg2(argv[2]);
      std::ofstream OutputFile(arg2, std::ios::out | std::ios::binary);
      if (errno != 0){
        std::cout << "Output File Error: " << strerror(errno) << std::endl;
        InputFile.close();
        return 1;
      }
      
      std::string reference;
      std::string tmp;
      std::int16_t readOneRecord = 0;
      std::string alpha = "ACGT";
      while ((readOneRecord < 2) && getline (InputFile, tmp)) {
        if ((!tmp.empty()) && (tmp.at(0) != '>')) {
          int64_t tSize = tmp.size(); 
          for (size_t i = 0; i < tSize; ++i) {
            if (tmp[i] == 'n' || tmp[i] == 'N'){
              int64_t val = rand()%4;
              tmp[i] = alpha[val];
            }
          }
          tmp.erase(std::remove_if(tmp.begin(), tmp.end(), ::isspace), tmp.end());
          reference = reference + tmp;
          tmp.clear();
        } else {
          readOneRecord++;
        }
      }
      reference = reference + "$";
      transform(reference.begin(), reference.end(), reference.begin(), ::toupper);
      InputFile.close();       

      int64_t fSize = reference.size();
      assert(fSize > 0);
      int64_t fs = 0;
      int64_t* freq = nullptr;

      auto begin = std::chrono::high_resolution_clock::now();
      std::unique_ptr<int64_t[]> suffArr(new int64_t[reference.size()]);
      int64_t ret = libsais64(reinterpret_cast<uint8_t*>(reference.data()), suffArr.get(), fSize, fs, freq);
      if (ret != 0) {
        std::cout << "suffix array construction failed; exit code = " << ret << "\n";
        return -1;
      } 
      auto end = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
      std::cout << "Time to make SA: " << duration.count() << " nanoseconds" << std::endl;

      int64_t k = 0;
      OutputFile.write((char *) &fSize, sizeof(fSize));
      OutputFile.write(reference.c_str(), fSize);
      OutputFile.write((char *) &suffArr[0], fSize*sizeof(suffArr[0]));
      OutputFile.write((char *) &k, sizeof(k));

      OutputFile.close();
    
    }
  }

  return 0;
}
