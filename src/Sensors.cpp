#include "Sensors.h"

void setSwitchState(bool state)
{
    switch_state = state;
}

void setMaxLight(float value)
{
    // Avoid divide-by-zero in the NeoPixel brightness calculation.
    if (value <= 0.0f)
        value = 500.0f;

    max_light = value;
}
