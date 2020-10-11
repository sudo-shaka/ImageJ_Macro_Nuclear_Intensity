#include <iostream>
#include <string>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;


/*Warning: This was my first attempt at C++ so it is not very elegent, but it does work.. */
 

//returns a vector of csv files found in folder
vector<string> GetCsvFiles(string dir)
{
    DIR *p;
    int i =0;
    struct dirent *pp;
    p = opendir(dir.c_str());
    vector<string> file_list;
    {
        if (p != NULL)
        {
            while ((pp = readdir (p))!=NULL)
            {
                int length = strlen(pp->d_name);
                char filename[255];
                strncpy(filename,pp->d_name,254);
                if (strncmp(filename + length -4, ".csv", 4) == 0)
                {
                    if(pp->d_type == DT_REG)
                    {   
                        file_list.push_back(filename);
                    }
                }
            }
        }
    }
    if (int n = file_list.size() == 0)
    {
        cout << "[!] No .CSV files found... Please try again." << endl;
        exit(1);
    }
    return file_list;
}
//Gets user input for the folder to file files containing data
string BrowseDir() 
{
    char path[1024];
    cout << "Enter path to folder containing your CSV files (Enter '.' for current folder) >> ";
    cin >> path;
    cout << "\n";
    return path;
}

//Gets the 2nd column of a CSV file and returns those values into vector of floats.
vector<float> Get_Means(string filename, string dir)
{   
    vector<float> Means;
    int numLines = 0, x=0;

    string line;
    fstream CSV;
    string path = dir+filename;
    try
    {
        CSV.open(path);
    }
    catch (int e)
    {
        cout << "[!] Error could not open file " << path << endl; 
        exit(1);
    }

    for(numLines = 0; getline(CSV,line); numLines++)
    {
        if(numLines!=0)
        {
            string value;
            int c=0;
            for(x=0;x<line.length();x++)
            {   
                if(line[x] == ',')
                {
                    c++;
                }
                if(c == 1 && line[x] != ',')
                {
                    value = value+line[x];
                }
                else if(c==2)
                {
                    break;
                }
            }
            try
            {
                Means.push_back(stof(value));
            }
            catch(const std::exception& e)
            {
                cerr << e.what() << '\n';
                cout << "[!] Error. Value in data that is not a number!" << endl;
                exit(1);
            }
            
        }
    }
    return(Means);
}
/*Simply removes the last 9 elements of a string. 
Output data from Image J macros all end in "#.oir.csv". 
This is used to find replicates because the first half of the filename should be between replicates*/
string remove_last_9(string l)
{
    string s = l.substr(0,l.size()-9);
    return s;
}

//This uses that described above functionality to get the number of experimental conditions and put them into a list.
vector<string> Get_Condtions(string PATH, vector<string> filename)
{
    int size_v = filename.size(); bool found = false;
    vector<string> condtion_names, done_names;

    for(int i=0;i<size_v;i++)
    {
        string condition = remove_last_9(filename[i]);
        done_names.push_back(condition);
        int j=0;
        while(j <= i-1)
        {
            if(condition == done_names[j])
            {
                found = true;
                break;
            } 
            j++;
        }
        if(!found)
        {
            condtion_names.push_back(condition);
        }
    }

    return condtion_names;

}

//This combines data from different conditions into a master vector with vectors of data taken from each replicate
//** In the future... return a struct with DATA and Conditons names so memeory space of DATA and thier labels are linked
vector<vector<float> > Get_Data(string PATH,vector<string> filename)
{  
    int size_v = filename.size(); 
    int i=0;
    vector<string> condition_names;
    vector<vector<float> > DATA;


    for(i=0;i < size_v; i++)
    {
        string cond_name = remove_last_9(filename[i]);
        condition_names.push_back(cond_name);

        vector<float> Means = Get_Means(filename[i], PATH);

        int j=0; bool found = false;
        
        while(j <= i-1)
        {
            if(i>=1 && condition_names[i] == condition_names[j])
            {
                cout << filename[i] << " ---> " << condition_names[j] << endl;
                found = true;
                for(int k=0;k<Means.size();k++)
                {
                    DATA[j].push_back(Means[k]);
                }
                break;
            }
            j++;
        }

        if(!found)
        {
            cout << "Saving data for " << filename[i] << endl;
            DATA.push_back(Means);
        }
    }

    cout << "\n";
    return DATA;
}

//This calculates the percentage of values over a given threshold
float Get_Percent_Pos(vector<float> nums, float thresh)
{
    int c=0; int t=0;

    for(int i=0; i < nums.size(); i++)
    {
        t++;
        if(nums[i] > thresh)
            c++;
    }
    
    float n = ((float)c/t)*(float)100;

    return n;
}

