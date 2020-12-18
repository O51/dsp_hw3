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
        map<string,double> model;
        
        //char* buffer_map;
        //char* buffer_model;



};
disambig::disambig()
{
    //buffer_map = new char[5e8];
    //buffer_model = new char[5e8];
 
    


}
bool disambig::is_Zhuin(string alpha)
{
string hante[37] = {"ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ","ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄧ","ㄨ","ㄩ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ","ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ"};
for(int i=0;i<37;i++)
{
    if(alpha == hante[i])return true;

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
    //int zhuin_cnt =0;
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
                //zhuin_cnt +=1;
                //printf("Zhuin=%d\n",zhuin_cnt); 
            }
            else if(mapvector[0] == mapvector[1])
            {
                mapping[mapvector[0]] = {mapvector[1]};
            }
            else
            {
                vector<string> storage = mapping[pivot];
                storage.insert(storage.end(),mapvector.begin(),mapvector.end());
                mapping[pivot] = storage;
            };
            
            
        };
        file.close();
        mapping["<s>"] = {"<s>"};
        mapping["</s>"] = {"</s>"};
        mapping["<unk>"] = {"<unk>"};
        printf("map size=%d\n",mapping.size());
        //fout.close();
    };

    
    
}

void disambig:: read_model(string modelpath)
{
    // printf("start read model\n");
    fstream file;
    file.open(modelpath,::ios::in);
    string buffer;
    vector<string> modelvector;
    // string starter[2] = {"\1-grams:","\2-grams:"};
    // char* starterbig5[2];
    // iconv_t cd = iconv_open("BIG-5", "UTF8");
    static bool gram[2] = {false,false};
    int spce=0;
    if(!file)
    {
        printf("open model failed\n");
    }
    else
    {
        while(getline(file,buffer))
        {
            // static int counter=0;
            // counter +=1;
            //printf("model %d line\n",counter);
            modelvector = splitStr2Vec(buffer,"\t");

            // cout<<buffer<<endl;
            // printf("after cout\n");
            // bool a = modelvector[0] == starter[0];
            // printf("a=%d\n",a);

            if(modelvector.empty())
            {
                // printf("space occur\n");
                spce +=1;
                continue;
            }
            else if(spce==2 && gram[0]==false)
            {
                gram[0] = true;
                // printf("start 1-gram\n");
                continue;
            }
            else if(spce==3 && gram[1]==false)
            {
                gram[1] = true;
                gram[0] = false;
                // printf("start 2-gram\n");
                continue;
            }
            else if(!(gram[0] || gram[1]) || spce==4)
            {
                // printf("none\n");
                continue;
            };

            if(gram[0])
            {
                printf("modelvector size=%d\n",modelvector.size());
                if(modelvector.size()==3)
                {
                    model[modelvector[1]] = atof(modelvector[2].c_str());
                    // printf("model added=%f\n",model[modelvector[1]]);
                }
                else if(modelvector.size()==2)
                {
                    model[modelvector[1]] = atof(modelvector[0].c_str());
                    // printf("model added=%f\n",model[modelvector[1]]);
                };
            }
            else if(gram[1])
            {
                printf("modelvector size=%d\n",modelvector.size());
                vector<string> stg1 = splitStr2Vec(modelvector[1]," ");
                if(modelvector.size()+stg1.size()==4)
                {
                    model[stg1[0]+stg1[1]] = atof(modelvector[0].c_str());
                    // printf("model added=%f\n",model[stg1[0]+stg1[1]]);  
                } 
            };

        };
        file.close();
    };
    printf("model size=%d\n",model.size());
    gram[0] = false;
    gram[1] = false;
    

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
            static int count=0;
            count +=1;
            printf("resolving %d line\n",count);
            buffer = "<s> " + buffer +" </s>";
            vector<string> Input = splitStr2Vec(buffer," ");
            printf("splitted\n");
            vector<vector<string> > possible;
            vector<map<string,double> > prob;
            possible.resize(Input.size());
            prob.resize(Input.size());
            possible[0]={Input[0]};
            prob[0][Input[0]] = model[Input[0]];
            map<string,double>connect;
            for(int i=1;i<Input.size();i++)
            {
                if(mapping.count(Input[i]))possible[i] = mapping[Input[i]];
                else
                {
                    printf("mapping error=%s at %d line %d element\n",Input[i],count,i);
                    possible[i] = {"<unk>"};
                }
                
                
                printf("mapped\n");
                
                for(int veter=0;veter<possible[i].size();veter++)
                {
                    prob[i][possible[i][veter]] = -DBL_MAX;
                    for(int cf=0;cf<possible[i-1].size();cf++)
                    {
                        //double connect=0.0;
                        if(model.count(possible[i-1][cf]+possible[i][veter]))connect[possible[i-1][cf]+possible[i][veter]]=model[possible[i-1][cf]+possible[i][veter]];
                        else if(model.count(possible[i][veter]))connect[possible[i-1][cf]+possible[i][veter]]=model[possible[i][veter]];
                        else connect[possible[i-1][cf]+possible[i][veter]]=model["<unk>"];
                        printf("connect found\n");
                        if(prob[i][possible[i][veter]]<prob[i-1][possible[i-1][cf]]+connect[possible[i-1][cf]+possible[i][veter]])prob[i][possible[i][veter]] = prob[i-1][possible[i-1][cf]]+connect[possible[i-1][cf]+possible[i][veter]];//language model
                    };
                };
                

            };
            printf("before backtrack\n");
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
            printf("after backtrac\n");
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