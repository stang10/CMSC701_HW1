#include <math.h>
#include <iostream>
#include <memory>
#include <fstream>
#include <typeinfo>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

int64_t * binaryQuery(std::string ref, int64_t sa[], int64_t l, int64_t r, std::string query, int64_t qLen) {
    static int64_t ret[3];
    if (r >= l){
        int64_t c = l + (r-l)/2;
        std::string comp = ref.substr(sa[c], qLen);

        if(comp == query){
            ret[0] = c; 
            ret[1] = l;
            ret[2] = r;
            return ret;
        }

        if(comp > query){
            return binaryQuery(ref, sa, l, (c-1), query, qLen); 
        } 

        return binaryQuery(ref, sa, (c+1), r, query, qLen);

    } else {
        ret[0] = -1;
        return ret;
    }
}

int64_t lcp (std::string s1, int64_t s1Len, std::string s2, int64_t s2Len){
    int64_t incr = 0; 
    while((incr < s1Len) && (incr < s2Len)){
        if (s1[incr] != s2[incr]){
            //Can return current incr number as it marks the length of lcp
            return incr;
        }
        incr = incr + 1;
    }
    return incr; 
}

int64_t * lcpBinaryQuery(std::string ref, int64_t sa[], int64_t l, int64_t lLCP, int64_t r, int64_t rLCP, std::string query, int64_t qLen) {
    static int64_t ret[6];
    if (r >= l){
        int64_t c = l + (r-l)/2;
        std::string comp = ref.substr(sa[c], qLen);
        int64_t useLCP = std::min(lLCP, rLCP);
        std::string lcpComp = comp.substr(useLCP);
        std::string lcpQuery = query.substr(useLCP);

        int64_t cLCP = useLCP + lcp(lcpComp, lcpComp.size(), lcpQuery, lcpQuery.size());
        if(lcpComp == lcpQuery){
            ret[0] = c; 
            ret[1] = l;
            ret[2] = r;
            ret[3] = cLCP;
            ret[4] = lLCP;
            ret[5] = rLCP;
            return ret;
        }

        if(lcpComp > lcpQuery){
            return lcpBinaryQuery(ref, sa, l, lLCP, (c-1), cLCP, query, qLen); 
        } 

        return lcpBinaryQuery(ref, sa, (c+1), cLCP, r, rLCP, query, qLen);

    } else {
        ret[0] = -1;
        return ret;
    }
}

