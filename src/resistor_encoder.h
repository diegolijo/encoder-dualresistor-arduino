/* #define RESISTOR_ENCODE_H
#ifdef RESISTOR_ENCODE_H
struct EncoderResistivo
{
    void updateEncoder(void);
};
#endif */

#include <Arduino.h>
#include <Control_Surface.h>

struct EncoderResistivo
{

    const static int MIN_OPERATIVE_VALUE = 7 * 8;
    const static int MAX_OPERATIVE_VALUE = 119 * 8;
    const static int MIN_STEP_CHANGE = 1 * 8;
    const static int MAX_STEP_CHANGE = 16 * 8;
    const static int MIN_TOTAL_CHANGE = 7;
    const static int MAX_TOTAL_CHANGE = 64 * 8;

    const static int REFRESH_TIME = 20;
    const static int VECTOR_LENGHT = 5;
    const static int SEND_FACTOR = 2;

    unsigned long lastTime = millis();

    int arrayValues[VECTOR_LENGHT] = {-1, -1, -1, -1, -1};
    int changes[VECTOR_LENGHT] = {0, 0, 0, 0, 0};
    int totalChanges = 0;
    boolean aleatory = false;
    boolean stop = false;

    int arrayValues1[VECTOR_LENGHT] = {-1, -1, -1, -1, -1};
    int changes1[VECTOR_LENGHT] = {0, 0, 0, 0, 0};
    int totalChanges1 = 0;
    boolean aleatory1 = false;
    boolean stop1 = false;

    int sendCount = 0;

    int getTotalChanges(int *changesArr)
    {
        int changes = 0;
        for (int i = VECTOR_LENGHT - 1; i >= 0; i--)
        {
            changes += changesArr[i];
        }
        return changesArr[0] + changesArr[1] + changesArr[2] + changesArr[3] + changesArr[4];
    }

    void updateVector(int value, int *valuesArr, int *changesArr, boolean &random, int &total)
    {
        int up = false;
        int down = false;
        for (int i = VECTOR_LENGHT - 1; i >= 0; i--)
        {
            valuesArr[i] = i != 0 ? valuesArr[i - 1] : value;
            changesArr[i] = i != 0 ? changesArr[i - 1] : valuesArr[0] - valuesArr[1];
            if (changesArr[i] > 0)
            {
                up = true;
            }
            if (changesArr[i] < 0)
            {
                down = true;
            }
        }
        if (up == true && down == true)
        {
            random = true;
        }
        else
        {
            random = false;
        }
        total = getTotalChanges(changesArr);
    }

