#ifndef QRSMESSAGE_H
#define QRSMESSAGE_H

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "nodeMCU.h"

#define LED_ON_TIME_MICROS 200000
#define ID 4
#define M 5
#define N 30
#define winSize 250 //is this 250 sample per second?
#define HP_CONSTANT ((float)1 / (float)M)
#define MAX_BPM 100
#define BPM_BUFFER_SIZE 5

// resolution of RNG
#define RAND_RES 100000000

class QRSMessage {
private:
    const long PERIOD = 1000000 / winSize; //4000microseconds is each step
    
    // circular buffer for BPM averaging
    float bpm = 0; 
    
    unsigned long bpm_buff[BPM_BUFFER_SIZE] = {0};
    int bpm_buff_WR_idx = 0;
    int bpm_buff_RD_idx = 0;
    
    int tmp = 0;
    int cnt = 0;
    float delta = 0;
    float z = 0.95;

    // timing variables
    unsigned long previousMicros = 0;      // will store last time LED was updated
    unsigned long foundTimeMicros = 0;     // time at which last QRS was found
    unsigned long old_foundTimeMicros = 0; // time at which QRS before last was found
    unsigned long currentMicros = 0;       // current time
    

    /* Portion pertaining to Pan-Tompkins QRS detection */

    // circular buffer for input ecg signal
    // we need to keep a history of M + 1 samples for HP filter
    float ecg_buff[M + 1] = {0};
    int ecg_buff_WR_idx = 0;
    int ecg_buff_RD_idx = 0;

    // circular buffer for input ecg signal
    // we need to keep a history of N+1 samples for LP filter
    float hp_buff[N + 1] = {0};
    int hp_buff_WR_idx = 0;
    int hp_buff_RD_idx = 0;

    // LP filter outputs a single point for every input point
    // This goes straight to adaptive filtering for eval
    float next_eval_pt = 0;

    // running sums for HP and LP filters, values shifted in FILO
    float hp_sum = 0;
    float lp_sum = 0;

    // working variables for adaptive thresholding
    float treshold = 0;
    boolean triggered = false;
    int trig_time = 0;
    float win_max = 0;
    int win_idx = 0;

    // numebr of starting iterations, used determine when moving windows are filled
    int number_iter = 0;

    int rawData;
    int data;
public:
    QRSMessage() { }
    boolean detect(float new_ecg_pt);
    void execute();
    int getRawData();
    int getData();
};

#endif