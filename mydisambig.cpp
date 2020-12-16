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
    printf("before open\n");
    file.open(mappath,ios::in);
    printf("before big\n");
    char buffer[int(5e8)];
    printf("after big\n");
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
    //buffer = "";
    printf(buffer);
    
    
}
int main(int argc,char* argv[])
{
    disambig dsp_hw3;
    printf("line60\n");
    for(int i=0;i<argc;i++)printf("%s\n",argv[i]);
    string mappath = argv[2];
    dsp_hw3.read_map(mappath);
}