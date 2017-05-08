#ifndef IMAGEANNOTATIONS_H
#define IMAGEANNOTATIONS_H
#include <string>
#include <map>
#include <list>

namespace IA
{
    enum AnnotationFlag
    {
        UNKOWN = 0,
        YES = 1,
        NO = 2,
    };

    class ImageAnnotations;
    class Dir;
    class File;

    typedef int size_type;
    typedef int ID;
    typedef std::map<std::string, File*> StrFileMap;
    typedef std::map<std::string, Dir*> StrDirMap;
    typedef std::list<std::string> StrList;
    typedef std::list<Dir*> DirList;
    typedef std::list<File*> FileList;

    class ImageAnnotations
    {
        StrDirMap dirs;
        std::string annotationsFilePath;
    public:
        ImageAnnotations();
        ~ImageAnnotations();
        void loadFromFile(const std::string &fileName);
        void saveToFile(const std::string &fileName);
        std::string getAnnotationsFilePath() const
        {
            return annotationsFilePath;
        }

        size_type numOfDirs() const;
        size_type numOfFiles(const std::string &dirPath = std::string()) const;

        bool existsDir(const std::string &dirPath) const;
        bool existsFile(const std::string &filePath) const;
        StrList getDirPaths() const;
        StrList getFilePaths(const std::string &dirPath = std::string()) const;
        File *getFile(const std::string &filePath) const;
        Dir* getDir(const std::string&) const;
        DirList getDirs() const;
        FileList getFiles(const std::string &dirPath = std::string()) const;

        void addFile(const std::string &filePath);
        void addFiles(const StrList &filePaths);
        void removeFiles(const StrList &);
        void removeFile(const std::string &);
        void clear();

        // split the path to a file into its dirPath and fileName
        static std::string dirPath(const std::string &filePath);
        static std::string fileName(const std::string &filePath);
        static std::string filePath(const std::string &dirPath, const std::string &fileName);
    };

    class Dir
    {
        friend class ImageAnnotations;
        StrFileMap  files;
        std::string dirPath;
    public:
        Dir(const std::string &dirPath);
        ~Dir();
        std::string getDirPath() const;
        StrList getFileNames() const;
        FileList getFiles() const;
        size_type numOfFiles() const;
        IA::File *getFile(const std::string &fileName) const;
    };

    class File
    {
        friend class ImageAnnotations;
        std::string filePath;
        AnnotationFlag flag; // annotation value;
    public:
        File(const std::string &filePath);
        ~File();
        std::string getFilePath() const;
        AnnotationFlag getFlag() const;
        void setFlag(int);
    };
}

#endif // IMAGEANNOTATIONS_H
