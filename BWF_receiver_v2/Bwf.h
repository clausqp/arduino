

#include "Arduino.h"


class Bwf {
  public:
    Bwf();
    void Init( Stream *out );

    bool CheckEvents();

    bool IsInside();
    bool IsOutside();
    bool IsError();

    int insideCnt;
    int outsideCnt;
    int crapCnt;

    int state;

  private:
    Stream *bwfSerial;
    bool isInside;
    bool isOutside;
    bool isError;
    long lastSeenInside;
    long lastSeenOutside;
    
};

