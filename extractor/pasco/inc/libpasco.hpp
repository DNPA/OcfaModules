#include<string>
#include<vector>

class URLRec {
  public:
  URLRec(std::string type, std::string url, std::string ascmodtime, std::string ascaccesstime, std::string filename, std::string dirname, std::string httpheaders )
    {
       Type = type;
       Url = url;
       Ascmodtime = ascmodtime ;
       Ascaccesstime = ascaccesstime;
       Filename = filename;
       Dirname = dirname ;
       Httpheaders = httpheaders ;

    }
    std::string Type;
    std::string Url;
    std::string Ascmodtime ;
    std::string Ascaccesstime ;
    std::string Filename ;
    std::string Dirname ;
    std::string Httpheaders ;
};

int pascomain(  std::string filename, bool deleted, std::string ieversion, std::vector< URLRec * > &urls );