    void updateEncoder(USBMIDI_Interface *midiusb, MIDIAddress controller)
    {

        if (abs(millis() - lastTime) > REFRESH_TIME)
        {
            int value = 0;
            int value1 = 0;
            updateVector(analogRead(A1), arrayValues1, changes1, aleatory1, totalChanges1);
            updateVector(analogRead(A0), arrayValues, changes, aleatory, totalChanges);
            if (abs(changes1[0]) > 0 && arrayValues1[0] > MIN_OPERATIVE_VALUE && arrayValues1[0] < MAX_OPERATIVE_VALUE)
            {
                if (abs(totalChanges1) > MIN_TOTAL_CHANGE && abs(totalChanges1) < MAX_TOTAL_CHANGE && abs(changes1[0]) < MAX_STEP_CHANGE && !aleatory1 && arrayValues1[0] != arrayValues1[1])
                {
                    stop1 = true;
                    /* Serial << F("** Values 1: ") << arrayValues1[0] << F(", ") << arrayValues1[1] << F(", ") << arrayValues1[2] << F(", ") << arrayValues1[3] << F(", ") << arrayValues1[4]
                           << F("  -  changes: ") << changes1[0] << F(", ") << changes1[1] << F(", ") << changes1[2] << F(", ") << changes1[3] << F(", ") << changes1[4]
                           << F("  -  totalChanges: ") << totalChanges1 << F("  -  aleatory: ") << aleatory1 << endl; */

                    /*  Serial << F("1  totalChanges: ") << totalChanges1 << F("  -  aleatory: ") << aleatory1 << endl;*/

                    value1 = changes1[0];
                }
                else if (aleatory1 && stop1)
                {
                    stop1 = false;
                    Serial << F("1 -------------------------------------------------------------------Values 1: ") << arrayValues1[0] << F(", ") << arrayValues1[1] << F(", ") << arrayValues1[2] << F(", ") << arrayValues1[3] << F(", ") << arrayValues1[4]
                           << F("  -  changes: ") << changes1[0] << F(", ") << changes1[1] << F(", ") << changes1[2] << F(", ") << changes1[3] << F(", ") << changes1[4]
                           << F("  -  totalChanges: ") << totalChanges1 << F("  -  aleatory: ") << aleatory1 << endl;
                }
            }
            if (abs(changes[0]) > 0 && arrayValues[0] > MIN_OPERATIVE_VALUE && arrayValues[0] < MAX_OPERATIVE_VALUE)
            {
                if (abs(totalChanges) > MIN_TOTAL_CHANGE && abs(totalChanges) < MAX_TOTAL_CHANGE && abs(changes[0]) < MAX_STEP_CHANGE && !aleatory && arrayValues[0] != arrayValues[1])
                {
                    stop = true;
                    /* Serial << F("++ Values 0: ") << arrayValues[0] << F(", ") << arrayValues[1] << F(", ") << arrayValues[2] << F(", ") << arrayValues[3] << F(", ") << arrayValues[4]
                           << F("  -  changes: ") << changes[0] << F(", ") << changes[1] << F(", ") << changes[2] << F(", ") << changes[3] << F(", ") << changes[4]
                           << F("  -  totalChanges: ") << totalChanges << F("  -  aleatory: ") << aleatory << endl; */

                    /*    Serial << F("0  totalChanges: ") << totalChanges << F("  -  aleatory: ") << aleatory << endl; */

                    value = changes[0];
                }
                else if (aleatory && stop)
                {
                    stop = false;
                    Serial << F("0 -------------------------------------------------------------------Values 0: ") << arrayValues[0] << F(", ") << arrayValues[1] << F(", ") << arrayValues[2] << F(", ") << arrayValues[3] << F(", ") << arrayValues[4]
                           << F("  -  changes: ") << changes[0] << F(", ") << changes[1] << F(", ") << changes[2] << F(", ") << changes[3] << F(", ") << changes[4]
                           << F("  -  totalChanges: ") << totalChanges << F("  -  aleatory: ") << aleatory << endl;
                }
            }

            if (abs(value) > 0 && abs(value1) > 0 && abs(value) == abs(value1) && ((sendCount + SEND_FACTOR) % SEND_FACTOR == 0 ? true : false))
            {
                midiusb->sendCC(controller, value1 > 0 ? 1 : -1);
                Serial << F("1 values:  ") << value1 << F("  -  ") << value1 << endl;
            }
            else if (abs(value) > 0 && value1 == 0 && ((sendCount + SEND_FACTOR) % SEND_FACTOR == 0 ? true : false))
            {
                midiusb->sendCC(controller, value > 0 ? 1 : -1);
                Serial << F("0 values:  ") << value << F("  -  ") << value1 << endl;
            }
            else if (abs(value1) > 0 && value == 0 && ((sendCount + SEND_FACTOR) % SEND_FACTOR == 0 ? true : false))
            {
                midiusb->sendCC(controller, value1 > 0 ? 1 : -1);
                Serial << F("1 values:  ") << value << F("  -  ") << value1 << endl;
            }

            sendCount += 1;

            lastTime = millis();
        }
    }
};

