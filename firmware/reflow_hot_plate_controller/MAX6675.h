#include <Arduino.h>

class CY_MAX6675 {

  public:
    CY_MAX6675(int _SCK, int _CS, int _SO);
    float readData(void);

  private:
    int SCK;
    int CS;
    int SO;

};