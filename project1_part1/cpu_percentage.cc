#include <iostream>
#include <unistd.h>
#define TIME_DELAY 50000

using namespace std;

int main(int argc, char **argv){
    int user_percentage;
    //User-proofing the input argument
    if(argc != 2)
        cin>>user_percentage;
    else
        user_percentage = atoi(argv[1]);
    if(!(user_percentage >= 1 && user_percentage <= 100)){
        cout<<"Error: invalid argument.\n";
        cout<<"Usage: ./cpu_percentage <percentage>\n";
        return 0;
    }
    while(1){
        clock_t time1, time2, diff;
        time1 = clock();
        while(1){
            time2 = clock();
            diff = (time2-time1);
            if(diff > TIME_DELAY)
                break;
        }
        int sleep_time = (TIME_DELAY*(100 - user_percentage))/user_percentage;
        usleep(sleep_time);
    }
}
