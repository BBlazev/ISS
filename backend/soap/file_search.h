//gsoap fs service name:       FileSearch
//gsoap fs service style:      document
//gsoap fs service encoding:   literal
//gsoap fs service namespace:  http://localhost:8081/fs.wsdl
//gsoap fs service location:   http://localhost:8081
#import "stl.h"
#import "stlvector.h"

class fs__FileResult {
  public:
    std::string id;
    std::string filename_download;
    std::string title;
    std::string type;
    LONG64 filesize;
    std::string description;
    std::string uploaded_on;
};

int fs__searchFiles(std::string term, std::vector<fs__FileResult> &result);
int fs__hello(std::string &result);
