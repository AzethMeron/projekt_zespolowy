#ifndef DIR_HPP
#define DIR_HPP

#include <dirent.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

vector<string> GetFilesInDir( const string& Path ) {
    struct dirent * file;
    DIR * path;
	
	vector<string> output;
	
    if(( path = opendir( Path.c_str() ) ) ) {
        while(( file = readdir( path ) ) )
        {
			string tmp = (file->d_name);
            if(tmp == ".") {} else if (tmp == "..") {} else
            {
				output.push_back(tmp);
			}
		}
             
        closedir( path );
    }
	return output;
}

void RemoveFileFromList(vector<string>& list, const string& Name)
{
	for(unsigned int i = 0; i < list.size(); ++i)
	{
		if(Name == list[i])
		{
			swap(list[i],list.back());
			list.pop_back();
			return;
		}
	}
}

#endif
