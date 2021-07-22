#include "logReader.hpp"
#ifndef debug
#define debug 0

auto match = [](std::string target, std::string pattern) {
  size_t size = pattern.size();
  return (target.size() >= size && target.substr(0, size) == pattern);
};

void replace(std::string& s, const char& target, const char& t) {
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == target)
      s[i] = t;
  }
}

readFlag logReader::checker(std::string s) {
  if (match(s, "Created orthogonal box")) {
    // read box info
    return readFlag::BOX;
  } else if (match(s, "Per MPI rank memory allocation")) {
    // read thermo info
    return readFlag::THERMO;
  } else if (match(s, "Lattice spacing")) {
    //read lattice spacing
    return readFlag::SPACING;
  } else if (match(s, "Performance")) {
    //read performance info
    return readFlag::PERFORMANCE;
  } else if (s.size() >= 15 && s.substr(0, 7) == "Created" && s.substr(s.size() - 5, 5) == "atoms") {
    return readFlag::ATOMNUMBER;
  }
  return readFlag::NONE;
};

void logReader::read(std::string fileName) {
#if debug
  std::cout << "reading file " << fileName << std::endl;
#endif

  auto internal_flag = readFlag::NONE;
  size_t numberOfRun = 0;

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
  std::string tmp, dummy;

  // useful lambda function
  // skip lines in file
  auto skipLines = [&file, &line](int i) {
    while (i-- > 0)
      getline(file, line);
  };

  // get string into ss
  auto ssGet = [&file, &line, &ss]() {
    getline(file, line);
    // reset buf as empty
    ss.str("");
    // clear stringstream state
    ss.clear();
    // input new string line
    ss << line;
    // using sstream is quite tricky. above is the way to rewrite buffer
  };

  // get string into ss with replace
  auto ssGetReplace = [&file, &line, &ss](const char& s, const char& t) {
    getline(file, line);
    // reset buf as empty
    ss.str("");
    // clear stringstream state
    ss.clear();
    // input new string line
    replace(line, s, t);
    ss << line;
    // using sstream is quite tricky. above is the way to rewrite buffer
  };

  // main loop for data input
  while (std::getline(file, tmp)) {
    internal_flag = checker(tmp);
    if (internal_flag == readFlag::BOX) {
#if debug
      std::cout << "reading box info from " << fileName << std::endl;
#endif
      // check if current run is added
      if (this->getSize() < numberOfRun + 1) {
        this->runs.push_back(new thermo);
      }
      // read box info
      ss.str("");
      ss.clear();
      for (size_t i = 0; i < tmp.size(); i++) {
        if (tmp[i] == '(') {
          tmp[i] = ' ';
        } else if (tmp[i] == ')') {
          tmp[i] = ' ';
        }
      }
      ss << tmp;
      size_t index = 0;

      while (index < 6) {
        ss >> this->runs.back()->box[index];
        if (ss.fail()) {
          ss.clear();
          ss >> dummy;
        } else {
          index += 1;
        }
      }

      this->runs.back()->Box_flag = true;
    } else if (internal_flag == readFlag::THERMO) {
#if debug
      std::cout << "reading thermo info from " << fileName << std::endl;
#endif
      // read thermo info
      // std::string
      size_t index = 0;
      ssGet();
      // check if current run is added
      if (this->getSize() < numberOfRun + 1) {
        this->runs.push_back(new thermo);
      }
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
      this->runs.back()->Thermo_flag = true;
    } else if (internal_flag == readFlag::ATOMNUMBER) {
#if debug
      std::cout << "reading particle info from " << fileName << std::endl;
#endif
      // check if current run is added
      if (this->getSize() < numberOfRun + 1) {
        this->runs.push_back(new thermo);
      }

      ss.str("");
      ss.clear();
      ss << tmp;
      ss >> this->runs.back()->particleN;
      while (ss.fail()) {
        ss.clear();
        ss >> dummy;
      }
      this->runs.back()->particleN = true;
    } else if (internal_flag == readFlag::SPACING) {
#if debug
      std::cout << "reading lattice info from " << fileName << std::endl;
#endif
      // check if current run is added
      if (this->getSize() < numberOfRun + 1) {
        this->runs.push_back(new thermo);
      }
      ss.str("");
      ss.clear();
      ss << tmp;
      size_t index = 0;
#if debug
      std::cout << "parsing line is: " << tmp << std::endl;
#endif
      while (index < 3) {
        ss >> this->runs.back()->lattice[index];
        if (ss.fail()) {
          ss.clear();
          ss >> dummy;
        } else {
#if debug
          std::cout << "insert: " << this->runs.back()->lattice[index] << std::endl;
#endif
          index += 1;
        }
      }
      this->runs.back()->lattice_flag = true;
    } else if (internal_flag == readFlag::PERFORMANCE) {
#if debug
      std::cout << "reading performance info from " << fileName << std::endl;
#endif
      // check if current run is added
      if (this->getSize() < numberOfRun + 1) {
        this->runs.push_back(new thermo);
      }

      ss.str("");
      ss.clear();
      ss << tmp;

      // read general performance info

      {
#if debug
        std::cout << "1" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        size_t index = 0;
        while (index < 2) {
          ss >> this->runs.back()->performance[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      // read run general cpu info
      {
        std::getline(file, tmp);
        size_t index = 0;
        for (size_t i = 0; i < tmp.size(); i++)
          if (tmp[i] == '%')
            tmp[i]
                = ' ';
        ss.str("");
        ss.clear();
        ss << tmp;
#if debug
        std::cout << "2" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        while (index < 3) {
          ss >> this->runs.back()->cpu[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      // read tablur info
      skipLines(4);

      // read Pair info
      {
        size_t index = 0;
        ssGetReplace('|',' ');
#if debug
        std::cout << "3" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        while (index < 5) {
          ss >> this->runs.back()->Pair[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      // read Neigh info
      {
        size_t index = 0;
        ssGetReplace('|',' ');;
#if debug
        std::cout << "4" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        while (index < 5) {
          ss >> this->runs.back()->Neigh[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      // read Comm info
      {
        size_t index = 0;
        ssGetReplace('|',' ');;
#if debug
        std::cout << "5" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        while (index < 5) {
          ss >> this->runs.back()->Comm[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      // read Output info
      {
        size_t index = 0;
        ssGetReplace('|',' ');;
#if debug
        std::cout << "6" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        while (index < 5) {
          ss >> this->runs.back()->Output[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      // read Modify info
      {
        size_t index = 0;
        ssGetReplace('|',' ');;
#if debug
        std::cout << "7" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        while (index < 5) {
          ss >> this->runs.back()->Modify[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      // read Other info
      {
        size_t index = 0;
        ssGetReplace('|',' ');;
#if debug
        std::cout << "8" << std::endl;
        std::cout << "parsing:" << ss.str() << std::endl;
#endif
        while (index < 2) {
          ss >> this->runs.back()->Other[index];
          if (!ss.fail()) {
            index += 1;
          } else {
            ss.clear();
            ss >> dummy;
          }
        }
      }
      this->runs.back()->performance_flag = true;
      //after reading performance add number of run by 1
      numberOfRun += 1;
    }
  }  // end of reading file
};
#endif