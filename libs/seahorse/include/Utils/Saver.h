#pragma once

#include <string>
class Saver {
public:
    Saver();
    ~Saver();


    
    void save(std::string path);

  private:
    
  char* m_store;
};