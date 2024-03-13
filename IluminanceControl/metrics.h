#ifndef METRICS_H
#define METRICS_H

const float LED_MAX_POWER {0.108}; //Watts

class Metrics
{
    double Energy, VisibilityError, FlickerError;
    double N;
    float get_f_k();
    public:
    float y_registry[6000];
    int u_registry[6000];
    int u_reg_pointer;
    int y_reg_pointer;
    explicit Metrics();
    ~Metrics();
    //void update_Energy(double d, double elapsedTime);
    //void update_VisibilityError(float error);
    void update_FlickerError();
    void increment_N();
    //double getEnergy();
    //double getVisibilityError();
    double getFlickerError();
    void update_u_registry(int value);
    void update_y_registry(float value);
};

inline Metrics::Metrics() : u_reg_pointer {0}, y_reg_pointer {0}, Energy {0.0}, VisibilityError {0.0}, FlickerError {0.0}, N {1}
{   
    // Inicializes all variables
    for(int i = 0; i < 6000; i++)
    {
        u_registry[i] = 0;
    }
    
    for(int i = 0; i < 6000; i++)
    {
        y_registry[i] = 0.0;
    }
}

inline float Metrics::get_f_k()
{
    const int DAC_RANGE {4095};
    float dk[3] = {0.0,0.0,0.0};

    dk[0] = u_registry[u_reg_pointer]/(float)DAC_RANGE;

    if(u_reg_pointer-1 < 0) dk[1] = u_registry[5999]/(float)DAC_RANGE;
    else dk[1] = u_registry[u_reg_pointer-1]/(float)DAC_RANGE;

    if(u_reg_pointer-2 < 0)
    {
        if(u_reg_pointer == 1) dk[2] = u_registry[5999]/(float)DAC_RANGE;
        else dk[2] = u_registry[5998]/(float)DAC_RANGE;
    }
    else dk[2] = u_registry[u_reg_pointer-2]/(float)DAC_RANGE;

    if((dk[0] - dk[1]) * (dk[1] - dk[2]) < 0) return abs(dk[0] - dk[1]) + abs(dk[1] - dk[2]);

    return 0;

}


inline void Metrics::update_FlickerError()
{
    FlickerError = ((float)1 - 1/N)*FlickerError + get_f_k();
}

inline void Metrics::increment_N()
{
    N +=1.0;
}


inline double Metrics::getFlickerError()
{
    return FlickerError;
}

inline void Metrics::update_u_registry(int value)
{
    if(u_reg_pointer >= 6000) u_reg_pointer = 0;
    u_registry[u_reg_pointer] = value;
    u_reg_pointer += 1;
}

inline void Metrics::update_y_registry(float value)
{
    if(y_reg_pointer >= 6000) y_reg_pointer = 0;
    y_registry[y_reg_pointer] = value;
    y_reg_pointer += 1;
}

#endif