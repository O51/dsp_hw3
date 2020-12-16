#include <cstring>
#include <fstream>
#include <stdio.h>
#include <map>
#include<vector>
#include<iostream>
using namespace std;
class disambig
{
    public:
        disambig();
        void resolve(const string mappath,const string modelpath);
    protected:
        void read_map(string mappath);
        void read_model(string modelpath);
        string resolve_line(int column,bool out);

        map<char*,vector<char*> > mapping;
        map<char*,vector<int> > model;
        
        //char* buffer_map;
        //char* buffer_model;



};
disambig::disambig()
{
    //buffer_map = new char[5e8];
    //buffer_model = new char[5e8];

};
void disambig:: read_map(string mappath)
{
    fstream file;
    file.open(mappath,ios::in);
    char buffer[5e8];
    string BUFFER;
    if(!file)
    {
        printf("open map fail\n");
    }
    else
    {
        file.read(buffer,sizeof(buffer));
        file.close();
    };
    BUFFER = buffer;
    buffer = "";
    printf(BUFFER[0] + '\n');
    
    
}
int main(int argc,char* argv[])
{
    disambig dsp_hw3;
    string mappath = argv[2];
    dsp_hw3.read_map(mappath);
}