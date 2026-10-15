#ifndef MOTOR_H
#define MOTOR_H

class Motor {
    public:
        Motor(int ENA, int IN1, int IN2);
        void init();
        void setSpeed( float val);
        void stop();
    private:
        int enable;
        int input_1;
        int input_2;

};


#endif