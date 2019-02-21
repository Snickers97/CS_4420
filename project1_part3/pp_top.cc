//Warning: this code is very ugly
//It works but it's a mess to look at sorry
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sys/ioctl.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

using namespace std;

string inp(string piddir, double &percent_c, double &percent_m, int &rcount);
double cpu_calc(string str[52]);
double mem_calc(string str[52]);
void pairsort(vector<string> &a,vector<string> &b, int n, int rows);
void pairsort_cpu(vector<double> &a, vector<string> &b, int n, int rows);

int main(int argc, char **argv){
    if(argc != 2){
        cout<<"Usage: ./pp_ps [-cpu|-mem|-pid|-com]\n";
        return 0;
    }
    string input = argv[1];
    if(input != "-cpu" && input != "-mem" && input != "-pid" && input != "-com"){
        cout<<"Usage: ./pp_ps [-cpu|-mem|-pid|-com]\n";
        return 0;
    }
    struct winsize wsize;
    while(1){
        clear();
        gotoxy(0,0);
        ioctl(STDOUT_FILENO,TIOCGWINSZ,&wsize);
        int rows = wsize.ws_row - 3;
        long pid;
        vector<string> output;
        vector<double> percent_c;
        vector<double> percent_m;
        int i = 0;
        int rcount = 0;
        double cpucount = 0;
        double usedcount = 0;
        struct dirent *ep;
        DIR *dp;
        dp = opendir ("/proc");
        if (dp != NULL){
            while (ep = readdir (dp)){
                pid = strtol(ep->d_name, NULL, 10);
                if( ( ep->d_type == DT_DIR ) && ( pid > 0) ){
                    //printf("directory name: %s\n", ep->d_name);
                    // Do something with the proc directory
                    string piddir = string(ep->d_name);
                    percent_c.push_back(0);
                    percent_m.push_back(0);
                    output.push_back(inp(piddir, percent_c[i], percent_m[i], rcount));
                    cpucount+= percent_c[i];
                    usedcount += percent_m[i];
                    i++;
                }
                //closedir(dp);
                //This line gave me trouble so I commented it out
                //The program works fine without it for now
            }
        }
        else{
            perror("Couldn't open the directory");
            exit(-1);
        }
        long phys_pages, phys_memsize, pagesize;
        phys_pages = sysconf(_SC_PHYS_PAGES);
        pagesize = sysconf(_SC_PAGE_SIZE);
        phys_memsize = phys_pages * pagesize;
        long memused = usedcount*phys_memsize/100;
        memused = memused/1024; //matches the reference program
        cout<<i<<" total processes | ";
        if(rcount == 1){
            cout<<rcount<<" process running | ";
        }
        else{
            cout<<rcount<<" processes running | ";
        }
        cout<<phys_memsize<<" physical memory | "<<memused<<" memory used | "<<cpucount<<" total % CPU\n";
        cout<<"PID           Command                 State    %CPU         %MEM       VSZ            RSS            Core"<<endl;
        int n = output.size();
        if(input == "-pid"){
            for(int i = 0; i < rows; i++){
                cout<<output[i]<<endl;
            }
        }
        else if(input == "-com"){
            vector<string> output2 = output;
            for(int i = 0; i < output.size(); i++){
                output2[i] = output[i].substr(14, output.size());
            }
            pairsort(output2, output, n, rows);
        }
        else if(input == "-cpu"){
            pairsort_cpu(percent_c, output, n, rows);
        }
        else if(input == "-mem"){
            pairsort_cpu(percent_m, output, n, rows);
        }
        usleep(1000000);
    }
    return 0;
}

string inp(string piddir, double &percent_c, double &percent_m, int &rcount){
    ifstream in;
    in.open("/proc/" + piddir + "/stat");
    if(!in){
        cout<<"Error: unable to open stat file\n";
        exit(-1);
    }
    string str[52];
    in>>str[0];
    in>>str[1];
    //I had touble with strtok so I just did this manually
    //Handles the case where the command contains no spaces
    if(str[1][str[1].size()-1] == ')'){
        str[1] = str[1].substr(1,str[1].size()-2);
    }
    //Handles the case where the command contains a space
    else{
        in>>str[2];
        str[1] = str[1]+ ' '+ str[2];
        str[1] = str[1].substr(1,str[1].size()-2);
    }
    for(int i = 2; i < 52; i++){
        in>>str[i];
    }
    if(str[2] == "R"){
        rcount++;
    }
    in.close();
    percent_c = cpu_calc(str);
    percent_m = mem_calc(str);
    string output = str[0];
    for(int i = 0; i < 14-str[0].size(); i++){
        output+=' ';
    }
    output+=str[1];
    for(int i = 0; i < 24-str[1].size(); i++){
        output+=' ';
    }
    output+=str[2] + "        " + to_string(percent_c).substr(0,5);
    output+="        ";
    output+=to_string(percent_m).substr(0,5);
    output+="      ";
    output+=str[22];
    for(int i = 0; i < 15 - str[22].size(); i++){
        output+=' ';
    }
    output+=str[23];
    for(int i = 0; i < 15 - str[23].size(); i++){
        output+=' ';
    }
    output+=str[38];
    return output;
}

//Based on the formula in the handout
double cpu_calc(string str[52]){
    ifstream in;
    double utime, stime, process_time, starttime, uptime, realtime, percent_c;
    utime = stod(str[13]);
    stime = stod(str[14]);
    process_time = utime/sysconf(_SC_CLK_TCK) + stime/sysconf(_SC_CLK_TCK);
    starttime = stod(str[21]);
    in.open("/proc/uptime");
    if(!in){
        cout<<"Error: unable to open uptime file\n";
        exit(-1);
    }
    in>>uptime;
    in.close();
    realtime = uptime - (starttime/sysconf(_SC_CLK_TCK));
    percent_c = process_time*100/realtime;
    return percent_c;
}

//Based on handout
double mem_calc(string str[52]){
    double rss, percent_m;
    long phys_pages, pagesize, phys_memsize;
    rss = stod(str[23]);
    phys_pages = sysconf(_SC_PHYS_PAGES);
    pagesize = sysconf(_SC_PAGE_SIZE);
    phys_memsize = phys_pages * pagesize;
    percent_m = (rss * pagesize * 100) / phys_memsize;
    return percent_m;
}


void pairsort(vector<string> &a,vector<string> &b, int n, int rows) {
    pair<string, string> pairt[n];
    // Storing the respective array
    // elements in pairs.
    for (int i = 0; i < n; i++)  {
        pairt[i].first = a[i];
        pairt[i].second = b[i];
    }

    // Sorting the pair array.
    sort(pairt, pairt + n);

    // Modifying original arrays
    for (int i = 0; i < rows; i++)  {
        a[i] = pairt[i].first;
        b[i] = pairt[i].second;
        cout<<b[i]<<endl;
    }
}

void pairsort_cpu(vector<double> &a, vector<string> &b, int n, int rows){
    pair<double, string> pairt[n];
    // Storing the respective array
    // elements in pairs.
    for (int i = 0; i < n; i++)  {
        pairt[i].first = a[i];
        pairt[i].second = b[i];
    }

    // Sorting the pair array.
    sort(pairt, pairt + n);

    // Modifying original arrays
    for (int i = n-1; i>=n-rows ; i--)  {
        a[i] = pairt[i].first;
        b[i] = pairt[i].second;
        cout<<b[i]<<endl;
    }
}
