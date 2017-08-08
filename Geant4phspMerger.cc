//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//- Geant4PhspMerger is an utility that can be used to merge different phase space came from slaves machines 
//- to make one phase-space file named phsp.(IAEAphsp ,IAEAheader)  
//- Geant4PhspMerger Automatically detect phase-space files in the current directory.after the merge operation   
//- is done the utilility remove all phase-space files .                                                      
//- to compile it in Linux , you must run this line :                                                          
//- cc  Geant4PhspMerger.cc iaea_header.cc iaea_phsp.cc iaea_record.cc utilities.cc -lm -lstdc++ -o  Geant4PhspMerger 
//- Tested in Centos 5.4 with compilers gcc                                                                     
//- Geant4PhspMerger is created by  Jaafar El Bakkali a Phd student                                      
//- ERSN/LMR, University Abdelmalek Essaadi.                                                                    
//- Faculty of Sciences - Tetouan, Morocco.                                                                         
//- Contact: bahmedj@gmail.com 
//- november-2012                                                                           
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 
#include <sys/types.h>
#include <ctime>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include "iaea_phsp.h"
#include "iaea_record.h"
#include <stdio.h>
#include <math.h>

   using namespace std;

  double diffclock(clock_t clock1,clock_t clock2)
  {
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
   } 

   int addPhsp( char* file1,char*  file2)
   {
   // Declaring IAEA phsp and access rights 
   IAEA_I32 source_read = 0, source_write = 1, source_append = 2; 
   IAEA_I32 access_read = 1, access_write = 2, access_append = 3;
   IAEA_I32 result; 
   // Particle properties
   IAEA_I32 type, extra_ints[NUM_EXTRA_LONG];
   IAEA_Float E, wt, x, y, z, u, v, w, extra_floats[NUM_EXTRA_FLOAT];
   // Initializing IAEA source for reading and retrieving logical information
   IAEA_I32 len = 81;
   char* fiaearead; fiaearead =(char *)  file1;
   char* fiaeaappend; fiaeaappend =(char *)  file2;
   IAEA_I64 read_original_particles, append_original_particles;
   iaea_new_source(&source_read , fiaearead , &access_read , &result, len);
   iaea_get_total_original_particles(&source_read, &read_original_particles);
   iaea_new_source(&source_append, fiaeaappend, &access_append, &result, len );
   iaea_get_total_original_particles(&source_append, &append_original_particles);
   append_original_particles += read_original_particles;
   iaea_set_total_original_particles(&source_append, &append_original_particles); 
   result = -1; // To request for the total number of particles
   IAEA_I64 histories;
   iaea_get_max_particles(&source_read, &result, &histories); 
   // printf("\n Total number of histories to read: %lld\n",histories);
   IAEA_I64 irecord;
   IAEA_I64 nrecorded = 0;
   IAEA_I32 n_stat;   
   for(irecord=0; irecord<histories; irecord++)
   {
       // read IAEA particle
       iaea_get_particle(&source_read, &n_stat,
              &type, /* particle type */
              &E   , /* kinetic energy in MeV */
              &wt  , /* statistical weight */
              &x   ,
              &y   ,
              &z   , /* position in cartesian coordinates*/
              &u   ,
              &v   ,
              &w   , /* direction in cartesian coordinates*/
              extra_floats, extra_ints);
       if( n_stat == -1 ) break; 
       // write IAEA particle
       iaea_write_particle(&source_append, &n_stat,
              &type, /* particle type */
              &E   , /* kinetic energy in MeV */
              &wt  , /* statistical weight */
              &x   ,
              &y   ,
              &z   , /* position in cartesian coordinates*/
              &u   ,
              &v   ,
              &w   , /* direction in cartesian coordinates*/
              extra_floats, extra_ints);
       nrecorded++;
   }


   iaea_destroy_source(&source_read,&result);
   iaea_destroy_source(&source_append,&result);
   return 0;
   }
   string ANSI_RESET = "\033[0m";
   string ANSI_RED = "\033[31m";
   string ANSI_BLUE = "\033[34m";
   string ANSI_GREEN = "\033[32m";
    int getdir (string dir, vector<string> &files)
    {
    DIR *dp;
    struct dirent *dirp;
    cout<<ANSI_BLUE + "\n>> phase-space files in the current directory are :\033[0m"  << endl;

    if((dp  = opendir(dir.c_str())) == NULL) 
    {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
    }
    while ((dirp = readdir(dp)) != NULL)
    {
    files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
    }


     string & replace(string & mystring, string myOldSubString, string myNewSubString)
     {
     size_t  StringToFind = mystring.find(myOldSubString);
     if (StringToFind != string::npos)
     {
     mystring.erase(StringToFind, myOldSubString.size());
     mystring.insert(StringToFind, myNewSubString);
     }
     return mystring;
     }

     int deleteAllFiles(string file)
     { 
    string extension_header=".IAEAheader";
    string extension_phsp=".IAEAphsp";
    string file_header =file+extension_header;
    string file_phsp =file+extension_phsp;
    remove(file_header.c_str());
    remove(file_phsp.c_str());
    }

    int renamefile(string file)
    { 
    string extension_header=".IAEAheader";
    string extension_phsp=".IAEAphsp";
    string file_header =file+extension_header;
    string file_phsp =file+extension_phsp;
    rename(file_header.c_str(),"phsp.IAEAheader");
    rename(file_phsp.c_str(),"phsp.IAEAphsp");
    }

   int main()
   {
   cout<<ANSI_GREEN+"------------------------------------------------------------------------";
   cout<<ANSI_GREEN+ " \n|->Geant4phspMerger v1.0 , developed  by Jaafar El Bakkali,Phd student .\n|->ERSN/LMR, University Abdelmalek Essaadi.\n|->Faculty of Sciences - Tetouan, Morocco [november 2012]. "  << endl;
   cout<<ANSI_GREEN+"------------------------------------------------------------------------"+ ANSI_RESET;
    clock_t begin=clock();
    static int NumberOfPhaseSpace=0;
    string dir = string(".");
    vector<string> files = vector<string>();
    vector<string> phspFiles= vector<string>();
    getdir(dir,files);

    for (unsigned int i = 0;i < files.size();i++) 
    {
    string fn = files[i];
    if(fn.substr(fn.find_last_of(".") + 1) == "IAEAheader")
    {
    replace(fn, ".IAEAheader", "");
    cout << fn << endl;
    phspFiles.push_back(fn);
    NumberOfPhaseSpace=NumberOfPhaseSpace+1;
    } 
    }

    if(NumberOfPhaseSpace==0) {
    cout<<ANSI_RED +"Error: phase-space files not found !"<<"\033[0m"<<endl;
    return 0; }

    cout<<ANSI_BLUE+">> number of phase-space files to be merged is \033[0m "  <<NumberOfPhaseSpace<<endl;

    for(unsigned int k=1; k < phspFiles.size(); k++) 
    {
    char *  file_to_accumulate = (char*)phspFiles[k].c_str(); 
    char * receiver_file=(char*) phspFiles[0].c_str();  
    // method to add  n-1 phase-spaces to the primary one in the vector of this files
    addPhsp(file_to_accumulate,receiver_file);
    }
    string filemerged= phspFiles[0];
    FILE *fp;
    char filename[80];
    long length;
    string filen=filemerged+".IAEAphsp";
    fp=fopen(filen.c_str(),"rb");
    fseek(fp,0L,SEEK_END);
    length=ftell(fp) ;
    double MB_length = length / 1048576.0;
    renamefile(filemerged);
    cout <<ANSI_BLUE+">> all data files accumulated in the file : \033[0m phsp.IAEAphsp"<<endl;
    cout <<ANSI_BLUE+">> the file's length is\033[0m "<<MB_length<<" Mio ( "<<length<<" octets )"<<endl;
    for(unsigned int l=1; l < phspFiles.size(); l++) 
    {
    string  file_to_delete = phspFiles[l]; 
    deleteAllFiles(file_to_delete);
    cout <<ANSI_BLUE+">> removing phase-space file : \033[0m"  <<phspFiles[l]<<"'"<<endl;
    }
   clock_t end=clock();
   cout <<ANSI_BLUE+">> elapsed Time: \033[0m" << double(diffclock(end,begin)) << " ms "<< endl;
   cout <<ANSI_BLUE+">> The operation completed successfully !\033[0m "  <<endl;
   return 0;
   }