int main(int argc, char* argv[]) {
    if (argc == 5){
        std::string arg1(argv[1]);
        std::ifstream Index(arg1, std::ios::in | std::ios::binary);
        if (errno != 0){
            std::cout << "Input File Error: " << strerror(errno) << std::endl;
            return 1;
        }

        std::string arg2(argv[2]);
        std::ifstream QueryFile(arg2);
        if (errno != 0){
          std::cout << "Query File Error: " << strerror(errno) << std::endl;
          Index.close();
          return 1;
        }

        std::string arg3(argv[3]);
        if (arg3 != "naive" && arg3 != "simpaccel"){
          std::cout << "Query Mode not correctly entered. Please select \'naive\' or \'simpaccel\'." << std::endl;
          Index.close();
          QueryFile.close();
          return 1;
        }

        std::string arg4(argv[4]);
        std::ofstream OutputFile(arg4);
        if (errno != 0){
          std::cout << "Output File Error: " << strerror(errno) << std::endl;
          Index.close();
          QueryFile.close();
          return 1;
        }

        
        int64_t refLen, k, pSize; 
        Index.read((char *) &refLen, sizeof(refLen));
        std::string ref;
        ref.resize(refLen);
        Index.read((char *) ref.c_str(), refLen);
        std::unique_ptr<int64_t[]> sa(new int64_t[refLen]);
        Index.read((char *) &sa[0], refLen*sizeof(sa[0]));
        Index.read((char *) &k, sizeof(k));

        std::vector<std::string> names;
        std::vector<std::string> queries;
        std::string tmp;
        std::string tmpRec = "";
        std::int64_t recCount = 0;
        while (getline(QueryFile, tmp)) {
          if ((!tmp.empty()) && (tmp.at(0) != '>')) {
            tmp.erase(std::remove_if(tmp.begin(), tmp.end(), ::isspace), tmp.end());
            tmpRec = tmpRec + tmp; 
            tmp.clear();
          }
          if ((!tmp.empty()) && (tmp.at(0) == '>')) { 
            if(recCount > 0){
                transform(tmpRec.begin(), tmpRec.end(), tmpRec.begin(), ::toupper);
                queries.push_back(tmpRec);
            }
            recCount = recCount + 1;
            tmpRec.clear();
            int pos = tmp.find(" ");
            std::string name = tmp.substr(1, pos-1);
            names.push_back(name);
            tmp.clear();
          }
        }

        if(!tmpRec.empty()){
            transform(tmpRec.begin(), tmpRec.end(), tmpRec.begin(), ::toupper);
            queries.push_back(tmpRec);
        }

        int64_t nSize = names.size();
        int64_t qSize = queries.size();

        pSize = int(pow(4,k));
        int64_t ps[pSize], pe[pSize];
        if (k != 0) {
            Index.read((char *) &ps[0], pSize*sizeof(ps[0]));
            Index.read((char *) &pe[0], pSize*sizeof(pe[0]));
        }

        auto begin = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(begin-begin); 
        for (size_t i = 0; i < qSize; ++i) {
            begin = std::chrono::high_resolution_clock::now();
            int64_t leftIndex, rightIndex;

            std::string check = queries[i];
            int64_t checkLen = queries[i].size();

            if (k == 0) {
                leftIndex = 0;
                rightIndex = refLen - 1; 
            } else {
                int64_t calc = 0; 
                if (checkLen < k){
                    leftIndex = 0;
                    rightIndex = refLen - 1;
                } else {
                    for (size_t i = 0; i < k; ++i){
                        if (check[i] == 'A'){
                            calc = calc + 0*int(pow(4, (k-1-i)));
                        } else if (check[i] == 'C'){
                            calc = calc + 1*int(pow(4, (k-1-i)));
                        } else if (check[i] == 'G'){
                            calc = calc + 2*int(pow(4, (k-1-i)));
                        } else {
                            calc = calc + 3*int(pow(4, (k-1-i)));
                        }
                    }
                    leftIndex = ps[calc];
                    rightIndex = pe[calc];
                }
            }
            if (leftIndex < 0 || rightIndex < 0){
                end = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
                std::cout << "Duration: " << duration.count() << " nanoseconds" << std::endl;
                int64_t count = 0; 
                if (i > 0){
                    OutputFile << "\n";
                }

                OutputFile << names[i];
                OutputFile << "\t";
                OutputFile << count;
            } else {
                if (arg3 == "naive"){
                    int64_t* test = binaryQuery(ref, (sa.get()), leftIndex, rightIndex, check, checkLen);
                    int64_t c = *test, l = *(test + 1), r = *(test + 2);

                    int64_t count;

                    if (c != -1){
                        int64_t* test3 = binaryQuery(ref, (sa.get()), l, c-1, check, checkLen);
                        int64_t m = *test3, ml = *(test3 + 1), mr = *(test3 + 2), left = -1;
                        while ((m) != -1){
                            left = m;
                            test3 = binaryQuery(ref, (sa.get()), ml, m-1, check, checkLen);
                            m = *test3, ml = *(test3 + 1), mr = *(test3 + 2);
                        }
                        
                        if (left == -1){
                            left = c;
                        }

                        int64_t* test4 = binaryQuery(ref, (sa.get()), c + 1, r, check, checkLen);
                        int64_t n = *test4, nl = *(test4 + 1), nr = *(test4 + 2), right = -1;
                        while ((n) != -1){
                            right = n;
                            test4 = binaryQuery(ref, (sa.get()), n + 1, nr, check, checkLen);
                            n = *test4, nl = *(test4 + 1), nr = *(test4 + 2);
                        }

                        if (right == -1) {
                            right = c; 
                        }

                        count = (right - left + 1);
                        end = std::chrono::high_resolution_clock::now();
                        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
                        std::cout << "Duration: " << duration.count() << " nanoseconds" << std::endl;
                        if (i > 0){
                            OutputFile << "\n";
                        }

                        OutputFile << names[i];
                        OutputFile << "\t";
                        OutputFile << count;
                        for (size_t j = 0; j < count; ++j){
                            OutputFile << "\t";
                            OutputFile << sa[(left+j)];
                        }
                    } else {
                        count = 0;
                        end = std::chrono::high_resolution_clock::now();
                        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
                        std::cout << "Duration: " << duration.count() << " nanoseconds" << std::endl;
                        if (i > 0){
                            OutputFile << "\n";
                        }

                        OutputFile << names[i];
                        OutputFile << "\t";
                        OutputFile << count;
                    }
                } else {
                    int64_t lLCP, rLCP;
                    lLCP = lcp(ref.substr(sa[leftIndex]), ref.substr(sa[leftIndex]).size(), check, checkLen);
                    rLCP = lcp(ref.substr(sa[rightIndex]), ref.substr(sa[rightIndex]).size(), check, checkLen);

                    int64_t * first = lcpBinaryQuery(ref, (sa.get()), leftIndex, lLCP, rightIndex, rLCP, check, checkLen);
                    int64_t c = *first, l = *(first + 1), r = *(first + 2);
                    int64_t fclcp = *(first + 3), fllcp = *(first + 4), frlcp =  *(first + 5);
                    
                    int64_t count;

                    if (c != -1){
                        int64_t cLCP = lcp(ref.substr(sa[c]), ref.substr(sa[c]).size(), check, checkLen);
                        int64_t* leftHit = lcpBinaryQuery(ref, (sa.get()), l, fllcp, c-1, fclcp, check, checkLen);
                        int64_t m = *leftHit, ml = *(leftHit + 1), mr = *(leftHit + 2), left = -1;
                        int64_t mclcp = *(leftHit + 3), mllcp = *(leftHit + 4), mrlcp =  *(leftHit + 5);
                        while ((m) != -1){
                            left = m;
                            leftHit = lcpBinaryQuery(ref, (sa.get()), ml, mllcp, m-1, mclcp, check, checkLen);
                            m = *leftHit, ml = *(leftHit + 1), mr = *(leftHit + 2);
                            mclcp = *(leftHit + 3), mllcp = *(leftHit + 4), mrlcp = *(leftHit + 5);
                        }
                        
                        if (left == -1){
                            left = c;
                        }

                        int64_t* rightHit = lcpBinaryQuery(ref, (sa.get()), c + 1, fclcp, r, frlcp, check, checkLen);
                        int64_t n = *rightHit, nl = *(rightHit + 1), nr = *(rightHit + 2), right = -1;
                        int64_t nclcp = *(rightHit + 3), nllcp = *(rightHit + 4), nrlcp =  *(rightHit + 5);
                        while ((n) != -1){
                            right = n;
                            rightHit = lcpBinaryQuery(ref, (sa.get()), n + 1, nclcp, nr, nrlcp, check, checkLen);
                            n = *rightHit, nl = *(rightHit + 1), nr = *(rightHit + 2);
                            nclcp = *(rightHit + 3), nllcp = *(rightHit + 4), nrlcp = *(rightHit + 5);
                        }

                        if (right == -1) {
                            right = c; 
                        }

                        count = (right - left + 1);
                        end = std::chrono::high_resolution_clock::now();
                        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
                        std::cout << "Duration: " << duration.count() << " nanoseconds" << std::endl;
                        if (i > 0){
                            OutputFile << "\n";
                        }

                        OutputFile << names[i];
                        OutputFile << "\t";
                        OutputFile << count;
                        for (size_t j = 0; j < count; ++j){
                            OutputFile << "\t";
                            OutputFile << sa[(left+j)];
                        }

                    } else {
                        count = 0;
                        end = std::chrono::high_resolution_clock::now();
                        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
                        std::cout << "Duration: " << duration.count() << " nanoseconds" << std::endl;
                        if (i > 0){
                            OutputFile << "\n";
                        }

                        OutputFile << names[i];
                        OutputFile << "\t";
                        OutputFile << count;
                    }
                }
            }
        }

        Index.close();
        QueryFile.close();
        OutputFile.close();
        
        
    } else {
        std::cout << "Argument count is wrong." << std::endl;
        return 1;
    }
    return 0;
}