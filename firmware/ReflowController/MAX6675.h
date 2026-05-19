#include <Arduino.h>

class MAX6675 {

  public:
    MAX6675(int _SCK, int _CS, int _SO);
    float readData(void);

  private:
    int SCK;
    int CS;
    int SO;

};