#include "kdtree_interface.hpp"
#include "args.hxx"
#include "picojson.h"
#include <fstream>
#include <chrono>

template <class T>
std::vector<float> loadbinary(std::string f)
{
    std::ifstream fi(f.c_str(),std::ios::binary);
    if(!fi)
        return std::vector<T> (0);
    fi.seekg (0, std::ios::end);
    auto s = fi.tellg();
    fi.seekg( 0, std::ios::beg );
    std::vector<T> r(s/sizeof(T));
    fi.read((char*)&r[0],r.size()*sizeof(T));
    return r;
}

template <class T>
void savebinary(std::string f, T * p, int n)
{
    std::ofstream fi(f.c_str(),std::ios::binary);
    if(!fi)
        return;
    fi.write((char*)p,n*sizeof(T));
}

int main(int argc, char const *argv[])
{
	args::ArgumentParser parser("KDD tester");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Command aList(parser, "list","list types");
    args::Command aLimits(parser, "limits","limits of type(s)");
    args::Command aAllLimits(parser, "alllimits","limits of all type(s)");
    args::Command aRun(parser, "run","run test");
    args::Command aGenTrain(parser, "gentrain","gen train");
    args::Command aGenTest(parser, "gentest","gen test");
    args::ValueFlag<std::string> aInput(parser, "input", "The input file with rows NxD. Float binary file", {'i'});
    args::ValueFlag<std::string> aTestfile(parser, "test", "The test file with rows MxD. Float binary file (TODO JSON)", {'t'});
    args::ValueFlag<std::string> aOutput(parser, "output", "The output file JSON: points matched", {'o'});
    args::ValueFlag<int> aD(parser, "dim", "The number of dimensions", {'d'});
    args::ValueFlag<int> aK(parser, "k", "The number of neightbors", {'k'});
    args::ValueFlag<int> aS(parser, "s", "The number of outputs for radius search", {'s'});
    args::ValueFlag<int> aMaxLeaf(parser, "maxleaf", "Max leaves", {"maxleaf"});
    args::ValueFlag<float> aR(parser, "r", "The radius", {'r'});
    //args::ValueFlag<float> aEPS(parser, "e", "The epsilon for search", {'e','eps'},0);
    args::ValueFlagList<std::string> aTypes(parser, "types", "The types flag", {'T'});
    args::ValueFlag<bool> abuildfloat(parser, "buildfloat", "Build From Float Tree", {'f', "float"},false);
    args::ValueFlag<int> aitems(parser,"items","Generate items",{"c","count"},1000);
    args::ValueFlag<float> amaxrange(parser,"max_range","Max Range",{"R","maxrange"},1.0);
    args::ValueFlag<float> aminrange(parser,"min_range","Min Range",{"r","minrange"},0.0);

    // TODO: create groundtruth with bruteforce
    // TODO: load/save tree as type T
    // TODO: load/save data as type T

    // Randomize Seed
    srand(time(NULL));

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Completion e)
    {
        std::cout << e.what();
        return 0;
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    if(args::get(aList))
    {
    	for(auto & a: kdtree_any_float_list())
    	{
    		std::cout << " " << a << std::endl;	
    	}
    }
    else if(args::get(aLimits))
    {
        for(auto & t: args::get(aTypes))
        {
            double q[3] = {0,0,0};
            auto p = kdtree_any_float_create(t.c_str());
            if(p)
            {
                p->limits(q);
            }
            std::cout << t << "\t" << q[0] << "\t" << q[1] << "\t" << q[2] << std::endl; 
        }
    }
    else if(args::get(aAllLimits))
    {
        for(auto & t: kdtree_any_float_list())
        {
            double q[3] = {0,0,0};
            auto p = kdtree_any_float_create(t.c_str());
            if(p)
            {
                p->limits(q);
            }
            std::cout << t << "\t" << q[0] << "\t" << q[1] << "\t" << q[2] << std::endl; 
        }
    }    else if(args::get(aGenTrain) || args::get(aGenTest))
    {
        int n = args::get(aitems);
        int dim = args::get(aD);
        float mr = args::get(aminrange);
        float MR = args::get(amaxrange);
        float s = MR-mr;
        std::vector<float> q(n*dim);
        for(int i = 0; i < q.size(); i++)
        {
            q[i] = s * (float(rand() % 1000) / float(1000)) + mr;
        }
        savebinary<float>(args::get(aGenTrain) ? args::get(aInput): args::get(aTestfile),&q[0],q.size()); 
    }
    else if(args::get(aRun))
    {
	    auto si = loadbinary<float>(args::get(aInput));
        auto st = loadbinary<float>(args::get(aTestfile));
        int dim = args::get(aD);
        bool hasoutput = !args::get(aOutput).empty();
        bool buildfloat = args::get(abuildfloat);
        if(dim <= 0)
        {
            std::cerr << "dim required positive" << std::endl;
            return -1;
        }

        typename picojson::array jtests;
        for(auto & t: args::get(aTypes))
        {
            jtests.push_back(picojson::value(picojson::object()));
            typename picojson::object & jtestmap = jtests.back().get<picojson::object>();
            //picojson::value & jtest = 
            jtestmap["type"] = picojson::value(t);

            std::cout << "processing type " << t << std::endl;
            kdtree_any_float * p = kdtree_any_float_create(t.c_str());
            if(!p)
            {
                jtestmap["typeresult"] = picojson::value("missing");
                std::cout << "\tmissing " << t << std::endl;
                continue;
            }
            else
            {
                jtestmap["typeresult"] = picojson::value("ok");
            }
            if(buildfloat)
                p->initFromFloatTree(&si[0],si.size()/dim,dim,args::get(aMaxLeaf));
            else
                p->init(&si[0],si.size()/dim,dim,args::get(aMaxLeaf));
            // measure time
            auto beginB = std::chrono::high_resolution_clock::now();
            bool b = p->build();
            auto endB = std::chrono::high_resolution_clock::now();
            jtestmap["buildtime_us"] = picojson::value((double)std::chrono::duration_cast<std::chrono::microseconds>(endB - beginB).count());
            if(!b)
            {                  
                std::cout << "\tbuild " << t << " failed" << std::endl;
                continue;
            }
            if(args::get(aK) > 0) // K search
            {
                std::vector<size_t> output(args::get(aK));
                std::vector<picojson::value> jps;
                for(int i = 0; i < jps.size()/dim; i++)
                {
                    //jps.push_back(picojson());
                    picojson::value & jp = jps.back();
                    int n = p->knnSearch(args::get(aK),&st[i*dim],&output[0]);
                    jtestmap["type"] = picojson::value(t);
                    // emit
                    //picojson jp
                }
            }
            else if(args::get(aS) > 0) // r search
            {
                std::vector<size_t> output(args::get(aS));
                for(int i = 0; i < st.size()/dim; i++)
                {
                    int n = p->radiusSearch(args::get(aR),&st[i*dim],output.size(),&output[0]);
                    // emit
                }
            }
        }
        if(hasoutput)
        {
            picojson::value jroot(jtests);
            std::string json = jroot.serialize();
            std::ofstream onf(args::get(aOutput),std::ios::binary);
            onf.write(json.c_str(),json.size());
        }
    }
    return 0;
}