/* #include <Arduino.h>
#include <Control_Surface.h>
#include "resistor_encoder.h"

class EncoderResistivo
{

    const int MIN_OPERATIVE_VALUE1 = 60;
    const int MAX_OPERATIVE_VALUE1 = 960;
    const int MIN_OPERATIVE_VALUE0 = 60;
    const int MAX_OPERATIVE_VALUE0 = 960;
    const int MIN_DIFF_VALUES = 400;
    const int MIN_DIFF_READINGS = 0;
    const int MAX_DIFF_READINGS = 9;
    const int OVERFLOW_DIFF_READINGS = 64;
    const int OVERFLOW_DIFF = 300;
    const int REFRESH_TIME = 5;

    const static int VECTOR_LENGHT = 5;

    // int lastCADValue0 = analogRead(A0);
    int lastCADValue1 = analogRead(A1);

    int values0[EncoderResistivo::VECTOR_LENGHT] = {-1, -1,-1, -1, -1};
    unsigned long lastTime = millis();

    int lastCADOverflow0 = analogRead(A0);
    boolean overflow0 = false;

    void updateVectorTo7bit(int value)
    {
        for (int i = VECTOR_LENGHT - 1; i >= 0; i--)
        {
            values0[i] = i != 0 ? values0[i - 1] : value / 8;
        }
    }

    int getTotalChanges()
    {
        int changes = 0;
        for (int i = 0; i < VECTOR_LENGHT - 1; i++)
        {
            changes += values0[i] - values0[i + 1];
        }
        return changes;
    }

    boolean goInSameDirection(int totalChanges) // TODO
    {
        int up = 0;
        int down = 0;
        for (int i = 0; i < VECTOR_LENGHT - 1; i++)
        {

            if (values0[i] - values0[i + 1] > 2) // clockwise : !
            {
                up += 1;
                // values0[i] = 0;
                // Serial << F("direction ") << values0[i] << endl;
            }
            else if (values0[i] - values0[i + 1] < -2)
            {
                down += 1;
            }
        }
        if (up > 0 && down > 0)
        {
            Serial << F("goInSameDirection: ") << values0[0] << F(" , ") << values0[1] << F(" , ") << values0[2] << F(" , ") << values0[3] << F(" , ") << values0[4] << endl;
            return false;
        }

        return true;
    }

    void updateEncoder()
    {
        if (abs(millis() - lastTime) > REFRESH_TIME)
        {
            updateVectorTo7bit(analogRead(A0));
            int totalChanges = getTotalChanges();
            if (abs(totalChanges) > 1)
            {
                // Serial << F("totalChanges ") << totalChanges << endl;
                boolean changeDirection = goInSameDirection(totalChanges);
                if (abs(totalChanges) < 64 && !overflow0 && goInSameDirection(totalChanges)) // TODO comprobar que los 5 cambios del vector van en la misma dirección
                {

                    Serial << F("Values0: ") << values0[0] << F(" - ") << values0[1] << F(" - ") << values0[2] << F(" - ") << values0[3] << F(" - ") << values0[4] << F(" - ") << F("  -  change:") << totalChanges << endl;
                }
                else
                {
                    overflow0 = true;
                    Serial << F("                                 Values0: ") << values0[0] << F(" - ") << values0[1] << F(" - ") << values0[2] << F(" - ") << values0[3] << F(" - ") << values0[4] << F("  -  change:") << totalChanges << endl;
                }

                if (overflow0 && abs(totalChanges) < 32 && values0[0] > MIN_OPERATIVE_VALUE0 && values0[0] < MAX_OPERATIVE_VALUE0)
                {
                    overflow0 = false;
                    Serial << F("******* rearmado ****** ") << values0[0] << F(" - ") << values0[1] << F(" - ") << values0[2] << F(" - ") << values0[3] << F(" - ") << values0[4] << F("  -  change:") << totalChanges << endl;
                }
            }

            // ******************************** overflow0 ************************************
                      lastCADOverflow0 = value0;
                      if (lastCADOverflow0 < MIN_OPERATIVE_VALUE0 && value0 > OVERFLOW_DIFF && !overflow0)
                      {
                          Serial << F("overflow0up");
                          overflow0 = true;
                      }
                      if (lastCADOverflow0 > MAX_OPERATIVE_VALUE0 && value0 > OVERFLOW_DIFF && !overflow0)
                      {
                          Serial << F("overflow0down");
                          overflow0 = true;
                      }
                      //*********************************** CAD0 ***************************************
                      if (MIN_OPERATIVE_VALUE0 < value0 && value0 < MAX_OPERATIVE_VALUE0 && MIN_DIFF_VALUES < abs(value0 - value1))
                      {
                          int diff0 = (value0 - lastCADValue0) / 8;
                          if (abs(diff0) > OVERFLOW_DIFF_READINGS)
                          {
                              lastCADValue0 = value0;
                              lastCADValue1 = value1;
                          }
                          if (MIN_DIFF_READINGS < abs(diff0) && abs(diff0) < OVERFLOW_DIFF_READINGS)
                          {
                              if (diff0 < MAX_DIFF_READINGS)
                              {
                                  Serial << F("value0: ") << value0 << F(" - value1: ") << value1 << F(" - lastCADValue1: ") << lastCADValue1 << F("  -  change:") << diff0 << endl;
                                  overflow0 = true;
                              }

                              lastCADValue0 = value0;
                              lastTime = millis();
                              return;
                          }
                      }
                      //*********************************** CAD1 ***************************************
                      if (MIN_OPERATIVE_VALUE1 < value1 && value1 < MAX_OPERATIVE_VALUE1 && MIN_DIFF_VALUES < abs(value0 - value1))
                      {
                          int diff1 = (value1 - lastCADValue1) / 8;
                          if (abs(diff1) > OVERFLOW_DIFF_READINGS)
                          {
                              lastCADValue0 = value0;
                              lastCADValue1 = value1;
                          }
                          if (MIN_DIFF_READINGS < abs(diff1) && abs(diff1) < OVERFLOW_DIFF_READINGS)
                          {
                              if (diff1 < MAX_DIFF_READINGS)
                              {
                                  Serial << F("value1: ") << value1 << F(" - value0: ") << value0 << F(" - lastCADValue0: ") << lastCADValue0 << F("  -  change:") << diff1 << endl;
                              }
                              lastCADValue1 = value1;
                          }
                      }
            lastTime = millis();
        }
    }
};
 */