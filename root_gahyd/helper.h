// See route_guide

#ifndef GRPC_COMMON_CPP_ROUTE_GUIDE_HELPER_H_
#define GRPC_COMMON_CPP_ROUTE_GUIDE_HELPER_H_

#include <string>
#include <vector>

namespace rootgahyd {
class Feature;

std::string GetDbFileContent(int argc, char** argv);

void ParseDb(const std::string& db, std::vector<Feature>* feature_list);

}  // namespace routeguide

#endif  // GRPC_COMMON_CPP_ROUTE_GUIDE_HELPER_H_