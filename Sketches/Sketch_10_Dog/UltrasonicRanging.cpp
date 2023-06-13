/**
 * @file UltrasonicRanging.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Ultrasonic ranging driver and serive.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "UltrasonicRanging.h"

pthread_mutex_t mutexPin33;

void setupSonar()
{
    pthread_mutex_init(&mutexPin33, NULL); //Initialize mutex
}
float getSonar()
{
    unsigned long pingTime, t0, t1;
    float distance;

    pthread_mutex_lock(&mutexPin33); //Lock
    pinMode(PIN_TRIG, OUTPUT);  // set trigPin to output mode
    pinMode(PIN_ECHO, INPUT);   // set echoPin to input mode

    // make PIN_TRIG output high level lasting for 10μs to triger HC_SR04
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);
    // Wait HC-SR04 returning to the high level and measure out this waitting time
    t0 = micros();
    pingTime = pulseIn(PIN_ECHO, HIGH, timeOut); // unit: us
    t1 = micros() - t0;
    pinMode(PIN_TRIG, INPUT);     // set trigPin to input mode
    pthread_mutex_unlock(&mutexPin33); //Unlock

    // calculate the distance according to the time
    if (t1 < timeOut)
    {
        distance = (float)pingTime * soundVelocity / 2 / 10000;
    }
    else
    {
        distance = MAX_DISTANCE;
    }

    return distance; // return the distance value
}

unsigned long lastUpdateSonarTime = 0;
float dist = 0;
bool enableAutoWalking = false;
void setEnableAutoWalking(bool b)
{
    enableAutoWalking = b;
}
// task: Automatic Obstacle Avoidance Walking
void task_AutoWalking(void *pvParameters)
{
    // Serial.print("task_Sonared() function running on core: ");
    // Serial.println(xPortGetCoreID());
    // controlTask(TASK_AUTO_WALKING, TASK_SUSPEND);

    // while (1)
    {
        if (enableAutoWalking)
        {
            if (millis() - lastUpdateSonarTime > 200)
            {

                dist = getSonar();
                // bleSend(String(ACTION_ULTRASONIC) + "#" + String(dist) + "#\n");
                mqTx.enterForced(String(ACTION_ULTRASONIC) + "#" + String(dist) + "#\n");
                Serial.printf("dist: %.2f\n", dist);
                if (dist < 30)
                {
                    enterMessageQueue(String(ACTION_MOVE_ANY) + "#0#10#10#5#");
                }
                else
                {
                    enterMessageQueue(String(ACTION_MOVE_ANY) + "#0#20#0#20#");
                }
                // vTaskDelay(1000);
                lastUpdateSonarTime = millis();
            }
        }
    }
}