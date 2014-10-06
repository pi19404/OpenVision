/*
 * ImgUtils.cpp
 *
 *  Created on: 20-Aug-2014
 *      Author: pi19404
 */

#include "ImgUtils.hpp"
Logger logger;
vector<string> ImgUtils::getSubDirs(char *path)
{
	vector<string> name;
	int count=-1;
	DIR           *d1;
	struct dirent *dir1;
    d1 = opendir((const char*)path);
    if(d1)
    {
    count=0;

    while ((dir1 = readdir(d1)) != NULL)
    {
        if(!(strcmp(dir1->d_name,".") ||strcmp(dir1->d_name,"..")))
        continue;


        if (dir1->d_type == DT_DIR) { /* If the entry is a regular file */
        	name.push_back(dir1->d_name);
           }
    }
    closedir(d1);
    }
    return name;


}
string ImgUtils::getBaseDir(string file)
{
	  unsigned found = file.find_last_of("/\\");
		  //std::cout << " path: " << str.substr(0,found) << '\n';
		  string base=file.substr(0,found);
		  return base;
}

string ImgUtils::getBaseName(string file)
{

	  unsigned found = file.find_last_of("/\\");
	  //std::cout << " path: " << str.substr(0,found) << '\n';
	  string base=file.substr(found+1);

	  // Remove extension if present.
	  const size_t period_idx = base.rfind('.');
	  if (std::string::npos != period_idx)
	  {
	      base.erase(period_idx);
	  }
	  return base;
}

int ImgUtils::getDirCount(char *path)
{
	int count=-1;
	DIR           *d1;
	struct dirent *dir1;
    d1 = opendir((const char*)path);
    if(d1)
    {
    count=0;

    while ((dir1 = readdir(d1)) != NULL)
    {
        if(!(strcmp(dir1->d_name,".") ||strcmp(dir1->d_name,"..")))
        continue;


        if (dir1->d_type == DT_DIR) { /* If the entry is a regular file */
        	count++;
           }
    }
    closedir(d1);
    }
    return count;
}

int ImgUtils::getFileExtn(char *name,char *extn)
{
	string file=name;
	string ext=file.substr(file.find_last_of(".") + 1);

	if(ext.length()==0)
		return 1;
	strcpy(extn,ext.c_str());
	return 0;

}
int ImgUtils::getFileCount(const char *path,char *extn)
{
	char ext[100];
	int count=-1;
	DIR           *d1;
	struct dirent *dir1;
    d1 = opendir((const char*)path);
    if(d1)
    {
    count=0;

    while ((dir1 = readdir(d1)) != NULL)
    {
        if(!(strcmp(dir1->d_name,".") ||strcmp(dir1->d_name,"..")))
        continue;



        if (dir1->d_type == DT_REG) { /* If the entry is a regular file */
            if(getFileExtn(dir1->d_name,ext))
            	continue;

            if(strcmp(ext,extn)!=0)
            	continue;

        	count++;
           }
    }
    closedir(d1);
    }
    return count;

}
int ImgUtils::getFileCount(const char *path)
{
		int count=-1;
		DIR           *d1;
    	struct dirent *dir1;
	    d1 = opendir((const char*)path);
	    if(d1)
	    {
	    count=0;

	    while ((dir1 = readdir(d1)) != NULL)
	    {
	        if(!(strcmp(dir1->d_name,".") ||strcmp(dir1->d_name,"..")))
	        continue;


	        if (dir1->d_type == DT_REG) { /* If the entry is a regular file */
	        	count++;
	           }
	    }
	    closedir(d1);
	    }
	    return count;


}

int ImgUtils::dirExists(const char *path)
{
    struct stat info;

    if(stat( path, &info ) != 0)
        return 0;
    else if(info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

int ImgUtils::createDir(const char *path)
{
    if(!ImgUtils::dirExists(path))
    {

    	if(mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)!=0)
    		return 0;
    	else
    		return 1;
    }
    return 0;

}
