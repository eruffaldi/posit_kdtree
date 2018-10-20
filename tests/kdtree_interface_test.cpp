#include "kdtree_interface.hpp"
#include "args.hxx"
#include "picojson.h"
#include <fstream>
#include <chrono>

template <class T>
std::vector<T> loadbinary(std::string f)
{
    std::ifstream fi(f.c_str(),std::ios::binary);
    if(!fi)
        return std::vector<T> ();
    fi.seekg (0, std::ios::end);
    auto s = fi.tellg();
    fi.seekg( 0, std::ios::beg );
    std::vector<T> r(s/sizeof(T));
    fi.read((char*)&r[0],r.size()*sizeof(T));
    if(!fi)
        return std::vector<T> ();
    else
        return r;
}

double distanceL2(float * a, float * b, int n)
{
    double r = 0;
//    std::cout << "<<res\n";
    for(int i = 0; i < n; i++)
    {
  //      std::cout << " " << a[i] << " " << b[i] << std::endl;
        r += fabs(a[i]-b[i]);//*(a[i]-b[i]);
    }
    //std::cout << "res>> " << r << std::endl;
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
    args::Group commands(parser, "commands");
    args::Command aList(commands, "list","list types");
    args::Command aLimits(commands, "limits","limits of type(s)");
    args::Command aAllLimits(commands, "alllimits","limits of all type(s)");
    args::Command aRun(commands, "run","run test");
    args::Command aGenTrain(commands, "gentrain","gen train");
    args::Command aGenTest(commands, "gentest","gen test");
    args::Command aDump(commands, "dumpfile","dump file");
    args::Group arguments(parser, "arguments", args::Group::Validators::DontCare, args::Options::Global);
    args::ValueFlag<std::string> aInput(arguments, "input", "The input file with rows NxD. Float binary file", {'i'});
    args::ValueFlag<std::string> aTestfile(arguments, "test", "The test file with rows MxD. Float binary file (TODO JSON)", {'t'});
    args::ValueFlag<std::string> aOutput(arguments, "output", "The output file JSON: points matched", {'o'});
    args::ValueFlag<int> aD(arguments, "dim", "The number of dimensions", {'d'});
    args::ValueFlag<int> aK(arguments, "k", "The number of neightbors", {'k'});
    args::ValueFlag<int> aS(arguments, "s", "The number of outputs for radius search", {'s'});
    args::ValueFlag<int> aMaxLeaf(arguments, "maxleaf", "Max leaves", {"maxleaf"},10);
    args::ValueFlag<float> aR(arguments, "r", "The radius", {'r'});
    //args::ValueFlag<float> aEPS(parser, "e", "The epsilon for search", {'e','eps'},0);
    args::ValueFlagList<std::string> aTypes(arguments, "types", "The types flag", {'T'});
    args::ValueFlag<bool> abuildfloat(arguments, "buildfloat", "Build From Float Tree", {'f', "float"},false);
    args::ValueFlag<bool> adumpit(arguments, "dumpit", "Dumps data from free", {'D', "dumpit"},false);
    args::ValueFlag<int> aitems(arguments,"items","Generate items",{"c","count"},1000);
    args::ValueFlag<float> amaxrange(arguments,"max_range","Max Range",{"R","maxrange"},1.0);
    args::ValueFlag<float> aminrange(arguments,"min_range","Min Range",{"r","minrange"},0.0);

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
    if(args::get(aDump))
    {
        auto si = loadbinary<float>(args::get(aInput));
        const int dim = args::get(aD);
        if(dim == 0)
        {
            std::cerr << "specify dim\n";
            return 0;
        }
        for(int i = 0; i < si.size()/dim; i++)
        {
            float * pme = &si[i*dim];
            for(int j = 0; j < dim; j++)
                std::cout << pme[j] << " " ;
            std::cout << std::endl;
        }

    }
    else if(args::get(aList))
    {
    	for(auto & a: kdtree_any_float_list())
    	{
    		std::cout << " " << a << std::endl;	
    	}
    }
    else if(args::get(aLimits))
    {
        std::cout << "type\tmax\tmin\tepsilon\n";
        for(auto & t: args::get(aTypes))
        {
            auto p = kdtree_any_float_create(t.c_str());
            if(p)
            {
                int n = p->limits(0,0,0);
                std::vector<double> qd(n);
                std::vector<uint64_t> qr(n);
                p->limits(&qd[0],&qr[0],n);
                std::cout << t << "\t" << qd[0] << "\t" << qd[1] << "\t" << qd[2] << std::endl; 
            }
            else
                std::cout << t << "\tunk\tunk\tunk" << std::endl; 
        }
    }
    else if(args::get(aAllLimits))
    {
        std::cout << "atype\tmax\tmin\tepsilon\n";
        for(auto & t: kdtree_any_float_list())
        {
            auto p = kdtree_any_float_create(t.c_str());
            if(p)
            {
                int n = p->limits(0,0,0);
                std::vector<double> qd(n);
                std::vector<uint64_t> qr(n);
                p->limits(&qd[0],&qr[0],n);
                std::cout << t << "\t" << qd[0] << "\t" << qd[1] << "\t" << qd[2] << std::endl; 
            }
        }
    }    
    else if(args::get(aGenTrain) || args::get(aGenTest))
    {
        const auto output = args::get(aGenTrain) ? args::get(aInput): args::get(aTestfile);
        if(!output.empty())
        {
            const int n = args::get(aitems);
            const int dim = args::get(aD);
            const float mr = args::get(aminrange);
            const float MR = args::get(amaxrange);
            const float s = MR-mr;
            std::vector<float> q(n*dim);
            for(int i = 0; i < q.size(); i++)
            {
                q[i] = s * (float(rand() % 1000) / float(1000)) + mr;
            }
            std::cout << "generating test file " << output << std::endl;
            savebinary<float>(output,&q[0],q.size()); 
        }
    }
    else if(args::get(aRun))
    {
	    auto si = loadbinary<float>(args::get(aInput));
        auto st = loadbinary<float>(args::get(aTestfile));
        if(si.empty())
        {
            std::cerr << "empty input train file " << aInput << std::endl;
            return -1;
        }
        if(st.empty())
        {
            std::cerr << "empty input test file " << aTestfile << std::endl;
            return -1;
        }
        int dim = args::get(aD);
        bool hasoutput = !args::get(aOutput).empty();
        bool buildfloat = args::get(abuildfloat);
        if(dim <= 0)
        {
            std::cerr << "dim required positive" << std::endl;
            return -1;
        }
        std::cout << "Train: " << si.size() << " Test:" << st.size() << " Dim:"<< dim << " With output:" << args::get(aOutput) << "Build Float: " << buildfloat << std::endl;

        typename picojson::array jtests;
        for(auto & t: args::get(aTypes))
        {
            std::cout << "\nprocessing type " << t << std::endl;
            jtests.push_back(picojson::value(picojson::object()));
            typename picojson::object & jtestmap = jtests.back().get<picojson::object>();
            //picojson::value & jtest = 
            jtestmap["type"] = picojson::value(t);

            kdtree_any_float * p = kdtree_any_float_create(t.c_str());
            if(!p)
            {
                jtestmap["typeresult"] = picojson::value("missing");
                std::cout << "\tmissing processing type " << t << std::endl;
                continue;
            }
            else
            {
                jtestmap["typeresult"] = picojson::value("ok");
            }
            std::cerr << "processing type " << t << " found " << std::endl;
            if(buildfloat)
                p->initFromFloatTree(&si[0],si.size()/dim,dim,args::get(aMaxLeaf));
            else
                p->init(&si[0],si.size()/dim,dim,args::get(aMaxLeaf));
            std::cerr << "training data inited" << std::endl;
            if(args::get(adumpit))
            {
                p->dumpData();
                continue;
            }
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
            else
            {
                std::cerr << "built\n";
            }
            if(args::get(aK) > 0) // K search
            {
                std::cerr << "K search" << std::endl;

                std::vector<kdtree_any_float::IndexType> output(args::get(aK));
                std::vector<picojson::value> jps; // for each point
                jtestmap["type"] = picojson::value(t);
                jtestmap["K"] = picojson::value((double)args::get(aK));
                // for every vector                
                std::cout << "evaluating " << st.size()/dim << " points" << std::endl;
                for(int i = 0; i < st.size()/dim; i++)
                {
                    float * pme = &st[i*dim];
                    int n = p->knnSearch(args::get(aK),pme,&output[0]);
                    std::vector<picojson::value> jpi(n);
                    std::vector<picojson::value> jpd(n);
                    picojson::object ri;
                    ri["i"] = picojson::value((double)i);
                    ri["n"] = picojson::value((double)n);
                    for(int q = 0; q < n; q++)
                    {
                        jpd[q] = picojson::value(distanceL2(pme,&si[output[q]*dim],dim));
                        jpi[q] = picojson::value((double)output[q]);
                    }
                    ri["indices"] = picojson::value(jpi);
                    ri["distancesL2"] = picojson::value(jpd);
                    jps.push_back(picojson::value(ri));
                }
                jtestmap["results"] = picojson::value(jps);
            }
            else if(args::get(aS) > 0) // r search
            {
                std::cerr << "R search" << std::endl;
                std::vector<kdtree_any_float::IndexType> output(args::get(aS));
                std::vector<picojson::value> jps; // for each point
                jtestmap["type"] = picojson::value(t);
                jtestmap["radius"] = picojson::value((double)args::get(aR));
                std::cout << "evaluating " << st.size()/dim << " points" << std::endl;
                for(int i = 0; i < st.size()/dim; i++)
                {
                    float * pme = &st[i*dim];
                    int n = p->radiusSearch(args::get(aR),pme,output.size(),&output[0]);
                    std::vector<picojson::value> jpi(n);
                    std::vector<picojson::value> jpd(n);
                    picojson::object ri;
                    ri["i"] = picojson::value((double)i);
                    ri["n"] = picojson::value((double)n);
                    for(int q = 0; q < n; q++)
                    {
                        jpd[q] = picojson::value(distanceL2(pme,&si[output[q]*dim],dim));
                        jpi[q] = picojson::value((double)output[q]);
                    }
                    ri["indices"] = picojson::value(jpi);
                    ri["distancesL2"] = picojson::value(jpd);
                    jps.push_back(picojson::value(ri));
                }
                jtestmap["results"] = picojson::value(jps);
            }
        }
        if(hasoutput)
        {
            std::cerr << "final output" << std::endl;
            picojson::value jroot(jtests);
            std::string json = jroot.serialize(true);
            std::ofstream onf(args::get(aOutput),std::ios::binary);
            onf.write(json.c_str(),json.size());
        }
    }
    return 0;
}