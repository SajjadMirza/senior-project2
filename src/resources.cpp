// function imports object using assimp

#include <resources.hpp>

namespace resource {

   bool import_object(const std::string& file) {
      Assimp::Importer importer;
      const aiScene *scene = importer.ReadFile(file,
                                               aiProcess_CalcTangentSpace       | 
                                               aiProcess_Triangulate            |
                                               aiProcess_JoinIdenticalVertices  |
                                               aiProcess_SortByPType);
   
      if (!scene) {
         std::cerr << importer.GetErrorString() << std::endl;
         return false;
      }

   
      return true;
   }

}
