#include "logReader.hpp"

#define debug 0

int logReader::checker(std::string s) {
  if (s.size() >= 22 && s.substr(0, 22) == "Created orthogonal box") {
    // read box info
    return 1;
  } else if (s.size() >= 30 && s.substr(0, 30) == "Per MPI rank memory allocation") {
    // read thermo info
    return 2;
  }
  return -1;
};

void logReader::read(std::string fileName) {
#if debug
  std::cout << "reading file " << fileName << std::endl;
#endif

  int internal_flag = -1;
  std::ifstream file(fileName, std::ios_base::in);

  if (file.fail()) {
    throw std::system_error(errno, std::system_category(), "failed to open " + fileName);
  } else {
#if debug
    std::cout << "reading file " << fileName << std::endl;
#endif
  }

  // a temp string for storing lines
  std::string line;
  std::stringstream ss;
  std::string tmp;

  // useful lambda function
  // skip lines in file
  auto skipLines = [&file, &line](int i) {
    while (i-- > 0)
      getline(file, line);
  };

  // get string into ss
  auto ssGet = [&file, &line, &ss]() {
    getline(file, line);
    ss.str("");
    ss.clear();
    ss << line;
    // using sstream is quite tricky. above is the way to rewrite buffer
  };

  // main loop for data input
  while (std::getline(file, tmp)) {
    internal_flag = checker(tmp);
    if (internal_flag == 1) {
      // read box info

    } else if (internal_flag == 2) {
      // read thermo info
      // std::string
      size_t index = 0;
      ssGet();
      this->runs.push_back(new thermo);
      runs.back()->attr_order = line;
      while (std::getline(ss, tmp, ' ')) {
        this->runs.back()->attr_index[tmp] = index++;
      }
      // index is the # of attrs
      this->runs.back()->attr_table.resize(index);
#if debug
      std::cout << "attrs are" << line << std::endl;
#endif
      bool notSwitch = true;
      double d_tmp;
      while (getline(file, line) && notSwitch) {
        std::stringstream tmp(line);
        for (size_t i = 0; i < index; i++) {
          tmp >> d_tmp;
          // https://stackoverflow.com/questions/24504582/how-to-test-whether-stringstream-operator-has-parsed-a-bad-type-and-skip-it
          if (tmp.fail()) {
            notSwitch = false;
            for (size_t j = 0; j < i; j++) {
              // if some run get a corrupted line, pop out those value
              this->runs.back()->attr_table[j].pop_back();
            }
            break;
          }
          this->runs.back()->attr_table[i].push_back(d_tmp);
        }
      }

    }  //end of read thermo data

  }  // end of reading file
};
