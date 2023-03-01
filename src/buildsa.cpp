#include "libsais64.h"
#include <math.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <typeinfo>
#include <string>
#include <cctype>

int main(int argc, char* argv[]) {
  //auto fullBegin = std::chrono::high_resolution_clock::now();
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

  //will read all the lines except those with headers and stops at next header


  // std::string fileText;
  // std::ifstream MyReadFile("./FASTA/ecoliedit.fa");
  // if (errno != 0){
  //   std::cout << "Error: " << strerror(errno);
  // }
  // std::string temp;


  //Opting to only read one record as per the follow up on https://piazza.com/class/ld0gqwk4xme2vy/post/13
  // std::int16_t readOneGenome = 0;
  // std::string replace = "ACGT";
  // while ((readOneGenome < 2) && getline (MyReadFile, temp)) {
  //   if ((!temp.empty()) && (temp.at(0) != '>')) {
  //     int64_t tSize = temp.size(); 
  //     for (size_t i = 0; i < tSize; ++i) {
  //       if (temp[i] == 'n' || temp[i] == 'N'){
  //         int64_t val = rand()%4;
  //         temp[i] = replace[val];
  //       }
  //     }
  //     fileText = fileText + temp;
  //     temp.clear();
  //   } else {
  //     readOneGenome++;
  //   }
  // }
  // fileText = fileText + "$";
  // transform(fileText.begin(), fileText.end(), fileText.begin(), ::toupper);
  // std::cout << fileText;
  // MyReadFile.close();
  

  // std::unique_ptr<int64_t[]> sa(new int64_t[fileText.size()]);
  // int64_t n = fileText.size();
  // assert(n > 0);
  // int64_t fs = 0;
  // int64_t* freq = nullptr;

  // int64_t ret = libsais64(reinterpret_cast<uint8_t*>(fileText.data()), sa.get(), n, fs, freq);
  // if (ret == 0) {
  //   std::cout << "suffix array construction successful!\n";
  //   // std::cout  << "[ ";
  //   // for (size_t i = 0; i < n; ++i) {
  //   //   std::cout << sa[i] << ((i < n-1) ? ", " : " ");
  //   // }
  //   // std::cout << "]\n";
  // } else {
  //   std::cout << "suffix array construction failed; exit code = " << ret << "\n";
  // }

  // std::ofstream MyFile("./outputs/output1.txt");
  // MyFile << fileText;
  // MyFile << "\n[ ";
  // for (size_t i = 0; i < n; ++i) {
  //   MyFile << sa[i] << ((i < n-1) ? ", " : " ");
  // }
  // MyFile << "]";
  // MyFile.close();



  //Prefix table ramblings
  //Need to store k and then get general size
  //Need to store 2 numbers for each prefix set 
  //Could binary search and grow each side until no good

  // int64_t k = 4;

  // int64_t count = int(pow(4, k));
  // std::string prefixes[count];

  // for (size_t i = 0; i < count; ++i) {
  //   std::string temp = "";
  //   int64_t val = i;
  //   for (size_t j = k; j > 1; --j) {
  //     int64_t power = int(pow(4, j-1));
  //     temp = temp + replace[val/power];
  //     val = val - (val/power*power);
  //   }
  //   temp = temp + replace[val%4];
  //   prefixes[i] = temp;
  // }
    
  // std::cout  << "[ ";
  // for (size_t i = 0; i < count; ++i) {
  //   std::cout << prefixes[i] << ((i < count-1) ? ", " : " ");
  // }
  // std::cout << "]\n";

  // int64_t startL[count], endL[count];
  // for (size_t i = 0; i < count; ++i){
  //   startL[i] = -1;
  //   endL[i] = -1;
  // }


  // int64_t start = 0, end = 1;
  // int64_t eval;
  // size_t i = 0;
  // while (i < count && start < n){
  //   if (sa[start] <= (n-k)) {
  //     std::string str2 = fileText.substr(sa[start],k);
  //     //std::cout << str2 << std::endl;
  //     //std::cout << prefixes[i] << std::endl;
  //     //eval = strcmp((char *) &prefixes[i], (char *) &str2);
  //     //std::cout << eval << std::endl;
  //     if (prefixes[i] < str2){
  //       i = i + 1;
  //     } else if (prefixes[i] == str2) {
  //       if (startL[i] == -1){
  //         startL[i] = start;
  //       }
  //       endL[i] = start;
  //       start = start + 1;
  //     } else {
  //       start = start + 1;
  //     }
  //   } else {
  //     start = start + 1;
  //   }
  // }

  // std::cout  << "[ ";
  // for (size_t i = 0; i < count; ++i) {
  //   std::cout << startL[i] << ((i < count-1) ? ", " : " ");
  // }
  // std::cout << "]\n";

  // std::cout  << "[ ";
  // for (size_t i = 0; i < count; ++i) {
  //   std::cout << endL[i] << ((i < count-1) ? ", " : " ");
  // }
  // std::cout << "]\n";









  // std::ofstream output("./outputs/test.bin", std::ios::out | std::ios::binary);
  // output.write((char *) &n, sizeof(n));
  // output.write(fileText.c_str(), n);
  // output.write((char *) &sa[0], n*sizeof(sa[0]));
  // output.write((char *) &k, sizeof(k));
  // output.write((char *) &startL[0], count*sizeof(startL[0]));
  // output.write((char *) &endL[0], count*sizeof(endL[0]));
  // output.close();






  /* READ BACK IN THE OUTPUT*/

  // int64_t test, pre, len; 
  // std::ifstream input("./outputs/test.bin", std::ios::in | std::ios::binary);
  // input.read((char *) &test, sizeof(test));
  // std::cout << test;
  // std::cout << "\n";
  // std::string test_str;
  // test_str.resize(test);
  // input.read((char *) test_str.c_str(), test);
  // //std::cout << test_str << std::endl;
  // std::unique_ptr<int64_t[]> tSA(new int64_t[test]);
  // input.read((char *) &tSA[0], test*sizeof(tSA[0]));
  // std::cout  << "[ ";
  // for (size_t i = 0; i < test; ++i) {
  //   std::cout << tSA[i] << ((i < test-1) ? ", " : " ");
  // }
  // std::cout << "]\n";
  // input.read((char *) &pre, sizeof(pre));
  // std::cout << pre << std::endl;
  // len = int(pow(4,pre));
  // int64_t sL[len], eL[len];
  // input.read((char *) &sL[0], len*sizeof(sL[0]));
  // input.read((char *) &eL[0], len*sizeof(eL[0]));

  /*Optional Print Out of Prefix Table*/
  // std::cout  << "[ ";
  // for (size_t i = 0; i < len; ++i) {
  //   std::cout << sL[i] << ((i < len-1) ? ", " : " ");
  // }
  // std::cout << "]\n";
  // std::cout  << "[ ";
  // for (size_t i = 0; i < len; ++i) {
  //   std::cout << eL[i] << ((i < len-1) ? ", " : " ");
  // }
  // std::cout << "]\n";



  // std::cout << sL[len-1] << std::endl;
  // std::cout << eL[len-1] << std::endl;
  // // std::string str3 = fileText.substr(tSA[4604067], 5);
  // // std::string str4 = fileText.substr(tSA[4639675], 5);
  // // std::cout << str3 << std::endl;
  // // std::cout << str4 << std::endl; 
  // input.close();












  /* Base line test string */
  /*
  std::string test_string("banana$");
  std::unique_ptr<int64_t[]> sa2(new int64_t[test_string.size()]);
  int64_t n2 = test_string.size();
  assert(n2 > 0);
  int64_t fs2 = 0;
  int64_t* freq2 = nullptr;
  
  int64_t ret2 = libsais64(reinterpret_cast<uint8_t*>(test_string.data()), sa2.get(), n2, fs2, freq2);
  if (ret2 == 0) {
    std::cout << "\nsuffix array construction successful!\n";
    std::cout  << "[ ";
    for (size_t i = 0; i < n2; ++i) {
      std::cout << sa2[i] << ((i < n2-1) ? ", " : " ");
    }
    std::cout << "]\n";
  } else {
    std::cout << "suffix array construction failed; exit code = " << ret2 << "\n";
  }
  */
  // auto fullEnd = std::chrono::high_resolution_clock::now();
  // auto fduration = std::chrono::duration_cast<std::chrono::nanoseconds>(fullEnd-fullBegin);
  // std::cout << "Full Program Time: " << fduration.count() << " nanoseconds" << std::endl;
  return 0;
}
