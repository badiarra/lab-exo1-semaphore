Motor::Motor(int ENA, int IN1, int IN2){
    enable = ENA;
    input_1 = IN1;
    input_2 = IN2;
}

void Motor::init(){
    pinMode(enable, OUTPUT);
    pinMode(input_1, OUTPUT);
    pinMode(input_2, OUTPUT);

    digitalWrite(input_1, HIGH);
    digitalWrite(input_2, LOW);
     analogWrite(enable,0);
}

void setSpeed(float value){
    digitalWrite(input_1, HIGH);
    digitalWrite(input_2, LOW);
    analogWrite(enable, constrain(value, 0, 255));
}

void Motor::stop(){
     analogWrite(enable,0);
}