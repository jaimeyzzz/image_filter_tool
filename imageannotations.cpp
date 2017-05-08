#include "imageannotations.h"

// C++ STD Lib
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <vector>

// Boost Lib
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

// using namespaces
using namespace IA;
using std::string;
//using __gnu_cxx::hash_map;
using std::map;
using std::list;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::algorithm::starts_with;
using boost::algorithm::trim;
using boost::algorithm::split;
using boost::algorithm::to_lower;
using boost::algorithm::to_lower_copy;
using boost::algorithm::is_any_of;
using boost::algorithm::contains;

using namespace IA;
using namespace std;

IA::ImageAnnotations::ImageAnnotations()
{

}

IA::ImageAnnotations::~ImageAnnotations()
{
    // delete all allocated Dir references .. the rest is done automatically
    typedef std::pair<string, Dir*> thePair;
    BOOST_FOREACH(thePair entry, dirs)
        delete entry.second;
}

void IA::ImageAnnotations::loadFromFile(const string &filePath)
{
    // try to open the file
        std::ifstream in(filePath.c_str());
        if (!in) {
            cerr << "IA::ImgAnnotations::loadFromFile()" << endl;
            cerr << "\tError opening the file: " << filePath << endl;
            return;
        }

        // save annotations file path
        annotationsFilePath = filePath;

        // clear the current content
        clear();

        // loop over all lines
        string currentFilePath;
        string line;
        int lineCounter = 0;
        bool error = false;
        File* currentFile = 0;
        while (!std::getline(in, line).eof()) {
            // read next line
            trim(line);
            lineCounter++;

            // ignore empty lines and comment lines
            if (line.empty() || starts_with(line, "#") || !contains(line, ":"))
                continue;

            // split the line in its key and value (up to ':' and the value)
            string::size_type found = line.find(':');
            string key(line, 0, found);
            string value;
            if (found != string::npos) value = string(line, ++found, string::npos);
            trim(key);
            to_lower(key);
            trim(value);

            // decide what to do based on the key value
            if ("file" == key)
            {
                // add a new empty file .. if it doesn't exist already
                if (!existsFile(value))
                    addFile(value);

                // update currentFile and currentObj (to zero, since we added a new file)
                currentFilePath = value;
                currentFile = getFile(currentFilePath);
            }
            else if ("type" == key)
            {
                if (!currentFile)
                    continue;
                currentFile->setFlag(atoi(value.c_str()));
            }
        }

        if (error) {
            cerr << "IA::ImgAnnotations::loadFromFile()" << endl;
            cerr << "\tOne or more errors occured due to invalid object ids, new ids have been choosen." << endl;
            cerr << "\tWARNING: If you use references to IDs this is going to result in a mess!" << endl;
            cerr << "\t         I assume you know what you do :) !" << endl;
        }
}

void IA::ImageAnnotations::saveToFile(const string &filePath)
{
    // try to open the file
    std::ofstream out(filePath.c_str(), std::ios::trunc);
    if (!out) {
        cerr << "IA::ImgAnnotations::saveToFile()" << endl;
        cerr << "\tError opening the file: " << filePath << endl;
        return;
    }

    // save annotations file path
    annotationsFilePath = filePath;

    // loop over all elements in our datastructur and write their data to the file
    BOOST_FOREACH (File *iFile, getFiles()) {
        // output the fileName
        assert(NULL != iFile);
        out << "########## NEW FILE ##########\n";
        out << "file: " << iFile->getFilePath() << "\n";
        out << "type: " << iFile->getFlag() << "\n";
        out << "\n";
    }
}

size_type IA::ImageAnnotations::numOfDirs() const
{
    return dirs.size();
}

size_type IA::ImageAnnotations::numOfFiles(const string &dirPath) const
{
    size_type num = 0;

    // if the dir is not given, return number of all existing files
    if (dirPath.empty()) {
        BOOST_FOREACH (const Dir *iDir, getDirs())
            num += iDir->numOfFiles();
    }
    // otherwise return number of all files in the specified directory
    else if (existsDir(dirPath)) {
        const Dir *dir = getDir(dirPath);
        assert(NULL != dir);
        num = dir->numOfFiles();
    }

    return num;
}


bool IA::ImageAnnotations::existsDir(const std::string &dirPath) const
{
    return NULL != getDir(dirPath);
}

bool IA::ImageAnnotations::existsFile(const std::string &filePath) const
{
    return NULL != getFile(filePath);
}

StrList IA::ImageAnnotations::getDirPaths() const
{
    StrList outList;
    BOOST_FOREACH(Dir *dir, getDirs())
        outList.push_back(dir->getDirPath());
    return outList;
}

StrList IA::ImageAnnotations::getFilePaths(const string &dirPath) const
{
    StrList outList;

    BOOST_FOREACH(File *file, getFiles(dirPath))
        outList.push_back(file->getFilePath());

    return outList;
}

IA::File *IA::ImageAnnotations::getFile(const string &filePath) const
{
    File *file = NULL;

    // get the file name and the dir path
    string dirPath = this->dirPath(filePath);
    string fileName = this->fileName(filePath);

    // get the pointer to the corresponding dir
    Dir *dir = getDir(dirPath);

    // get the pointer to the corresponding file
    if (NULL != dir)
        file = dir->getFile(fileName);

    return file;
}