//Start of the main function. 
//** In future put the input parsing into seperate function and make classes based in functional type.
int main(int argc, char **argv)
{
    string dir;
    char a_p;
    bool b_p;

    if(argv[1] == NULL) //if no CL arguments as the user for some info
    {
        dir = BrowseDir();
        cout << "Do you want to calculate the percent postive cells? (y/n) >> ";
        cin >> a_p;
        if(a_p == 'y')
        {
            b_p = true;
        }
        else if (a_p == 'n')
        {
            b_p = false;
        }
        else
        {
            cout << "[!] Not a valid answer! Only enter lowercase 'y' or 'n'" << endl;
            b_p = false;
            exit(1);
        }
    }
    else //if there are flags parse them to know what to do
    {   
        for(int arg=1; arg < argc; arg++) //let the user know if the right flags aren't there
        {
            if(strcmp(argv[arg],"-d") == 0 || strcmp(argv[1],"--directory") == 0 || strcmp(argv[arg],"-h") == 0 || strcmp(argv[arg],"--help") == 0)
            {
                break;
            }
            else
            {
                cout << "[!] Error! Improper useage of flags!\n\nUsage: ./ImageJ_Macro_Process -d <folder> (-p)\n\nSee --help for more details." << endl;
                exit(1);
            }
            
        }
        for(int arg=1; arg<argc; arg++) //if there are there use that information
        {
            if(strcmp(argv[arg],"-h") == 0 || strcmp(argv[arg],"--help") == 0)
            {
                cout << "Help Menu:\n\t--help (-h) : Shows this prompt!\n";
                cout << "\t--directory (-d) : Path to folder containing exported CSV files from ImageJ Macro\n";
                cout << "\t--percent (-p): Enable this flag to calculate the percent positive cells";  
                exit(1);              
            }
            if(strcmp(argv[arg],"-d") == 0 || strcmp(argv[arg],"--directory") == 0)
            {   
                if(argv[arg+1] != NULL)
                    dir = argv[arg+1];
                else
                {
                    cout << "Please enter name of path after directory flag." << endl;
                    exit(1);
                }
            }
            if(strcmp(argv[arg],"-p") == 0 || strcmp(argv[arg],"--percent") == 0)
            {
                b_p = true;
            }
        }
    }

    //Start the calculations
    dir = dir + "/";
    vector<string> filelist = GetCsvFiles(dir);
    vector<vector<float> > DATA = Get_Data(dir,filelist);   
    vector<string> condition_names = Get_Condtions(dir,filelist);
    
    
    //Here is my strange way of getting the vectors of floats and vector of condition names into a nice csv file
    cout << "Saving data to Master.csv\n";
    try
    {
        ofstream(file);
        file.open("Master.csv",ios::out);  //opening file

        for(int i=0; i < DATA.size(); i++)
        {
            file << condition_names[i] << ","; //make comma separated labels
        }
        
        file << "\n";
        int max=0,pos = 0;
        for(int i=0; i<DATA.size(); i++)   //Gets the largest vector in DATA. that way all values are put into CSV
        {
            if(max < DATA[i].size())
            {
                max = DATA[i].size();
                pos=i;
            }
        }
        
        for(int i=0; i<DATA.at(pos).size(); i++)
        {   
            for(int j=0; j<DATA.size();j++)
            {
                int k=0;
                if (strcmp(typeid(DATA[j][i]).name(),"f") !=0) //if value is a number (float)
                {
                    if(DATA[j][i] > 5 && DATA[j][i] < 100000) //if a number is wayyy to high or too low to not put into CSV
                    {
                        file << DATA[j][i]; //add value to csv
                    }
                }
                file << ","; //serparate values by comma
            }
            file <<"\n"; // print new line & rinse and repeat until CSV file is made
        }
        file.close();
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << '\n';
        cout << "[!] Error exporting/parsing data!";
    }

    if(b_p == true) //if they wanna calculate percent positve cells..
    {
        float v;
        cout << "Please enter threshold value >> "; //get threshhold from user
        try
        {
            cin >> v;
            cout << "\n";
        }
        catch(const std::exception& e)
        {
            cerr << e.what() << '\n';
            cout << "Please only enter a number\n";
            exit(1);
        }
        
        vector<float> percents;
        for(int i=0; i < DATA.size(); i++) //calculate %postive cells and display result
        {
            float percent = Get_Percent_Pos(DATA[i],v); 
            percents.push_back(percent);
            cout << setw(30) << left << condition_names[i] << ":\t" << percent << "%" << endl;
        }

        cout << "\nDo you want to save this data (y/n) >> ";
        char s;
        cin >> s;
        if(s == 'y')
        {
            ofstream(file);
            file.open("Percent_Data.csv", ios::out);
            for(int i=0; i < percents.size(); i++)
            {
                file << condition_names[i] << "," << percents[i] << endl;
            }
            file.close();
        }
        else if(s == 'n')
        {
            cout << "Not saving percent postive data...\n" << endl;
        }
        else
        {
            cout << "[!] Invalid Input. Only enter lowercase 'y' or 'n'" << endl;
        }
        

    }

    cout << "Done!\n";

    return 0;
}
