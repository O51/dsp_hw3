#include <cstring>
#include <fstream>
#include <stdio.h>
#include <map>
#include<vector>
#include<iostream>
#include <iconv.h>
#include<float.h>
using namespace std;
vector<string> splitStr2Vec(string s, string splitSep)//reference:  https://ithelp.ithome.com.tw/articles/10231431
{
    vector<string> result;
    int current = 0; 
    int next = 0;
    while (next != -1)
    {
        next = s.find_first_of(splitSep, current); 
        if (next != current)
        {
            string tmp = s.substr(current, next - current);
            if(!tmp.empty())  
            {
                result.push_back(tmp);
            }
        }
        current = next + 1; 
    }
    return result;
};

class disambig
{
    public:
        disambig();
        void resolver(string inputpath,string mappath,string modelpath,string outputpath);

        void read_map(string mappath);
        void read_model(string modelpath);
        bool is_Zhuin(string alpha);
        // void read_input(string inputpath);
        //string resolve(int column,bool out);

        map<string,vector<string> > mapping;
        map<string,int> model;
        
        //char* buffer_map;
        //char* buffer_model;



};
disambig::disambig()
{
    //buffer_map = new char[5e8];
    //buffer_model = new char[5e8];

};
bool disambig::is_Zhuin(string alpha)
{
    if(int(alpha.substr(0,1)[0])==0xA3)
    {
        if(int(alpha.substr(1,1)[0])>=0x74 && int(alpha.substr(1,1)[0])<=0xB7)return true;
    };
    return false;
}
void disambig:: read_map(string mappath)
{
    printf("read mapping start\n");
    fstream file;
    //printf("before open\n");
    file.open(mappath,ios::in);
    //printf("before big\n");
    //char buffer[];
    //printf("after big\n");
    string BUFFER;
    string pivot;
    vector<string> mapvector;
    //ofstream fout("buffer/exansbuffer.txt");
    
    if(!file)
    {
        printf("open map fail\n");
    }
    else
    {
        while(getline(file,BUFFER))
        {
            //mapvector.push_back(BUFFER);
            //fout<<BUFFER<<endl;
            mapvector = splitStr2Vec(BUFFER," ");
            if(is_Zhuin(mapvector[0]))
            {
                pivot = mapvector[0];
                mapvector.erase(mapvector.begin());
                mapping[pivot] = mapvector; 
            }
            else
            {
                vector<string> storage = mapping[pivot];
                storage.insert(storage.end(),mapvector.begin(),mapvector.end());
                mapping[pivot] = storage;
            };
            
            
        };
        file.close();
        //fout.close();
    };

    
    
}

void disambig:: read_model(string modelpath)
{
    printf("start read model\n");
    fstream file;
    file.open(modelpath,::ios::in);
    string buffer;
    vector<string> modelvector;
    string starter[2] = {"\1-grams:","\2-grams:"};
    // char* starterbig5[2];
    // iconv_t cd = iconv_open("BIG-5", "UTF8");
    bool gram[2] = {false,false};
    if(!file)
    {
        printf("open model failed\n");
    }
    else
    {
        while(getline(file,buffer))
        {
            modelvector = splitStr2Vec(buffer," ");
            
            if(modelvector[0] == starter[0])
            {
                gram[0] = true;
                printf("start 1-gram\n");
                continue;
            }
            else if(modelvector[0] == starter[1])
            {
                gram[1] = true;
                gram[0] = false;
                printf("start 2-gram\n");
                continue;
            }
            else if(!(gram[0] || gram[1]))continue;

            if(gram[0])
            {
                if(modelvector.size()==3)
                {
                    model[modelvector[1]] = atof(modelvector[2].c_str());
                }
                else if(modelvector.size()==2)
                {
                    model[modelvector[1]] = atof(modelvector[0].c_str());
                };
            }
            else if(gram[1])
            {
                model[modelvector[1]+modelvector[2]] = atof(modelvector[0].c_str());   
            }

        };
        file.close();
    };
    printf("model size=%d\n",model.size());
    

};
void disambig::resolver(string inputpath,string mappath,string modelpath,string outputpath)
{
    read_map(mappath);
    read_model(modelpath);
    printf("files read\n");
    fstream file;
    fstream output;
    file.open(inputpath,ios::in);
    output.open(outputpath,ios::out);
    string buffer;
    if(!file)printf("input open failed\n");
    else
    {
        while(getline(file,buffer))
        {
            vector<string> Input = splitStr2Vec(buffer," ");
            vector<vector<string> > possible;
            vector<map<string,double> > prob;
            prob[0][Input[0]] = model[Input[0]];
            map<string,double>connect;
            for(int i=1;i<Input.size();i++)
            {
                possible[i] = mapping[Input[i]];
                
                for(int veter=0;veter<possible[i].size();veter++)
                {
                    prob[i][possible[i][veter]] = -DBL_MAX;
                    for(int cf=0;cf<possible[i-1].size();cf++)
                    {
                        //double connect=0.0;
                        model.count(possible[i-1][cf]+possible[i][veter])==1?connect[possible[i-1][cf]+possible[i][veter]]=model[possible[i-1][cf]+possible[i][veter]]:connect[possible[i-1][cf]+possible[i][veter]]=model[possible[i][veter]];
                        if(prob[i][possible[i][veter]]<prob[i-1][possible[i-1][cf]]+connect[possible[i-1][cf]+possible[i][veter]])prob[i][possible[i][veter]] = prob[i-1][possible[i-1][cf]]+connect[possible[i-1][cf]+possible[i][veter]];//language model
                    };
                };
                

            };
            for(int i=Input.size()-2;i>0;i--)
            {
                // backtrack
                string take = possible[i][0];
                for(int bk=0;bk<possible[i].size();bk++)
                {
                    if(prob[i][take]<prob[i][possible[i][bk]])take=possible[i][bk];
                };
                Input[i] = take;
            };
            string outputstr=Input[0];
            for(int i=1;i<Input.size();i++)outputstr = outputstr + " " + Input[i];
            outputstr = outputstr + "\n";
            output << outputstr;




        };
    };
    file.close();
    output.close();
}
int main(int argc,char* argv[])
{
    disambig dsp_hw3;

    dsp_hw3.resolver(argv[1],argv[2],argv[3],argv[4]);
}