IA::Dir* IA::ImageAnnotations::getDir(const string &dirPath) const
{
    Dir *dir = NULL;
    StrDirMap::const_iterator iDirPath = dirs.find(dirPath);
    if (iDirPath != dirs.end())
        dir = iDirPath->second;
    return dir;
}
DirList IA::ImageAnnotations::getDirs() const
{
    DirList dirList;
    typedef std::pair<string, Dir*> thePair;
    BOOST_FOREACH (const thePair entry, dirs)
    {
        dirList.push_back(entry.second);
    }

    return dirList;
}

FileList IA::ImageAnnotations::getFiles(const string &dirPath) const
{
    FileList fileList;

    // if the dir is not given, return all existing files
    if (dirPath.empty()) {
        BOOST_FOREACH (Dir *iDir, getDirs()) {
            FileList tmpList = iDir->getFiles();
            fileList.splice(fileList.end(), tmpList);
        }
    }
    // otherwise return all files in the specified directory
    else if (existsDir(dirPath)) {
        Dir *dir = getDir(dirPath);
        assert(NULL != dir);
        return dir->getFiles();
    }

    return fileList;
}

void IA::ImageAnnotations::addFile(const string &filePath)
{
    // only add the file if the file does not already exist
    if (!existsFile(filePath)) {
        string dirPath = this->dirPath(filePath);
        string fileName = this->fileName(filePath);

        // add a new directory if it does not already exist
        // otherwise get its reference
        Dir *dir = NULL;
        if (!existsDir(dirPath)) {
            dir = new Dir(dirPath);
            dirs[dirPath] = dir;
        }
        else
            dir = getDir(dirPath);

        // add the file
        dir->files[fileName] = new File(filePath);
    }
}

void IA::ImageAnnotations::addFiles(const StrList& filePaths)
{
    // add all given files to the database
    BOOST_FOREACH (string filePath, filePaths)
        addFile(filePath);
}

void IA::ImageAnnotations::removeFile(const string &filePath)
{
    // only remove the file if the file does exist
    if (existsFile(filePath)) {
        string fileName = this->fileName(filePath);
        string dirPath = this->dirPath(filePath);

        // get the file and its directory
        File *file = getFile(filePath);
        assert(NULL != file);
        Dir *dir = getDir(dirPath);
        assert(NULL != dir);

        // delete the file entry from its directory
        StrFileMap::size_type nRemoves = dir->files.erase(fileName);
        assert(nRemoves == 1);
        delete file;

        // delete the directory in case it is now empty
        if (dir->numOfFiles() <= 0) {
            assert(NULL != getDir(dirPath));
            StrDirMap::size_type nRemoves = dirs.erase(dirPath);
            assert(nRemoves == 1);
            delete dir;
        }
    }
}

void IA::ImageAnnotations::removeFiles(const StrList &filePaths)
{
    // remove all given files from the database
    BOOST_FOREACH (string filePath, filePaths)
        removeFile(filePath);
}

void IA::ImageAnnotations::clear()
{
    dirs.clear();
}

string IA::ImageAnnotations::dirPath(const string &filePath)
{
    string::size_type pos = filePath.rfind('/', string::npos);
    if (pos == string::npos)
        return "";
    return filePath.substr(0, pos);
}

string IA::ImageAnnotations::fileName(const string &filePath)
{
    string::size_type pos = filePath.rfind('/', string::npos);
    if (pos == string::npos)
        return "";
    return filePath.substr(pos + 1);
}

string IA::ImageAnnotations::filePath(const string &dirPath, const string &fileName)
{
    string outStr(dirPath);
    outStr.append("/");
    outStr.append(fileName);
    return outStr;
}

/**********
 * Dir
 */
IA::Dir::Dir(const string &dirPath)
{
    this->dirPath = dirPath;
}

IA::Dir::~Dir()
{
    // delete all allocated File references .. the rest is done automatically
    typedef std::pair<string, File*> thePair;
    BOOST_FOREACH(thePair entry, files)
        delete entry.second;
}

string IA::Dir::getDirPath() const
{
    return this->dirPath;
}

StrList IA::Dir::getFileNames() const
{
    StrList outList;
    typedef std::pair<string, File*> thePair;
    BOOST_FOREACH(const thePair entry, files)
        outList.push_back(entry.first);
    return outList;
}

FileList IA::Dir::getFiles() const
{
    FileList outList;
    typedef std::pair<string, File*> thePair;
    BOOST_FOREACH(const thePair entry, files)
        outList.push_back(entry.second);
    return outList;
}

size_type IA::Dir::numOfFiles() const
{
    return files.size();
}

IA::File *IA::Dir::getFile(const string &fileName) const
{
    File *file = NULL;
    StrFileMap::const_iterator iFile = files.find(fileName);
    if (iFile != files.end())
        file = iFile->second;
    return file;
}

/**********
 * File
 */
IA::File::File(const string &filePath)
{
    this->filePath = filePath;
    this->flag = AnnotationFlag::UNKOWN;
}

IA::File::~File()
{
}

string IA::File::getFilePath() const
{
    return filePath;
}

AnnotationFlag IA::File::getFlag() const
{
    return flag;
}

void IA::File::setFlag(int flag)
{
    this->flag = (AnnotationFlag)flag;
}
